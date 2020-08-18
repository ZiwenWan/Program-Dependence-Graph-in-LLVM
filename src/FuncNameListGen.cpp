#ifndef LLVM_TEST_H
#define LLVM_TEST_H

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/GlobalVariable.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"

#include "DebugInfoUtils.hpp"
#include "PDGUtils.hpp"
#include <vector>
#include <set>
#include <queue>
#include <string.h>
#include <map>
#include <fstream>
#include <sstream>

namespace
{
  using namespace llvm;

  class llvmTest : public ModulePass
  {
  public:
    static char ID;

    llvmTest() : ModulePass(ID)
    {
    }

    StoreInst *getArgStore(Argument *arg)
    {
      for (auto UI = arg->user_begin(); UI != arg->user_end(); ++UI)
      {
        if (StoreInst *st = dyn_cast<StoreInst>(*UI))
        {
          return st;
        }
      }
      return nullptr;
    }

    AllocaInst *getArgAllocaInst(Argument *arg)
    {
      StoreInst *st = getArgStore(arg);
      if (st == nullptr)
      {
        return nullptr;
      }

      if (AllocaInst *ai = dyn_cast<AllocaInst>(st->getPointerOperand()))
        return ai;

      return nullptr;
    }

    std::string getDIFieldName(DIType *ty)
    {
      if (ty == nullptr)
        return "void";
      switch (ty->getTag())
      {
      case dwarf::DW_TAG_member:
      {
        return ty->getName().str();
      }
      case dwarf::DW_TAG_array_type:
      {
        ty = dyn_cast<DICompositeType>(ty)->getBaseType().resolve();
        return "arr_" + ty->getName().str();
      }
      case dwarf::DW_TAG_pointer_type:
      {
        std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
        return s;
      }
      case dwarf::DW_TAG_subroutine_type:
        return "func ptr";
      case dwarf::DW_TAG_const_type:
      {
        std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
        return s;
      }
      default:
      {
        if (!ty->getName().str().empty())
          return ty->getName().str();
        return "no name";
      }
      }
    }

    DIType *getBaseDIType(DIType *Ty)
    {
      if (Ty == nullptr)
        return nullptr;

      if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
          Ty->getTag() == dwarf::DW_TAG_member ||
          Ty->getTag() == dwarf::DW_TAG_typedef)
      {
        DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
        if (!baseTy)
        {
          errs() << "Type : NULL - Nothing more to do\n";
          return nullptr;
        }
        return baseTy;
      }
      return Ty;
    }

    DIType *getLowestDIType(DIType *Ty)
    {
      if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
          Ty->getTag() == dwarf::DW_TAG_member ||
          Ty->getTag() == dwarf::DW_TAG_typedef ||
          Ty->getTag() == dwarf::DW_TAG_const_type)
      {
        DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
        if (!baseTy)
        {
          errs() << "Type : NULL - Nothing more to do\n";
          return NULL;
        }

        //Skip all the DINodes with DW_TAG_typedef tag
        while ((baseTy->getTag() == dwarf::DW_TAG_typedef ||
                baseTy->getTag() == dwarf::DW_TAG_const_type ||
                baseTy->getTag() == dwarf::DW_TAG_pointer_type))
        {
          if (DIType *temp = dyn_cast<DIDerivedType>(baseTy)->getBaseType().resolve())
            return temp;
          else
            break;
        }
        return baseTy;
      }
      return Ty;
    }

    std::set<Value *> getReachableUsers(Value &V)
    {
      errs() << "getting reachable user for " << V << "\n";
      std::set<Value *> ret;
      std::set<Value *> seenVals;
      std::queue<Value *> workQ;
      workQ.push(&V);
      while (!workQ.empty())
      {
        auto V = workQ.front();
        workQ.pop();
        if (seenVals.find(V) != seenVals.end())
          continue;
        seenVals.insert(V);
        for (auto user : V->users())
        {
          if (user != nullptr)
          {
            ret.insert(user);
            workQ.push(user);
          }
        }
      }
      return ret;
    }

    bool canReachInterfaceFuncs(GlobalVariable &GV, std::set<Function *> interfaceFuncs)
    {
      if (!GV.getType()->isPointerTy() && !GV.getType()->getPointerElementType()->isStructTy())
        return false;
      std::queue<Value *> workQ;
      std::set<Value *> seenVals;
      std::set<Instruction* > allocInstSet;
      workQ.push(&GV);
      while (!workQ.empty())
      {
        auto V = workQ.front();
        workQ.pop();
        // for store instruction, there is no user, we need to figuer out the stored address
        if (seenVals.find(V) != seenVals.end())
          continue;
        seenVals.insert(V);

        // if it reaches to the parameter, and the parameter is for one of the interface function, then
        // this struct also become shared
        // deal with the situation that a func struct is assigned to a field of another struct
        // first, we backtrace to the alloc of the struct which the func struct is assigend to
        // then, we follow def-use chain to check if the assigend struct can be passed to interface function

        if (AllocaInst *ai = dyn_cast<AllocaInst>(V))
        {
          allocInstSet.insert(ai);
          continue;
        }

        Value *sourceVal = getDataFlowSourceVal(V);
        if (sourceVal != nullptr)
          workQ.push(sourceVal);

        for (auto user : V->users())
        {
          // chase the def-use chain to find if the data flow can flow to interface functions
          if (CallInst *ci = dyn_cast<CallInst>(user))
          {
            if (Function *calledFunc = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
            {
              if (interfaceFuncs.find(calledFunc) != interfaceFuncs.end())
                return true;
            }
          }
          if (user != nullptr)
            workQ.push(user);
        }
      }

      auto &pdgUtils = pdg::PDGUtils::getInstance();
      auto sharedDataTypes = pdg::DIUtils::collectSharedDITypes(*module, pdgUtils.computeCrossDomainFuncs(*module));
      // check if alloc inst can reach interface function
      for (auto ai : allocInstSet)
      {
        auto reachableUsers = getReachableUsers(*ai);
        for (auto u : reachableUsers)
        {
          if (CallInst *ci = dyn_cast<CallInst>(u))
          {
            if (Function *calledFunc = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
            {
              if (interfaceFuncs.find(calledFunc) != interfaceFuncs.end())
              {
                return true;
              }
            }
          }
        }

        // if it reaches parent func parameter, then consider it is shared.
        Function *parentFunc = ai->getFunction();
        for (auto &arg : parentFunc->args())
        {
          auto argAlloc = getArgAllocaInst(&arg);
          if (argAlloc == ai)
            return true;
        }
      }

      return false;
    }

    Value *getDataFlowSourceVal(Value *val)
    {
      if (auto si = dyn_cast<StoreInst>(val))
        return si->getPointerOperand();
      if (auto li = dyn_cast<LoadInst>(val))
        return li->getPointerOperand();
      if (auto gep = dyn_cast<GetElementPtrInst>(val))
        return gep->getPointerOperand();
      if (auto castI = dyn_cast<BitCastInst>(val))
        return castI->getOperand(0);
      return nullptr;
    }

    std::set<GlobalVariable *> getCrossDomainGlobals(Module &M, std::set<Function *> interfaceFuncs)
    {
      std::set<GlobalVariable *> ret;
      for (auto &Global : M.getGlobalList())
      {
        // first, analyze the uses of this global
        if (canReachInterfaceFuncs(Global, interfaceFuncs))
          ret.insert(&Global);
      }
      return ret;
    }

    bool runOnModule(Module &M)
    {
      module = &M;
      for (auto &F : M)
      {
        for (const Use &use : F.uses())
        {
          const User *user = use.getUser();
          if (isa<BlockAddress>(user))
            continue;
          const auto *Call = dyn_cast<CallInst>(user);
          if (!Call)
            continue;
          if (Call->getCalledFunction())
            continue;
          /* if (!Call->isCallee(&use)) { */
          errs() << "!  " << *Call << " - " << F.getName() << "\n";
          /* } */
        }
      }

      std::ofstream imported_func("imported_func.txt");
      std::ofstream defined_func("defined_func.txt");
      std::ofstream static_funcptr("static_funcptr.txt");
      std::ofstream static_func("static_func.txt");
      std::ofstream lock_func("lock_func.txt");
      std::ofstream driver_globalvars("driver_globalvars.txt");

      lock_func << "spin_lock\n";
      lock_func << "spin_lock_irq\n";
      lock_func << "mutex_lock\n";

      // print driver globals
      for (auto &globalVar : M.globals())
      {
        if (globalVar.getType()->isPointerTy())
        {
          if (globalVar.getType()->getPointerElementType()->isStructTy())
          {
            // auto globalVarDIType = pdg::DIUtils::getGlobalVarDIType(globalVar);
            // driver_globalvars << pdg::DIUtils::getDITypeName(globalVarDIType) <<"\n";
            driver_globalvars << globalVar.getName().str() <<"\n";
          }
        }
      }

      driver_globalvars.close();
      std::set<Function *> importedFuncs;
      for (auto &F : M)
      {
        auto funcName = F.getName().str();

        if (F.isIntrinsic())
          continue;

        if (F.isDeclaration())
        {
          imported_func << funcName << "\n";
          importedFuncs.insert(&F);
          if (funcName.find("lock") != std::string::npos)
            lock_func << funcName << "\n";
          continue;
        }

        defined_func << funcName << "\n";
      }

      imported_func.close();
      defined_func.close();

      // generate statically defined function, normally, these functions are passed from driver to kernel
      // through function pointers.
      auto crossDomainFuncStructs = getCrossDomainGlobals(M, importedFuncs);
      SmallVector<DIGlobalVariableExpression *, 4> sv;
      for (auto Global : crossDomainFuncStructs)
      {
        DIGlobalVariable *gv = nullptr;
        Global->getDebugInfo(sv);
        /* errs() << Global.getName() << " " << sv.size() << "\n"; */
        for (auto d : sv)
        {
          if (d->getVariable()->getName() == Global->getName())
          {
            gv = d->getVariable(); // get global variable from global expression
          }
        }

        if (gv == nullptr)
          continue;

        auto gvDItype = gv->getType().resolve();
        if (getLowestDIType(gvDItype)->getTag() != dwarf::DW_TAG_structure_type)
          continue;

        const auto &typeArrRef = dyn_cast<DICompositeType>(getLowestDIType(gvDItype))->getElements();
        if (Global->getType()->isPointerTy())
        {
          auto pointerEle = Global->getType()->getPointerElementType();
          /* errs() << typeArrRef.size() << " - " << pointerEle->getStructNumElements() << "\n"; */
          if (pointerEle->isStructTy())
          {
            for (unsigned i = 0; i < pointerEle->getStructNumElements(); ++i)
            {
              if (!Global->hasInitializer())
                continue;
              auto cons = Global->getInitializer()->getAggregateElement(i);
              if (cons != nullptr)
              {
                if (cons->hasName())
                {
                  static_funcptr << getDIFieldName(dyn_cast<DIType>(typeArrRef[i])) << "\n";
                  static_func << cons->getName().str() << "\n";
                }
              }
            }
          }
        }
      }
      static_funcptr.close();
      static_func.close();

      return false;
    }

    // Analysis Usage, specify PDG at this time
    void getAnalysisUsage(AnalysisUsage &AU) const
    {
      AU.addRequired<AAResultsWrapperPass>();
      AU.addRequired<MemoryDependenceWrapperPass>();
      /* AU.addRequired<CFLSteensAAWrapperPass>(); */
      /* AU.addRequired<CFLAndersAAWrapperPass>(); */
      AU.addRequired<TargetLibraryInfoWrapperPass>();
      AU.setPreservesAll();
    }

  private:
    /* AliasAnalysis *AA; */
    /* CFLSteensAAResult *steensAA; */
    /* CFLAndersAAResult *andersAA; */
    std::vector<std::string> aliasName;
    Module* module;
  };

  char llvmTest::ID = 0;
  static RegisterPass<llvmTest> llvmTest("gen-func-list", "LLVM TEST", false, true);
} // namespace
#endif