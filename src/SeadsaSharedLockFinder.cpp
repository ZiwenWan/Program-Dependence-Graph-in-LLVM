#ifndef SeadsaSharedLockFinder
#define SeadsaSharedLockFinder

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include <llvm/Support/raw_ostream.h>

#include "sea_dsa/CallGraphUtils.hh"
#include "sea_dsa/DsaAnalysis.hh"
#include "sea_dsa/Global.hh"
#include "sea_dsa/Local.hh"
#include "sea_dsa/Info.hh"
#include "sea_dsa/support/Debug.h"
#include "llvm/Pass.h"

#include "DebugInfoUtils.hpp"
#include "PDGUtils.hpp"
#include "AccessInfoTracker.hpp"

#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <stack>
/*
The general lock finding algorithm is as follow:
1. find shared objects. 
2. find all critical sections.
3. find all the store instruction within critical sections. 
4. check if the modified pointer may points to shared objects. 
*/

namespace pdg
{
using namespace llvm;

class SeadsaWarningGenerator : public ModulePass
{
public:
  static char ID;
  SeadsaWarningGenerator() : ModulePass(ID)
  {
  }

  bool runOnModule(Module &M)
  {
    ACT = &getAnalysis<AccessInfoTracker>();
    unsigned ptrInModule = countPtrInModule(M);
    errs() << "total pointer in module: " << ptrInModule << "\n";
    atomicRegionWarn.open("atomicRegionWarn.txt", std::ios::app);
    // seperate kernel and driver functions
    seperateKernelDriverFunctions(M);
    // collects boundary functions 
    std::set<std::string> boundaryFunctions = findBoundaryFunctions();
    // set up lock and unlock instruction map
    lockUnlockMap.insert(std::make_pair("mutex_lock", "mutex_unlock"));
    lockUnlockMap.insert(std::make_pair("_raw_spin_lock", "_raw_spin_unlock"));
    lockUnlockMap.insert(std::make_pair("_raw_spin_lock_irq", "_raw_spin_unlock_irq"));
    // 1. collects pointers to shared data
    // alias analysis is done through sea-dsa.
    m_dsa = &getAnalysis<sea_dsa::DsaAnalysis>();
    // CCG = &getAnalysis<sea_dsa::CompleteCallGraph>();
    std::set<Value *> ptrToSharedData = collectPtrToSharedData(boundaryFunctions, M);
    // 2. identify critical sections
    // using CDG to compute corresponding lock/unlock pairs instead of CFG.
    std::set<std::pair<Instruction*, Instruction*>> criticalSections = collectCS(M);
    // std::ofstream sharedLockWarn("sharedLockWarning.txt");
    // sharedLockWarn << "CS size in module: " << criticalSections.size() << "\n";
    // 3. collects store instructions in CS and test if shared data can be modified 
    unsigned warningCount = 0;
    unsigned warningCSNum = 0;
    unsigned privateLockCS = 0;
    std::set<Instruction*> seenLockInsts;
    for (auto CS : criticalSections)
    {
      // Eliminate critcal sections that use private lock
      if (usePrivateLock(CS))
      {
        privateLockCS++;
        continue;
      }

      // TODO: Eliminate warnings that has following cross-domain calls that synchronize the modified states
      auto instInCS = collectInstructionsInCS(CS);
      for (Instruction* inst : instInCS)
      {
        if (StoreInst *st = dyn_cast<StoreInst>(inst))
        {
          auto storedToVal = st->getPointerOperand();
          if (ptrToSharedData.find(storedToVal) != ptrToSharedData.end())
          {
            //TODO: use IDL to synchronized fields
            // if (modifyStatesAreSynchronized(CS))
            //   continue;
            // here one possible case is that the modification happens in the other domain. But this modification should be 
            // captured by the IDL. So, we should identify whether a modifiaiton is cross-domain modification. If so, then
            // no warning need to be generated. Otherwise, if it is a modifcation in the same domain, we need to check if this 
            // modifcation will be synchronized accross domain by following calls to the other domain. If so, no warning need to be generated, otherwise, generate warning.
            Function* CSFunction = CS.first->getFunction(); // get function that contain the critical section
            // bool inDriver = (driverFunctions.find(CSFunction) != driverFunctions.end());
            // std::set<Function*> searchDomain = inDriver ? kernelFunctions : driverFunctions;
            // Function* modifyFunc = st->getFunction();
            // if (searchDomain.find(modifyFunc) != searchDomain.end()) // this means a modification happens in a cross-domain function.
            //   continue;

            if (seenLockInsts.find(CS.first) != seenLockInsts.end())
              continue;
            seenLockInsts.insert(CS.first);
            warningCSNum++;

            // warningCount += 1;
            // sharedLockWarn << "Warning: " << warningCount << "\n";
            // Value* sourcePtr = aliasMap[storedToVal];
            // assert(sourcePtr != nullptr);
            // std::string str1;
            // raw_string_ostream ss1(str1); 
            // ss1 << (*sourcePtr);
            // sharedLockWarn << "[source pointer]: " << str1 << "\n";
            // if (Argument *arg = dyn_cast<Argument>(sourcePtr))
            // {
            //   sharedLockWarn << "[source pointer type]: argument" << "\n";
            //   sharedLockWarn << "[Arg function]: " << arg->getParent()->getName().str() << "\n";
            // }
            // if (Instruction *i = dyn_cast<Instruction>(sourcePtr))
            // {
            //   sharedLockWarn << "[source pointer type]: variable " << "\n";
            //   sharedLockWarn << "[Var function]: " << i->getFunction()->getName().str() << "\n";
            // }

            // sharedLockWarn << "[shared data modifed function]: " << st->getFunction()->getName().str() << "\n";

            // std::string str2;
            // raw_string_ostream ss2(str2);
            // ss2 << (*st);
            // sharedLockWarn << "[modify store inst]: " << str2 << "\n";
            // if (CSFunction != modifyFunc)
            //   sharedLockWarn << "[call graph]: " << CSFunction->getName().str() << " -> " << modifyFunc->getName().str() << "\n";
            // // print more informative information such as file name, modify location in the source code
            // // print source code location
            // printDebugLoc(st, CS, sharedLockWarn);
            // // print modified data name if possible. Basically, keep on back tracking the shared data.
            // printModifiedData(st, sharedLockWarn);
            // // here, try to get alloca site in the whole program
            // // sea_dsa::Graph *funcGraph = &m_dsa->getDsaAnalysis().getGraph(*st->getFunction());
            // // printAllocSites(st, funcGraph);
            // sharedLockWarn << " ------------------------------------------------ \n";
          }
        }
      }
    }
    // sharedLockWarn.close();
    atomicRegionWarn << "warning critical sections: " << warningCSNum << "\n";
    atomicRegionWarn << "private critical sections: " << privateLockCS << "\n";
    // sharedDataTypeMap = getAnalysis<AccessInfoTracker>().getSharedDataTypeMap();
    // // start processing for atomic operation
    // auto &pdgUtils = PDGUtils::getInstance();
    // crossDomainFuncs = pdgUtils.computeCrossDomainFuncs(M);
    // sharedDataTypes = DIUtils::computeSharedDataType(M, pdgUtils.computeCrossDomainFuncs(M));
    // errs() << "number of shared type: " << sharedDataTypes.size() << "\n";
    printWarningsForAtomicOperation(M, ptrToSharedData);
    atomicRegionWarn.close();
    return false;
  }

  std::set<Function *> computeFunctionsNeedPDGConstruction(Module &M)
  {
    std::set<Function *> funcSet;
    auto &pdgUtils = PDGUtils::getInstance();
    pdgUtils.constructFuncMap(M);
    // 1. get the set of cross-domain functions.
    std::set<Function *> crossDomainFuncs = pdgUtils.computeCrossDomainFuncs(M);
    // 2. for each cross-domain function, compute its transitive closure
    for (auto F : crossDomainFuncs)
    {
      // pdgUtils.categorizeInstInFunc(*F);
      auto transFuncs = pdgUtils.computeTransitiveClosure(*F);
      funcSet.insert(transFuncs.begin(), transFuncs.end());
    }

    return funcSet;
  }

  void printWarningsForAtomicOperation(Module &M, std::set<Value *> ptrToSharedData)
  {
    unsigned warningNum = 1;
    auto funcsNeedPDGConstruction = computeFunctionsNeedPDGConstruction(M);
    errs() << "func num need compute atomic warnings: "  << funcsNeedPDGConstruction.size() << "\n";
    for (auto func : funcsNeedPDGConstruction)
    {
      if (func->isDeclaration() || func->empty())
        continue;
      for (auto instI = inst_begin(func); instI != inst_end(func); instI++)
      {
        if (isAtomicOperation(&*instI))
        {
          auto modifiedAddrVar = instI->getOperand(0);
          if (ptrToSharedData.find(modifiedAddrVar) != ptrToSharedData.end())
            warningNum++;
          // printWarningForSharedVarInAtomicOperation(*modifiedAddrVar, *instI, *func, warningNum);
        }
      }
    }
    atomicRegionWarn << "Warning Atomic Operation Num: " << warningNum << "\n";
  }

  void printWarningForSharedVarInAtomicOperation(Value& modifiedAddrVar, Instruction& atomicOp, Function& F, unsigned warningNum)
  {
    auto disp = F.getSubprogram();
    if (!isa<Instruction>(&modifiedAddrVar))
      return;
    Instruction *i = cast<Instruction>(&modifiedAddrVar);
    std::string modifiedDataName = getModifiedDataName(i);
    if (modifiedDataName.empty())
      return;
    errs() << " ------------------------------------------------------- \n";
    errs() << "[WARNING " << warningNum << " | ATOMIC OPERATION ON SHARED DATA]: \n";
    errs() << "Modify in " << disp->getFilename() << " in function " << F.getName() << "\n";
    errs() << "Modified Name: " << modifiedDataName << "\n";
    errs() << "Line Number: " << atomicOp.getDebugLoc()->getLine() << "\n";
    errs() << "Modify instruction: " << atomicOp << "\n";
    printExecutionTrace(F);
  }

  void printExecutionTrace(Function &F)
  {
    auto &pdgUtils = PDGUtils::getInstance();
    for (auto crossDomainFunc : crossDomainFuncs)
    {
      auto transitiveFuncs = pdgUtils.computeTransitiveClosure(*crossDomainFunc);
      if (transitiveFuncs.find(&F) != transitiveFuncs.end())
      {
        std::vector<Function*> funcTrace;
        std::set<Function*> seenFuncs;
        printTrace(*crossDomainFunc, F, funcTrace, seenFuncs);
      }
    }
  }

  void printTrace(Function &sourceF, Function &targetF, std::vector<Function*> funcTrace, std::set<Function*> seenFuncs)
  {
    if (sourceF.isDeclaration() || sourceF.empty())
      return;
    if (seenFuncs.find(&sourceF) != seenFuncs.end())
      return;
    seenFuncs.insert(&sourceF);
    funcTrace.push_back(&sourceF);
    // find target
    if (&sourceF == &targetF)
    {
      funcTrace.push_back(&targetF);
      for (auto f : funcTrace)
      {
        errs() << f->getName() << "->";
      }
      errs() << "\n";
      return;
    }
    auto &pdgUtils = PDGUtils::getInstance();
    auto funcMap = pdgUtils.getFuncMap();
    auto callInstList = funcMap[&sourceF]->getCallInstList();
    for (auto CI : callInstList)
    {
      if (Function *calledFunc = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts())) // handle case for bitcast
      {
        printTrace(*calledFunc, targetF, funcTrace, seenFuncs);
      }
    }
  }

  bool isAllocaForArg(Value *val)
  {
    if (AllocaInst *ai = dyn_cast<AllocaInst>(val))
    {
      Function* func = ai->getFunction();
      auto dbgInstList = collectDbgInstInFunction(*func);
      auto dbi = DIUtils::getDbgInstForInst(ai, dbgInstList);
      if (!dbi)
        return false;
      DILocalVariable* DLV = dbi->getVariable();
      if (DLV->isParameter() && !DLV->getName().empty() && DLV->getScope()->getSubprogram() == func->getSubprogram())
        return true;
    }
    return false;
  }

  std::set<DbgDeclareInst *> collectDbgInstInFunction(Function &F)
  {
    std::set<DbgDeclareInst *> ret;
    for (auto instI = inst_begin(&F); instI != inst_end(&F); ++instI)
    {
      if (DbgDeclareInst *dbi = dyn_cast<DbgDeclareInst>(&*instI))
        ret.insert(dbi);
    }
    return ret;
  }

  bool isAtomicOperation(Instruction* inst)
  {
    if (CallInst *ci = dyn_cast<CallInst>(inst))
    {
      if (!ci->isInlineAsm()) 
        return false;
      if (InlineAsm *ia = dyn_cast<InlineAsm>(ci->getCalledValue()))
      {
        auto asmString = ia->getAsmString();
        if (isAtomicAsmString(asmString)) 
          return true;
      }
    }
    return false;
  }

  bool isAtomicAsmString(std::string str)
  {
    return (str.find("lock") != std::string::npos);
  }

  unsigned countPtrInModule(Module &M)
  {
    unsigned ptrNum = 0;
    for (auto &F : M)
    {
      for (auto it = inst_begin(F); it != inst_end(F); ++it)
      {
        if (it->getType()->isPointerTy())
          ptrNum += 1;
      }
    }
    return ptrNum;
  }

  void seperateKernelDriverFunctions(Module &M)
  {
    std::ifstream driverFuncs("defined_func.txt");
    // find driver functions
    for (std::string line; std::getline(driverFuncs, line);)
    {
      Function* f = M.getFunction(StringRef(line));
      if (f->isDeclaration())
        continue;
      driverFunctions.insert(f);
    }
    // find kernel functions
    for (Function &F : M)
    {
      if (F.isDeclaration())
        continue;
      if (driverFunctions.find(&F) == driverFunctions.end())
        kernelFunctions.insert(&F);
    }
  }

  // bool usePrivateLock(std::pair<Instruction *, Instruction *> CS)
  // {
  //   auto &pdgUtils = PDGUtils::getInstance();
  //   auto instMap = pdgUtils.getInstMap();
  //   CallInst *lockInst = dyn_cast<CallInst>(CS.first);
  //   assert((lockInst != nullptr) && "cannot find private lock for lockInst that with null call target.");
  //   Function *lockInstParentFunc = lockInst->getFunction();
  //   Value *lock = getUsedLock(lockInst);
  //   while (lock && !isa<LoadInst>(lock))
  //   {
  //     lock =  getDataFlowSourceVar(lock);
  //     if (lock && isa<LoadInst>(lock))
  //       break;
  //   }

  //   auto PDG = ACT->_getPDG();
  //   if (lock != nullptr)
  //   {
  //     if (Instruction *i = dyn_cast<Instruction>(lock))
  //     {
  //       auto valDepList = PDG->getNodesWithDepType(instMap[i], DependencyType::VAL_DEP);
  //       for (auto pair : valDepList)
  //       {
  //         auto depInstW = pair.first->getData();
  //         DIType *nodeDIType = depInstW->getDIType();
  //         if (nodeDIType != nullptr)
  //         {
  //           auto sharedDataTypeMap = ACT->getSharedDataTypeMap();
  //           std::string DITypeName = DIUtils::getDITypeName(nodeDIType);
  //           if (sharedDataTypeMap.find(DITypeName) != sharedDataTypeMap.end())
  //             return true;
  //         }
  //       }
  //     }
  //   }
  //   return false;
  // }

  bool usePrivateLock(std::pair<Instruction*, Instruction*> CS)
  {
    CallInst* lockInst = dyn_cast<CallInst>(CS.first);
    assert((lockInst != nullptr) && "cannot find private lock for lockInst that with null call target.");
    Function *lockInstParentFunc = lockInst->getFunction();
    // collects all lock call instructions in the other domain
    bool inKernelDomain = (kernelFunctions.find(lockInstParentFunc) != kernelFunctions.end());
    // Search the opposite domain when lock call is foudn in one domain
    std::set<Function* > searchDomain = !inKernelDomain ? kernelFunctions : driverFunctions;
    for (Function *F : searchDomain)
    {
      for (auto instI = inst_begin(F); instI != inst_end(F); ++instI)
      {
        if (CallInst *CI = dyn_cast<CallInst>(&*instI))
        {
          //check if we handle this lock call. Also, handle nullptr called functions
          if (!isLockCall(CI))
            continue;

          std::string lockName1 = lockInst->getCalledFunction()->getName().str();
          std::string lockName2 = CI->getCalledFunction()->getName().str();

          if (lockName1 != lockName2)
            continue;

          if (lockUnlockMap.find(lockName1) == lockUnlockMap.end() || lockUnlockMap.find(lockName2) == lockUnlockMap.end())
            continue;

          Value *lock1 = getUsedLock(lockInst);
          Value *lock2 = getUsedLock(CI);
          if (mustAlias(*lock1, *lockInst->getFunction(), *lock2, *F)) // if aliases are found in the other domain, then this is not a private lock.
            return false;
        }
      }
    }
    return true;
  }

  Value *getUsedLock(Instruction *I)
  {
    CallInst* CI = dyn_cast<CallInst>(I);
    assert((CI != nullptr) && "get used lock fail. Cannot find used lock instance.");
    Value *lock = CI->getArgOperand(0); // the first arg is the used lock
    assert(lock != nullptr && "used lock is nullptr.");
    return lock;
  }

  bool isLockCall(CallInst* CI)
  {
    if (CI->getCalledFunction() == nullptr)
      return false;
    std::string calledFuncName = CI->getCalledFunction()->getName().str();
    if (lockUnlockMap.find(calledFuncName) != lockUnlockMap.end())
      return true;
    return false;
  }

  std::set<std::string> findBoundaryFunctions()
  {
    std::set<std::string> boundaryFunctions;
    std::ifstream kernelInterfaceFuncs("imported_func.txt");
    std::ifstream driverInterfaceFuncs("static_func.txt");

    for (std::string line; std::getline(kernelInterfaceFuncs, line);)
    {
      boundaryFunctions.insert(line);
    }

    for (std::string line; std::getline(driverInterfaceFuncs, line);)
    {
      boundaryFunctions.insert(line);
    }

    assert(boundaryFunctions.size() != 0 && "no boundary functions are found...");
    return boundaryFunctions;
  }

  std::set<std::pair<Instruction *, Instruction *>> collectCS(Module &M)
  {
    // a list of locking functions we are looking for
    std::set<std::pair<Instruction*, Instruction*>> csInModule;
    auto reachableFuncs = computeFunctionsNeedPDGConstruction(M);
    for (auto F : reachableFuncs)
    {
      if (F->isDeclaration())
        continue;
      auto csInFunc = collectCSInFunc(*F); // find cs in each defined functions
      csInModule.insert(csInFunc.begin(), csInFunc.end());
    }
    return csInModule;
  }

  std::set<std::pair<Instruction *, Instruction *>> collectCSInFunc(Function &F)
  {
    std::set<std::pair<Instruction *, Instruction *>> csInFunc;
    for (inst_iterator I = inst_begin(F); I != inst_end(F); ++I)
    {
      // 1. find all lock instruction that call to acquire a lock
      if (CallInst *CI = dyn_cast<CallInst>(&*I))
      {
        auto calledFunc = CI->getCalledFunction();
        if (calledFunc == nullptr)
          continue;
        std::string calledFuncName = calledFunc->getName();
        if (lockUnlockMap.find(calledFuncName) == lockUnlockMap.end())
          continue;
        std::set<BasicBlock *> bbInFunc = getReachableBBInFunc(*CI->getParent(), *CI->getFunction());
        std::set<Instruction *> unlockInFunc = findUnlockInBBs(lockUnlockMap[calledFuncName], bbInFunc, CI);
        for (Instruction *unlockCall : unlockInFunc)
        {
          csInFunc.insert(std::make_pair (CI, unlockCall));
        }
      }
    }
    return csInFunc;
  }

  std::set<BasicBlock *> getReachableBBInFunc(BasicBlock &from, Function &F)
  {
    std::set<BasicBlock*> bbInFunc;
    std::queue<BasicBlock *> bbQ;
    bbQ.push(&from);
    bbInFunc.insert(&from); // a BB can reach itself
    // keep track of seen block. Avoid go through loop infinite amount of times
    while (!bbQ.empty())
    {
      BasicBlock* BB = bbQ.front();
      bbQ.pop();
      auto terminator = BB->getTerminator();
      if (!terminator)
        continue;
      for (unsigned i = 0; i < terminator->getNumSuccessors(); ++i)
      {
        BasicBlock* succ = terminator->getSuccessor(i);
        if (bbInFunc.find(succ) != bbInFunc.end()) // avoid loop
          continue;
        bbQ.push(succ);
        bbInFunc.insert(succ);
      }
    }
    return bbInFunc;
  }

  std::set<Instruction *> findUnlockInBBs(std::string targetUnlockName, std::set<BasicBlock*> searchBBs, Instruction *lockCall)
  {
    std::set<Instruction *> reachableUnlocks;
    for (BasicBlock *BB : searchBBs)
    {
      for (Instruction &I : *BB)
      {
        if (isPredecessorInst(&I, lockCall))
          continue;
        if (CallInst *unlockCall = dyn_cast<CallInst>(&I))
        {
          Function *calledFunc = unlockCall->getCalledFunction();
          if (!calledFunc)
            continue;
          // TODO: check predecessor
          std::string unlockCallName = calledFunc->getName().str();
          if (targetUnlockName == unlockCallName)
            reachableUnlocks.insert(unlockCall);
        }
      }
    }
    return reachableUnlocks;
  }

  bool isPredecessorInst(Instruction *inst1, Instruction *inst2)
  {
    while (inst1->getNextNode())
    {
      if (inst1 == inst2)
        return true;
      inst1 = inst1->getNextNode();
    }
    return false;
  }

  std::set<Value *> collectPtrToSharedData(std::set<std::string> boundaryFunctions, Module &M)
  {
    std::set<Value *> ptrToSharedData;
    // collect global pointers
    std::ifstream driverGlobalVars("driver_globalvars.txt");
    for (std::string line; std::getline(driverGlobalVars, line);)
    {
      auto globalVar = M.getGlobalVariable(StringRef(line));
      if (!globalVar)
        continue;
      if (!globalVar->user_empty())
      {
        if (globalVar->getType()->isPointerTy())
          ptrToSharedData.insert(globalVar);
      }
    }
    driverGlobalVars.close();

    // collect pointer type parameters in interface functions
    for (std::string boundaryFunc : boundaryFunctions)
    {
      Function *func = M.getFunction(StringRef(boundaryFunc));
      if ((func == nullptr) || func->isDeclaration())
        continue;
      for (auto argI = func->arg_begin(); argI != func->arg_end(); ++argI)
      {
        auto argVal = dyn_cast<Value>(argI);
        if (!argVal->getType()->isPointerTy())
          continue;
        ptrToSharedData.insert(argVal);
        // find alias in the whole module
        auto aliasPtrs = findGlobalAlias(M, *argVal, *func);
        ptrToSharedData.insert(aliasPtrs.begin(), aliasPtrs.end());
      }
    }
    errs() << "ptr to shared data size: " << ptrToSharedData.size() << "\n";
    return ptrToSharedData;
  }

  std::set<Value *> findGlobalAlias(Module &M, Value &sourceVal, Function &sourceValFunc)
  {
    std::set<Value *> globalAlias;
    for (Function &F : M)
    {
      if (F.isDeclaration() || &F == &sourceValFunc)
        continue;
      auto aliasInFunc = findAliasInFunc(sourceValFunc, sourceVal, F);
      globalAlias.insert(aliasInFunc.begin(), aliasInFunc.end());
    }
    return globalAlias;
  }

  std::set<Value *> findAliasInFunc(Function &sourceFunc, Value &sourceVal, Function &targetFunc)
  {
    std::set<Value *> alias;
    alias.insert(&sourceVal);
    // for each inst in target function, get its possible allocators and compare it with the sourceAllocs of sourceVal
    for (auto instI = inst_begin(targetFunc); instI != inst_end(targetFunc); ++instI)
    {
      if (&*instI == &sourceVal)
        continue;
      if (!sourceVal.getType()->isPointerTy() || !instI->getType()->isPointerTy())
        continue;
      if (!isa<LoadInst>(&*instI) && !isa<GetElementPtrInst>(&*instI))
        continue;
      if (mayAlias(sourceVal, sourceFunc, *instI, targetFunc))
      {
        alias.insert(&*instI);
        aliasMap.insert(std::make_pair(&*instI, &sourceVal));
      }
    }
    return alias;
  }

  bool mayAlias(Value &p1, Function &F1, Value &p2, Function &F2)
  {
    sea_dsa::Graph *G1 = &m_dsa->getDsaAnalysis().getGraph(F1);
    sea_dsa::Graph *G2 = &m_dsa->getDsaAnalysis().getGraph(F2);
    if (!G1 || !G2)
      return false;
    if (!G1->hasCell(p1) || !G2->hasCell(p2))
      return false;
    auto const &c1 = G1->getCell(p1);
    auto const &c2 = G2->getCell(p2);
    auto const &s1 = c1.getNode()->getAllocSites();
    auto const &s2 = c2.getNode()->getAllocSites();
    for (auto const a1 : s1)
    {
      if (s2.find(a1) != s2.end())
        return true;
    }
    return false;
  }

  bool mustAlias(Value &p1, Function &F1, Value &p2, Function &F2)
  {
    sea_dsa::Graph *G1 = &m_dsa->getDsaAnalysis().getGraph(F1);
    sea_dsa::Graph *G2 = &m_dsa->getDsaAnalysis().getGraph(F2);
    if (!G1 || !G2)
      return false;

    auto const &c1 = G1->getCell(p1);
    auto const &c2 = G2->getCell(p2);
    auto const &s1 = c1.getNode()->getAllocSites();
    auto const &s2 = c2.getNode()->getAllocSites();
    for (auto const a1 : s1)
    {
      if (s2.find(a1) != s2.end())
      {
        unsigned c1Offset = c1.getOffset();
        unsigned c2Offset = c2.getOffset();
        if (c1Offset == c2Offset)
          return true;
      }
    }
    return false;
  }

  std::set<Instruction *> collectInstructionsInCS(std::pair<Instruction *, Instruction *> CS)
  {
    std::set<Instruction *> retInstSet;
    Instruction *lockCallInst = CS.first;
    Instruction *unlockCallInst = CS.second;
    BasicBlock* lockBB = lockCallInst->getParent();
    BasicBlock* unlockBB = unlockCallInst->getParent();
    std::queue<BasicBlock*> bbQ;
    std::set<BasicBlock *> seenBBs; // used to avoid loop
    bbQ.push(lockBB);
    while (!bbQ.empty())
    {
      BasicBlock* bb = bbQ.front();
      bbQ.pop();
      // avoid loop
      if (seenBBs.find(bb) != seenBBs.end())
        continue;
      seenBBs.insert(bb);
      // collect instructions in BB. Take care of the case when lock and unlock are in the same BB.
      if (bb == unlockBB)
      {
        for (auto &I : *bb)
        {
          // eliminate instructions that before the lock call inst and after unlock call inst
          if (isPredecessorInst(&I, lockCallInst))
            continue;
          if (!isPredecessorInst(&I, unlockCallInst))
            continue;
          if (&I == unlockCallInst)
            break;
          retInstSet.insert(&I);
          if (CallInst *CI = dyn_cast<CallInst>(&I))
          {
            Function *calledFunc = CI->getCalledFunction();
            if (calledFunc == nullptr || calledFunc->isDeclaration())
              continue;
            BasicBlock &entryBlock = calledFunc->getEntryBlock();
            bbQ.push(&entryBlock);
          }
        }
      }
      else
      {
        for (auto &I : *bb)
        {
          retInstSet.insert(&I);
          if (CallInst *CI = dyn_cast<CallInst>(&I))
          {
            Function *calledFunc = CI->getCalledFunction();
            if (calledFunc == nullptr || calledFunc->isDeclaration())
              continue;
            BasicBlock &entryBlock = calledFunc->getEntryBlock();
            bbQ.push(&entryBlock);
          }
        }
      }
      // push successors
      auto terminator = bb->getTerminator();
      if (!terminator)
        continue;
      for (unsigned i = 0; i < terminator->getNumSuccessors(); ++i)
      {
        BasicBlock* succ = terminator->getSuccessor(i);
        bbQ.push(succ);
      }
    }
    return retInstSet;
  }

  // void printCallGraph(CallSite &CS)
  // {
  //   for (auto it = CCG->begin(CS); it != CCG->end(CS); ++it)
  //   {
  //     errs() << "call graph node: " << (*it)->getName() << "\n";
  //   }
  // }

  // =================== WARNING PRINT FUNCTIONS =======================
  void printAllocSites(StoreInst* st, sea_dsa::Graph* funcGraph)
  {
    if (!funcGraph->hasCell(*st->getPointerOperand()))
      return;
    auto const &cell = funcGraph->getCell(*st->getPointerOperand());
    auto const &allocSites = cell.getNode()->getAllocSites();
    errs() << "[Possible Allocators ]: \n";
    for (auto const allocSite : allocSites)
    {
      if (const Instruction *inst = dyn_cast<Instruction>(allocSite))
        errs() << "\t [" << inst->getFunction()->getName() << "]: " << *allocSite << "\n";
    }
    errs() << "\n";
  }

  void printDebugLoc(StoreInst *inst, std::pair<Instruction *, Instruction *> CS, std::ofstream &sharedLockWarn)
  {
    auto DILoc = inst->getDebugLoc();
    sharedLockWarn << "[Source file name]: " << DILoc->getFilename().str() << "\n";
    sharedLockWarn << "[Line number]: " << DILoc->getLine() << "\n";
    sharedLockWarn << "[CS Range]: ";
    sharedLockWarn << CS.first->getDebugLoc()->getLine() << " -> ";
    sharedLockWarn << CS.second->getDebugLoc()->getLine() << "\n";
    sharedLockWarn << "CS location: " << CS.first->getFunction()->getName().str() << "\n";
  }

  void printModifiedData(Instruction* inst, std::ofstream &sharedLockWarn)
  {
    std::string dataName = getModifiedDataName(inst);
    sharedLockWarn << "[Modified Data]: " << dataName << "\n";
  }

  int getGepOffset(GetElementPtrInst *gep)
  {
    int operand_num = gep->getNumOperands();
    llvm::Value *last_idx = gep->getOperand(operand_num - 1);
    if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx))
      return constInt->getSExtValue();
    return -1;
    // assert(false && "Get GEP offset fail.");
  }

  Value *getDataFlowSourceVar(Value *val)
  {
    // if (auto ai = dyn_cast<AllocaInst>(val))
    //   return getDataFlowSourceForAllocaInst(ai);
    if (auto si = dyn_cast<StoreInst>(val))
      return si->getValueOperand();
    if (auto li = dyn_cast<LoadInst>(val))
      return li->getPointerOperand();
    if (auto gep = dyn_cast<GetElementPtrInst>(val))
      return gep->getPointerOperand();
    if (auto castI = dyn_cast<BitCastInst>(val))
      return castI->getOperand(0);
    return nullptr;
  }

  Value *getDataFlowSourceForAllocaInst(AllocaInst *ai)
  {
    for (auto user : ai->users())
    {
      if (StoreInst *si = dyn_cast<StoreInst>(user))
      {
        if (si->getPointerOperand() == ai)
        {
          if (ai->getFunction()->getName() == "ixgbe_alloc_q_vector")
            errs() << "find user: " << *user << "\n";
          return si->getValueOperand();
        }
      }
    }
    return nullptr;
  }

  Instruction *getSourceInst(Instruction *inst)
  {
    if (auto si = dyn_cast<StoreInst>(inst))
      return dyn_cast<Instruction>(si->getPointerOperand());
    if (auto li = dyn_cast<LoadInst>(inst))
      return dyn_cast<Instruction>(li->getPointerOperand());
    if (auto gep = dyn_cast<GetElementPtrInst>(inst))
      return dyn_cast<Instruction>(gep->getPointerOperand());
    if (auto castI = dyn_cast<BitCastInst>(inst))
      return dyn_cast<Instruction>(castI->getOperand(0));
    return nullptr;
  }

  Argument *getArgByAlloc(Instruction *argAlloc)
  {
    for (auto user : argAlloc->users())
    {
      if (auto st = dyn_cast<StoreInst>(user))
      {
        if (Argument *arg = dyn_cast<Argument>(st->getValueOperand()))
          return arg;
      }
    }
    return nullptr;
  }

  std::string getModifiedDataName(Instruction *inst)
  {
    std::stack<Instruction *> gepStk;
    // while (inst && !isa<AllocaInst>(inst))
    std::set<Instruction*> seenInsts;
    while (inst && !isAllocaForArg(inst))
    {
      if (seenInsts.find(inst) != seenInsts.end())
        break;
      seenInsts.insert(inst);

      if (isa<GetElementPtrInst>(inst))
        gepStk.push(inst); // store all gep instruction seen by far
      auto sourceVar = getDataFlowSourceVar(inst);
      if (!sourceVar || !isa<Instruction>(sourceVar))
        return "";  
      inst = cast<Instruction>(sourceVar);
    }
    
    if (inst == nullptr) return "";

    Argument *arg = getArgByAlloc(inst);
    // in this case, we can simply print out this inst as the shared data cannot be traced back to arg
    if (!arg && inst != nullptr)
      errs() << "No arg | "  << *inst << "\n";

    // TODO: some back tracing goes to a alloca for non-arg IR variable. The reason is that some arg state is stored to some temporary IR variables. A step of alias analysis is needed here.
    if (!arg) return "";

    DIType *argDIType = DIUtils::getArgDIType(*arg);
    DIType *dt = DIUtils::getLowestDIType(argDIType);
    if (!dt || !DIUtils::isStructTy(dt))
      return "";
    while (!gepStk.empty())
    {
      Instruction *gep = gepStk.top();
      gepStk.pop();
      dt = DIUtils::getLowestDIType(dt);
      if (!dt) return "";
      if (!DIUtils::isStructTy(dt)) return "";
      if (auto dicp = dyn_cast<DICompositeType>(dt))
      {
        auto DINodeArr = dicp->getElements();
        int gepOffset = getGepOffset(dyn_cast<GetElementPtrInst>(gep));
        if (gepOffset >= DINodeArr.size())
          return "";
        if (auto tmpDIType = dyn_cast<DIType>(DINodeArr[gepOffset]))
          dt = tmpDIType;
      }
    }
    std::string fieldID = DIUtils::computeFieldID(argDIType, dt);
    std::string argTypeName = DIUtils::getDITypeName(argDIType);
    // check if accessed field is included in the set of shared data
    std::set<std::string> accessedFields = sharedDataTypeMap[argTypeName];
    if (accessedFields.find(fieldID) == accessedFields.end())
      return "";
    std::string argIdx = std::to_string(arg->getArgNo());
    std::string resStr = "[modifeid data name]: Arg Idx " + argIdx + " | " + argTypeName + "->" + DIUtils::getDIFieldName(dt);
    return resStr;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const
  {
    AU.addRequired<sea_dsa::DsaAnalysis>();
    AU.addRequired<AccessInfoTracker>();
    AU.setPreservesAll();
  }

private:
  // sea_dsa::CompleteCallGraph *CCG;
  sea_dsa::DsaAnalysis *m_dsa;
  std::map<std::string, std::string> lockUnlockMap;
  std::set<Function *> kernelFunctions;
  std::set<Function *> driverFunctions;
  std::map<Value*, Value*> aliasMap;
  std::set<std::string> sharedDataTypes;
  std::map<std::string, std::set<std::string>> sharedDataTypeMap;
  std::set<Function*> crossDomainFuncs;
  std::ofstream atomicRegionWarn;
  AccessInfoTracker *ACT;
};
char SeadsaWarningGenerator::ID = 0;
static RegisterPass<SeadsaWarningGenerator> SeadsaWarningGenerator("sea-warn-gen", "Warning Generation using sea dsa", false, true);
} // namespace pdg

#endif