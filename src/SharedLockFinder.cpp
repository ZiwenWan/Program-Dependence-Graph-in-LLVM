#ifndef LLVM_TEST_H
#define LLVM_TEST_H

#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "PDGUtils.hpp"
#include "DebugInfoUtils.hpp"
#include "DataDependencyGraph.hpp"
#include "AccessInfoTracker.hpp"

#include <vector>
#include <set>
#include <string.h>
#include <map>
#include <stack>
#include <fstream>
#include <sstream> 
namespace pdg {
    using namespace llvm;

    class SLFinder : public ModulePass {
    public:
        static char ID;

        SLFinder() : ModulePass(ID) {
        }

        bool runOnModule(Module &M)
        {
          CSwithCallInstSync = 0;
          CSwithStores = 0;
          module = &M;
          auto &pdgUtils = PDGUtils::getInstance();
          pdgUtils.constructFuncMap(M);
          // AIT = &getAnalysis<AccessInfoTracker>();

          // auto funcMap = pdgUtils.getFuncMap();
          // for (auto &F : M)
          // {
          //   pdgUtils.categorizeInstInFunc(F);
          // }

          std::ifstream importedFuncs("imported_func.txt");
          std::ifstream staticFuncs("static_func.txt");
          std::ifstream definedFuncs("defined_func.txt");
          std::set<std::string> interfaceFuncs;
          std::set<std::string> interfaceFuncsTrans;

          lockCallNameList = {
              "_raw_spin_lock",
              "_raw_spin_lock_irq",
              "mutex_lock",
              "rtnl_lock"};

          unlockCallNameList = {
              "_raw_spin_unlock",
              "_raw_spin_unlock_irq",
              "mutex_unlock",
              "rtnl_unlock"};


          for (std::string line; std::getline(importedFuncs, line);)
          {
            std::set<std::string> transitiveF = getTransitiveClosure(line);
            interfaceFuncsTrans.insert(transitiveF.begin(), transitiveF.end());
          }

          for (std::string line; std::getline(staticFuncs, line);)
          {
            std::set<std::string> transitiveF = getTransitiveClosure(line);
            interfaceFuncsTrans.insert(transitiveF.begin(), transitiveF.end());
          }

          for (std::string line; std::getline(definedFuncs, line);)
          {
            definedFuncBoundary.insert(line);
          }

          // std::set<std::string> TF = getTransitiveClosure("dummy_init_module");
          // interfaceFuncsTrans.insert(TF.begin(), TF.end());
          // interfaceFuncs.insert(TF.begin(), TF.end());
          // TF = getTransitiveClosure("ixgbe_init_module");
          // interfaceFuncsTrans.insert(TF.begin(), TF.end());
          // interfaceFuncs.insert(TF.begin(), TF.end());
          // TF = getTransitiveClosure("null_init");
          // interfaceFuncsTrans.insert(TF.begin(), TF.end());
          // interfaceFuncs.insert(TF.begin(), TF.end());
          // TF = getTransitiveClosure("nvme_init");
          // interfaceFuncsTrans.insert(TF.begin(), TF.end());
          // interfaceFuncs.insert(TF.begin(), TF.end());
          // TF = getTransitiveClosure("core_init");
          // interfaceFuncsTrans.insert(TF.begin(), TF.end());
          // interfaceFuncs.insert(TF.begin(), TF.end());

          // collects all lock instructions we can find in the given LLVM module
          for (auto &F : M)
          {
            // if (F.isDeclaration() || (interfaceFuncsTrans.find(F.getName()) == interfaceFuncsTrans.end()))
            if (F.isDeclaration())
              continue;
            auto callInstsList = pdgUtils.getFuncMap()[&F]->getCallInstList();
            for (auto ci : callInstsList)
            {
              Function *calledF = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts());
              if (calledF)
              {
                if (lockCallNameList.find(calledF->getName().str()) != lockCallNameList.end())
                {
                  lockCallSites.insert(ci);
                  // trackLock(ci);
                }
              }
            }
          }

          importedFuncs.clear();
          importedFuncs.seekg(0);
          for (std::string line; std::getline(importedFuncs, line);)
          {
            interfaceFuncs.insert(line);
          }

          staticFuncs.clear();
          staticFuncs.seekg(0);
          for (std::string line; std::getline(staticFuncs, line);)
          {
            interfaceFuncs.insert(line);
          }

          interfaceFuncs.insert("dummy_init_module");
          importedFuncs.close();
          staticFuncs.close();
          definedFuncs.close();

          errs() << "inter func size: " << interfaceFuncs.size() << "\n";
          errs() << "start finding shared lock..\n";
          findSharedLocks(interfaceFuncs);
          errs() << "Critical Section with reachable store instructions: " << CSwithStores << "\n";
          errs() << "Critical Section with syn call inst: " << CSwithCallInstSync << "\n";
          return false;
        }

        std::set<std::string> getTransitiveClosure(std::string startFuncName)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          std::queue<std::string> funcQ;
          std::set<std::string> ret;
          funcQ.push(startFuncName);
          unsigned depth = 4;
          while (!funcQ.empty() && depth > 0)
          {
            depth--;
            unsigned qSize = funcQ.size();
            while (qSize > 0)
            {
              qSize--;
              std::string funcName = funcQ.front();
              funcQ.pop();
              Function *F = module->getFunction(StringRef(funcName));
              if (!F || F->isDeclaration())
                continue;
              if (ret.find(funcName) != ret.end())
                continue;
              ret.insert(funcName);
              auto funcW = pdgUtils.getFuncMap()[F];
              auto callInstList = funcW->getCallInstList();
              for (auto callInst : callInstList)
              {
                Function *calledF = dyn_cast<Function>(callInst->getCalledValue()->stripPointerCasts());
                if (calledF)
                {
                  std::string calledFuncName = calledF->getName().str();
                  funcQ.push(calledFuncName);
                }
              }
            }
          }
          return ret;
        }

        Instruction *getSourceInst(Instruction* inst)
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

        Argument *getArgByAlloc(Instruction* argAlloc)
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

        int getGepOffset(GetElementPtrInst* gep)
        {
          int operand_num = gep->getNumOperands();
          llvm::Value *last_idx = gep->getOperand(operand_num - 1);
          if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx))
            return constInt->getSExtValue();
          return -1;
          // assert(false && "Get GEP offset fail.");
        }

        std::string getModifiedFieldName(Instruction* inst)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          if (!isa<StoreInst>(inst))
            return "";

          std::stack<Instruction*> gepStk;
          while (inst && !isa<AllocaInst>(inst))
          {
            if (isa<GetElementPtrInst>(inst))
              gepStk.push(inst); // store all gep instruction seen by far
            inst = getSourceInst(inst);
          }

          if (inst == nullptr)
            return "";

          Argument *arg = getArgByAlloc(inst);
          if (!arg)
            return "";
          
          DIType* dt = DIUtils::getArgDIType(*arg);

          dt = DIUtils::getLowestDIType(dt);

          if (!DIUtils::isStructTy(dt))
            return "";

          while (!gepStk.empty())
          {
            Instruction* gep = gepStk.top();
            gepStk.pop();
            int gepOffset = getGepOffset(dyn_cast<GetElementPtrInst>(gep));
            if (gepOffset < 0)
            {
              errs() << "find gep offsets smaller than 0! " << *gep << "\n";
              continue;
            }
            dt = DIUtils::getLowestDIType(dt);
            auto DINodeArr = dyn_cast<DICompositeType>(dt)->getElements();
            dt = dyn_cast<DIType>(DINodeArr[gepOffset]);
          }
          // retrive modifed arg name
          auto argAllocW = pdgUtils.getInstMap()[inst];
          auto &dbgInstList = pdgUtils.getFuncMap()[argAllocW->getFunction()]->getDbgDeclareInstList();
          std::string argName = DIUtils::getArgName(*arg, dbgInstList);
          std::string retStr = argName + "." + DIUtils::getDIFieldName(dt);
          return retStr;
        }

        Instruction *getLockGEPInst(Instruction* lockI)
        {
          for (auto U : lockI->users())
          {
            if (auto storeInst = dyn_cast<StoreInst>(U))
            {
              if (storeInst->getPointerOperand() == lockI)
              {
                auto lockField = dyn_cast<Instruction>(storeInst->getValueOperand());
                return lockField;
              }
            }
          }

          return nullptr;
        }

        std::pair<Type*, unsigned> getLocktypeAndOffset(Instruction* lockCallInst)
        {
          auto lockInstance = lockCallInst->getOperand(0);
          // a global lock instance
          if (isa<GlobalVariable>(lockInstance))
            return std::make_pair(lockInstance->getType(), 0);

          auto lockI = dyn_cast<Instruction>(lockInstance);
          while (lockI && !isa<AllocaInst>(lockI))
          {
            lockI = getSourceInst(lockI);
          }

          if (lockI == nullptr)
          {
            assert(false && "null alloca inst found for lock, abort...");
          }

          auto lockGEP = dyn_cast<GetElementPtrInst>(getLockGEPInst(lockI));
          auto lockOffset = getGepOffset(lockGEP);
          auto lockAlloca = dyn_cast<AllocaInst>(lockI);
          auto structTy = lockAlloca->getAllocatedType();
          return std::make_pair(structTy, lockOffset);
        }

        void trackLock(Instruction* lockCallInst)
        {
          auto p = getLocktypeAndOffset(lockCallInst);
          auto funcName = lockCallInst->getFunction()->getName().str();
          if (definedFuncBoundary.find(funcName) != definedFuncBoundary.end())
            driverLockOffsetMap[p.first].insert(p.second);
          else
            kernelLockOffsetMap[p.first].insert(p.second);
        }

        bool isPrivateLock(Instruction* lockCallInst)
        {
          auto p = getLocktypeAndOffset(lockCallInst);
          auto driverSideOffsets = driverLockOffsetMap[p.first];
          auto kernelSideOffsets = kernelLockOffsetMap[p.first];

          bool offsetFoundInBothSet = false;
          if (driverSideOffsets.find(p.second) != driverSideOffsets.end() && kernelSideOffsets.find(p.second) != kernelSideOffsets.end())
          {
            errs() << "not a private lock! " << "\n";
            return false;
          }

          return true;
        }

        void findSharedLocks(std::set<std::string> interfaceFuncs)
        {
          // 1. strat finding critical sections
          std::map<std::string, std::string> lockUnlockPair;
          lockUnlockPair.insert(std::make_pair("_raw_spin_lock", "_raw_spin_unlock"));
          lockUnlockPair.insert(std::make_pair("_raw_spin_lock_irq", "_raw_spin_unlock_irq"));
          lockUnlockPair.insert(std::make_pair("mutex_lock", "mutex_unlock"));
          lockUnlockPair.insert(std::make_pair("rtnl_lock", "rtnl_unlock"));

          // 2. runs CFG reachability test, start from the alloca, try to finds a lock_call
          std::ofstream sharedLockWarningFile;
          sharedLockWarningFile.open("LockWarning.txt");
          errs() << "Critical section number:" << lockCallSites.size() << "\n";
          auto sharedObjs = collectSharedObject(interfaceFuncs);
          errs() << "shared obj size: " << sharedObjs.size() << "\n";
          for (Instruction *lockCallInst : lockCallSites)
          {
            // if (isPrivateLock(lockCallInst))
            //   continue;

            std::string lockCallName = "";
            if (CallInst *lockCS = dyn_cast<CallInst>(lockCallInst))
            {
              Function *calledF = dyn_cast<Function>(lockCS->getCalledValue()->stripPointerCasts());
              if (lockCallNameList.find(calledF->getName()) != lockCallNameList.end())
                lockCallName = calledF->getName().str();
            }

            if (lockCallName.empty())
              continue;

            std::string unlockCallName = lockUnlockPair[lockCallName];
            std::vector<Instruction *> unlockCallInsts = findUnlockCalls(lockCallInst, unlockCallName); // finds reachable unlock instructions
            errs() << "finished findig unlocks" << "\n";
            errs() << "unlock Size: " << unlockCallInsts.size() << "\n";
            
            // get all CS
            std::set<Instruction *> seenStores; // record processed store instruction
            for (Instruction *unlockCallInst : unlockCallInsts)
            {
              errs() << "CS range: " << *lockCallInst << " -- " << *unlockCallInst << "\n";
              // collects all store instructions within a CS
              auto storesSet = collectStoreInstsInCS(lockCallInst, unlockCallInst);
              errs() << "Store inst num in " << lockCallInst->getFunction()->getName() << " : " << storesSet.size() << "\n";
              //for each store inst, test reachability on data flow graph. Test whether a store inst can read from a shared object
              auto &pdgUtils = PDGUtils::getInstance();
              unsigned reachableStores = 0;
              for (auto sharedObj : sharedObjs) // TODO: optimize for search space size of shared objects
              {
                std::set<Value *> processedFuncs;
                // process global variables users
                if (isa<GlobalVariable>(sharedObj))
                {
                  // errs() << "so: " << *sharedObj << "\n";
                  for (auto U : sharedObj->users())
                  {
                    if (auto ld = dyn_cast<LoadInst>(U))
                    {
                      InstructionWrapper *globalLoadInstW = pdgUtils.getInstMap()[ld];
                      InstructionWrapper *lockInstW = pdgUtils.getInstMap()[lockCallInst];
                      if (!callgraphCanReach(globalLoadInstW, lockInstW))
                        continue;

                      for (auto storeInst : storesSet)
                      {
                        if (storeInst == nullptr)
                          continue;

                        if (seenStores.find(storeInst) != seenStores.end())
                          continue;

                        if (isStoreToLock(storeInst))
                        {
                          // errs() << "Ignore, is a store for lock var - " << *storeInst << " - " << storeInst->getFunction()->getName() << "\n";
                          continue;
                        }

                        InstructionWrapper *storeInstW = pdgUtils.getInstMap()[storeInst];
                        // if (!callgraphCanReach(globalLoadInstW, storeInstW))
                        //   continue;

                        if (dfgCanReach(globalLoadInstW, storeInstW))
                        {
                          seenStores.insert(storeInst);
                          // check IDl and see if the synchronized fields cover the modified field
                          std::string modifiedFieldName = getModifiedFieldName(storeInst);
                          // get following calls that may follow the modification
                          // auto followingCallInsts = collectCallInstsBetweenInsts(storeInst, unlockCallInst);
                          // bool fieldIsCovered = false;
                          // for (auto inst : followingCallInsts)
                          // {
                          //   CallInst* CI = dyn_cast<CallInst>(inst);
                          //   Function* calledFunc = CI->getCalledFunction();
                          //   // TODO: handle indirect call soon
                          //   if (!calledFunc)
                          //     continue;
                          //   for (auto &arg : calledFunc->args())
                          //   {
                          //     auto accessedFields = AIT->computeArgIntraAccessFields(&arg, TreeType::FORMAL_IN_TREE);
                          //     if (accessedFields.find(modifiedFieldName) != accessedFields.end())
                          //     {
                          //       errs() << "Aha! " << modifiedFieldName << "\n";
                          //       fieldIsCovered = true;
                          //       break;
                          //     }
                          //   }
                          // }
                          // if (fieldIsCovered)
                          //   continue;
                          // print access information
                          std::string str;
                          llvm::raw_string_ostream rso(str);
                          sharedObj->print(rso);
                          sharedLockWarningFile << "[WARNING]: global var " << rso.str() << " modified in function: " << storeInst->getFunction()->getName().str() << "\n";
                          storeInst->print(rso);
                          sharedLockWarningFile << "\t Modify store instruction: " << rso.str() << "\n";
                          if (storeInst->getDebugLoc())
                            sharedLockWarningFile << "\t\t line number: " << storeInst->getDebugLoc()->getLine() << "\n";

                          sharedLockWarningFile << "\t modified field: " << getModifiedFieldName(storeInst) << "\n";
                          sharedLockWarningFile << "-------------------------------------------------------------------------------------" << "\n";
                        }
                      }
                    }
                  }
                }

                if (Argument *arg = dyn_cast<Argument>(sharedObj))
                {
                  Instruction *argAlloc = getArgAllocaInst(*arg);
                  if (!argAlloc)
                    continue;

                  InstructionWrapper *argAllocW = pdgUtils.getInstMap()[argAlloc];
                  InstructionWrapper *lockInstW = pdgUtils.getInstMap()[lockCallInst];

                  if (!callgraphCanReach(argAllocW, lockInstW))
                    continue;
                  // errs() << argAlloc->getFunction()->getName() << " -- " << *storeInst << "\n";
                  for (auto storeInst : storesSet)
                  {
                    if (storeInst == nullptr)
                      continue;
                    // if (processedFuncs.find(storeInst->getFunction()) != processedFuncs.end())
                    //   continue;
                    InstructionWrapper *storeInstW = pdgUtils.getInstMap()[storeInst];
                    if (!callgraphCanReach(argAllocW, storeInstW))
                      continue;

                    if (seenStores.find(storeInst) != seenStores.end())
                      continue;

                    if (argAllocW->getFunction() != storeInstW->getFunction() && definedFuncBoundary.find(storeInstW->getFunction()->getName()) != definedFuncBoundary.end())
                      continue;

                    if (isStoreToLock(storeInst))
                    {
                      errs() << "Ignore, is a store for lock var - " << *storeInst << " - " << storeInst->getFunction()->getName() << "\n";
                      continue;
                    }

                    if (dfgCanReach(argAllocW, storeInstW))
                    {
                      seenStores.insert(storeInst); // avoid process a same store instruction twice in future
                      reachableStores++;
                      auto &dbgInstList = pdgUtils.getFuncMap()[argAllocW->getFunction()]->getDbgDeclareInstList();
                      std::string argName = DIUtils::getArgName(*arg, dbgInstList);
                      sharedLockWarningFile << "[WARNING]: shared object \"" << argName << "\" is modified in function: " << storeInst->getFunction()->getName().str() << "\n";

                      if (argAllocW->getFunction() != storeInstW->getFunction())
                      {
                        sharedLockWarningFile << "\t Call Chain: ";
                        sharedLockWarningFile << argAllocW->getFunction()->getName().str() << " -> " << storeInstW->getFunction()->getName().str() << "\n";
                      }

                      std::string str;
                      llvm::raw_string_ostream rso(str);
                      storeInst->print(rso);
                      sharedLockWarningFile << "\t Modify store instruction: " << rso.str() << "\n";
                      sharedLockWarningFile << "\t Modify location: " << "\n";
                      if (storeInst->getFunction()->getSubprogram())
                        sharedLockWarningFile << "\t\t file: " << storeInst->getFunction()->getSubprogram()->getFile()->getRawFilename()->getString().str() << "\n";
                      sharedLockWarningFile << "\t\t function name: "  << storeInst->getFunction()->getName().str() << "\n";
                      if (storeInst->getDebugLoc())
                        sharedLockWarningFile << "\t\t line number: " << storeInst->getDebugLoc()->getLine() << "\n";
                      sharedLockWarningFile << "\t modified field: " << getModifiedFieldName(storeInst) << "\n\n";
                      rso.flush();
                      unlockCallInst->print(rso);
                      sharedLockWarningFile << "unlock inst: " << unlockCallInst->getFunction()->getName().str() << ": " << rso.str() << "\n";
                      sharedLockWarningFile << " ----------------------------------------------------------------------------------------------- \n";
                    }
                  }
                }
                // processedFuncs.insert(lockCallInst->getFunction());
              }
              errs() << "reachable stores for " << lockCallInst->getFunction()->getName() << " : " << reachableStores << "\n";
            }
          }
          sharedLockWarningFile.close();
        }

        // bool isSyncByCallInst(Argument *arg, Instruction *storeInst, Instruction *unlockCallInst)
        // {
        //   std::set<Instruction *> callInstsSet = collectCallInstsInCS(storeInst, unlockCallInst);
        //   auto instMap = PDGUtils::getInstance().getInstMap();
        //   if (callInstsSet.size() == 0)
        //     return false;
        //   for (auto callInst : callInstsSet)
        //   {
        //     auto argAllocW = instMap[getArgAllocaInst(*arg)];
        //     if (dfgCanReach(argAllocW, instMap[callInst]))
        //       return true;
        //   }
        //   return false;
        // }

        std::set<Value *> collectSharedObject(std::set<std::string> interfaceFuncs)
        {
          /*
          Collect pointer global object, all pointer type parameter in interface functions as shared objects.
          */
          std::set<Value *> sharedObjs;

          std::ifstream driverGlobalVars("driver_globalvars.txt");
          // driver defined functions, used to define boundary
          std::set<std::string> definedFuncs;
          std::ifstream definedFuncList("defined_func.txt");
          for (std::string line; std::getline(definedFuncList, line);)
          {
            definedFuncs.insert(line);
          }
          definedFuncList.close();

          for (std::string line; std::getline(driverGlobalVars, line);)
          {
            auto globalVar = module->getGlobalVariable(StringRef(line));
            if (!globalVar)
              continue;
            if (!globalVar->user_empty())
              sharedObjs.insert(globalVar);
            // for (auto U : globalVar->users())
            // {
            //   if (Instruction *i = dyn_cast<Instruction>(U)) // if a store instruciton stores to the global, then we know the global is modified. Thus, we need to synchornize status
            //   {
            //     // if (i->getPointerOperand() != globalVar)
            //     //   continue;
            //     Function *fUseGlobal = i->getFunction();
            //     // if (definedFuncs.find(fUseGlobal->getName()) == definedFuncs.end())
            //     // { // not in the driver defined func list. Means the global is used by the kernel
            //     errs() << "SO: " << *i << "\n";
            //     sharedObjs.insert(i);
            //     // }
            //   }
            // }
          }

          driverGlobalVars.close();

          // parameters passed across boundary
          for (std::string funcName : interfaceFuncs)
          {
            Function *func = module->getFunction(StringRef(funcName));
            if ((func == nullptr) || func->isDeclaration())
              continue;
            for (auto argI = func->arg_begin(); argI != func->arg_end(); ++argI)
            {
              Type *argTy = (*argI).getType();
              if (isa<PointerType>(argTy))
                sharedObjs.insert(&*argI);
            }
          }
          return sharedObjs;
        }

        std::vector<Instruction *> findUnlockCalls(Instruction *lockCallInst, std::string unlockCallName)
        {
          std::vector<Instruction *> unlockCallInsts;
          std::queue<BasicBlock *> entryBlockQ;
          std::set<BasicBlock *> seenBlock;
          BasicBlock *instBlock = lockCallInst->getParent();
          entryBlockQ.push(instBlock);
          seenBlock.insert(instBlock);

          while (!entryBlockQ.empty())
          {
            BasicBlock *entryB = entryBlockQ.front();
            entryBlockQ.pop();
            if (entryB->getParent()->getName().str().empty())
              continue;

            if (!entryB->getTerminator())
              continue;

            // put current basic block and its successors into a queue
            std::queue<BasicBlock *> bbQ;
            std::queue<BasicBlock *> tmpQ;
            std::set<BasicBlock*> seenBB;
            tmpQ.push(entryB);
            bbQ.push(entryB);
            while (!tmpQ.empty())
            {
              auto curBB = tmpQ.front();
              tmpQ.pop();
              if (seenBB.find(curBB) != seenBB.end())
                continue;
              seenBB.insert(curBB);
              auto TInst = curBB->getTerminator();
              if (!TInst)
                continue;
              for (unsigned I = 0; I < TInst->getNumSuccessors(); ++I)
              {
                BasicBlock *succ = TInst->getSuccessor(I);
                tmpQ.push(succ);
                bbQ.push(succ);
              }
            }

            while (!bbQ.empty())
            {
              BasicBlock *curBB = bbQ.front();
              bbQ.pop();

              for (Instruction &succI : *curBB)
              {
                if (&succI == lockCallInst)
                  continue;
                if (isa<CallInst>(&succI))
                {
                  CallSite lockCS = CallSite(&succI);
                  Function *calledFunc = lockCS.getCalledFunction();
                  if (!calledFunc)
                  {
                    if (Function *f = dyn_cast<Function>(lockCS.getCalledValue()->stripPointerCasts())) // handle case for bitcast
                      calledFunc = f;
                    if (!calledFunc)
                      continue;
                  }
                  BasicBlock *interBasicBlock = &(calledFunc->front());
                  if (seenBlock.find(interBasicBlock) == seenBlock.end())
                  {
                    entryBlockQ.push(interBasicBlock); // push the first basic block
                    seenBlock.insert(interBasicBlock);
                  }

                  if (calledFunc->getName().str() == unlockCallName)
                    unlockCallInsts.push_back(&succI);
                }
              }
            }
          }
          return unlockCallInsts;
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

        std::set<Instruction *> collectCallInstsBetweenInsts(Instruction *startInst, Instruction *endInst)
        {
          std::set<Instruction *> callInstsSet;
          auto instSet = collectInstsInCS(startInst, endInst);
          for (Instruction *i : instSet)
          {
            if (isa<CallInst>(i))
              callInstsSet.insert(i);
          }
          return callInstsSet;
        }

        std::set<Instruction *> collectStoreInstsInCS(Instruction *lockCallInst, Instruction *unlockCallInst)
        {
          std::set<Instruction *> storeInstsSet;
          auto instSet = collectInstsInCS(lockCallInst, unlockCallInst);
          for (Instruction *i : instSet)
          {
            if (auto st = dyn_cast<StoreInst>(i))
            {
              if (!isa<Argument>(st->getValueOperand()))
                storeInstsSet.insert(i);
            }
          }
          return storeInstsSet;
        }

        std::set<Instruction *> collectInstsInFunc(Function& func)
        {
          std::set<Instruction*> instSet;
          for (inst_iterator I = inst_begin(func), IE = inst_end(func); I != IE; ++I)
          {
            instSet.insert(&*I);
          }
          return instSet;
        }

        std::set<Instruction *> collectInstsInCS(Instruction *lockCallInst, Instruction *unlockCallInst)
        {
          std::set<Instruction *> instSet;
          BasicBlock *lockInstBlock = lockCallInst->getParent();
          if (lockInstBlock->getParent()->getName().str().empty())
            return instSet;
          std::queue<BasicBlock *> bbQ;
          std::queue<BasicBlock *> tmpQ;
          std::set<BasicBlock *> seenBB; // used to handle loop
          tmpQ.push(lockInstBlock);
          bbQ.push(lockInstBlock);
          while (!tmpQ.empty())
          {
            auto curBB = tmpQ.front();
            tmpQ.pop();
            if (seenBB.find(curBB) != seenBB.end())
              continue;
            seenBB.insert(curBB);
            auto TInst = curBB->getTerminator();
            if (!TInst)
              continue;
            for (unsigned I = 0; I < TInst->getNumSuccessors(); ++I)
            {
              BasicBlock *succ = TInst->getSuccessor(I);
              tmpQ.push(succ);
              bbQ.push(succ);
            }
          }

          while (!bbQ.empty())
          {
            BasicBlock *lockB = bbQ.front();
            bbQ.pop();
            for (Instruction &succI : *lockB)
            {
              if (lockB == lockInstBlock) // special handling for instructions in the same basic block as the lock instruction
              {
                if (isPredecessorInst(&succI, lockCallInst))
                  continue;
              }
              if (&succI == unlockCallInst)
                break; 

              instSet.insert(&succI);
            }
          }

          std::queue<Instruction*> callInstQ;
          for (auto inst : instSet)
          {
            if (isa<CallInst>(inst))
              callInstQ.push(inst);
          }

          while (!callInstQ.empty())
          {
            auto ii = callInstQ.front();
            callInstQ.pop();
            if (CallInst *CS = dyn_cast<CallInst>(ii))
            {
              Function *calledFunc = CS->getCalledFunction();
              if (!calledFunc)
              {
                if (Function *f = dyn_cast<Function>(CS->getCalledValue()->stripPointerCasts())) // handle case for bitcast
                  calledFunc = f;
              }
              // if (calledFunc && !calledFunc->isDeclaration() && !calledFunc->hasInternalLinkage())
              if (calledFunc && !calledFunc->isDeclaration())
              {
                auto interInstSet = collectInstsInFunc(*calledFunc); // collect inter func instructions
                instSet.insert(interInstSet.begin(), interInstSet.end());
                for (auto inst : interInstSet)
                {
                  if (isa<CallInst>(inst))
                    callInstQ.push(inst);
                }
              }
            }
          }

          return instSet;
          assert(false && "cannot reach unlock inst. Collecting Stores in CS fails...");
        }

        bool
        callgraphCanReach(InstructionWrapper *sourceW, InstructionWrapper *targetW)
        {
          if (sourceW->getFunction() == targetW->getFunction())
            return true;
          auto &pdgUtils = PDGUtils::getInstance();
          auto funcMap = pdgUtils.getFuncMap();
          std::queue<Function *> funcQ;
          std::set<Function *> seenFunc;
          funcQ.push(sourceW->getFunction());
          while (!funcQ.empty())
          {
            Function *curFunc = funcQ.front();
            funcQ.pop();
            if (funcMap.find(curFunc) == funcMap.end())
              continue;
            if (seenFunc.find(curFunc) != seenFunc.end())
              continue;
            auto callInstsList = funcMap[curFunc]->getCallInstList();
            for (auto callInst : callInstsList)
            {
              Function *calledFunc = callInst->getCalledFunction();
              if (!calledFunc)
                continue;
              if (calledFunc == targetW->getFunction())
                return true;
              funcQ.push(calledFunc);
              seenFunc.insert(calledFunc);
            }
          }
          return false;
        }

        std::set<InstructionWrapper *> collectsRelevantPtrs(InstructionWrapper *sourceInstW)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          auto instMap = pdgUtils.getInstMap();
          std::queue<InstructionWrapper *> instQ;
          std::set<InstructionWrapper *> retQ;
          std::set<InstructionWrapper *> seenInstW;
          instQ.push(sourceInstW);
          retQ.insert(sourceInstW);
          while (!instQ.empty())
          {
            auto curInstW = instQ.front();
            instQ.pop();
            //check for seen instW
            if (seenInstW.find(curInstW) != seenInstW.end())
              continue;
            seenInstW.insert(curInstW);

            auto dataDList = ddg->getNodeDepList(curInstW);
            for (auto depPair : dataDList)
            {
              auto depNode = depPair.first;
              auto depType = depPair.second;
              InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depNode->getData());
              if (depType == DependencyType::DATA_ALIAS)
              {
                retQ.insert(depInstW);
                instQ.push(depInstW);
              }

              if (depType == DependencyType::DATA_DEF_USE)
              {
                if (isa<GetElementPtrInst>(depInstW->getInstruction()))
                {
                  retQ.insert(depInstW);
                  instQ.push(depInstW);
                }
              }
            }
          }

          return retQ;
        }

        bool isStoreToLock(Instruction *storeInst)
        {
          std::string str;
          llvm::raw_string_ostream rso(str);
          storeInst->print(rso);
          if (rso.str().find("spinlock") != std::string::npos)
          {
            if (StoreInst *si = dyn_cast<StoreInst>(storeInst))
            {
              if (isa<AllocaInst>(si->getPointerOperand()))
                return true;
            }
          }
          // auto &pdgUtils = PDGUtils::getInstance();
          // auto instMap = pdgUtils.getInstMap();
          // auto *storeInstW = instMap[storeInst];
          // auto *ptrOp = dyn_cast<StoreInst>(storeInst)->getPointerOperand();

          // if (Instruction *pInst = dyn_cast<Instruction>(ptrOp))
          // {
          //   InstructionWrapper *sourceOpW = instMap[pInst];
          //   std::queue<InstructionWrapper *> instQ;
          //   std::set<InstructionWrapper *> seenInsts;
          //   instQ.push(sourceOpW);
          //   while (!instQ.empty())
          //   {
          //     auto *instW = instQ.front();
          //     instQ.pop();
          //     if (seenInsts.find(instW) != seenInsts.end())
          //       continue;
          //     seenInsts.insert(instW);

          //     auto dataDepList = ddg->getNodeDepList(instW);
          //     for (auto depPair : dataDepList)
          //     {
          //       auto depNode = depPair.first;
          //       if ((depNode == nullptr) || depNode->getData() == nullptr)
          //         continue;
          //       InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depNode->getData());
          //       instQ.push(depInstW);

          //       if (auto *callInst = dyn_cast<CallInst>(depInstW->getInstruction()))
          //       {
          //         if (!callInst->getCalledFunction())
          //           continue;
          //         if (unlockCallNameList.find(callInst->getCalledFunction()->getName().str()) != unlockCallNameList.end())
          //           return true;
          //       }
          //     }
          //   }
          // }
          return false;
        }

        bool dfgCanReach(InstructionWrapper *source, InstructionWrapper *target)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          std::queue<InstructionWrapper *> instWQ;
          std::set<InstructionWrapper *> seenInstWs;
          instWQ.push(source);
          seenInstWs.insert(source);
          while (!instWQ.empty())
          {
            auto curInstW = instWQ.front();
            instWQ.pop();
            Function *curFunc = curInstW->getFunction();
            if (curFunc->getName().empty())
              continue;

            ddg = &getAnalysis<DataDependencyGraph>(*curFunc);
            std::set<InstructionWrapper *> relevantPtrs = collectsRelevantPtrs(curInstW); // collects insts that have same address or dervied address
            for (auto relevantInstW : relevantPtrs)
            {
              if (!relevantInstW)
                continue;
              GraphNodeType depNodeTy = relevantInstW->getGraphNodeType();
              if (depNodeTy != GraphNodeType::INST && depNodeTy != GraphNodeType::CALL)
                continue;

              if (relevantInstW == target)
                return true;
              // errs() << relevantInstW->getFunction()->getName() << *relevantInstW->getInstruction() << "\n";
              auto dataDepList = ddg->getNodeDepList(relevantInstW);
              for (auto depPair : dataDepList)
              {
                auto depNode = depPair.first;
                // auto depNodeTy = depPair.second;
                if (depNode->getData() == nullptr)
                  continue;
                InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depNode->getData());
                auto depNodeTy = depInstW->getGraphNodeType();
                if (depNodeTy != GraphNodeType::INST && depNodeTy != GraphNodeType::CALL)
                  continue;

                if (depInstW == target)
                {
                  if (auto st = dyn_cast<StoreInst>(target->getInstruction()))
                  {
                    auto destOp = dyn_cast<Instruction>(st->getPointerOperand());
                    auto stW = pdgUtils.getInstMap()[destOp];
                    if (relevantPtrs.find(stW) != relevantPtrs.end())
                    {
                      errs() << "WARNING: reachable store instruction: " << *target->getInstruction() << "\n";
                      return true;
                    }
                  }
                }

                if (depNodeTy == GraphNodeType::CALL) // handle func calls
                {
                  auto retW = getInterprocAlloc(relevantInstW, depInstW); // get the arg alloc for the callee
                  // Instruction* depInst = depInstW->getInstruction();
                  // if (!depInst)
                  //   continue;
                  // // collects the call chain
                  // if (CallInst *CI = dyn_cast<CallInst>(depInst))
                  // {
                  //   Function *calledF = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts());
                  //   if (calledF)
                  //     callChain.push_back(calledF->getName());
                  // }

                  if (retW != nullptr && seenInstWs.find(retW) == seenInstWs.end())
                  {
                    instWQ.push(retW);
                    seenInstWs.insert(retW);
                  }
                }
              }
            }
          }
          return false;
        }

        Instruction *getArgAllocaInst(Argument &arg)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          auto funcMap = pdgUtils.getFuncMap();
          auto f = arg.getParent();
          if (funcMap.find(f) == funcMap.end())
            return nullptr;
          for (auto dbgInst : funcMap[f]->getDbgDeclareInstList())
          {
            if (auto DLV = dyn_cast<DILocalVariable>(dbgInst->getVariable()))
            {
              if (DLV->isParameter() && DLV->getScope()->getSubprogram() == arg.getParent()->getSubprogram())
              {
                Instruction *allocaInst = dyn_cast<Instruction>(dbgInst->getVariableLocation());
                return allocaInst;
              }
            }
          }
          // errs() << arg.getParent()->getName() << " " << arg.getArgNo() << "\n";
          return nullptr;
          assert(false && "no viable alloca inst for argument.");
        }

        InstructionWrapper *getInterprocAlloc(InstructionWrapper *curInstW, InstructionWrapper *callInstW)
        {
          auto &pdgUtils = PDGUtils::getInstance();
          auto instMap = pdgUtils.getInstMap();
          auto funcMap = pdgUtils.getFuncMap();

          InstructionWrapper *retW;
          CallInst *CI = dyn_cast<CallInst>(callInstW->getInstruction());
          // must have args
          if (CI->getNumArgOperands() == 0)
            return nullptr;

          Function *caller = curInstW->getFunction();
          Function *callee = CI->getCalledFunction();

          if (caller == callee)
            return nullptr;
          // TODO: handle indirect calls
          if (!caller || !callee || callee->isIntrinsic() || callee->isVarArg())
            return nullptr;

          // errs() << *curInstW->getInstruction() << "\n";
          // errs() << "call arg: " << callee->getName() << "\n";
          int argPos = 0;
          auto calleeFuncW = funcMap[callee];
          for (auto argI = CI->arg_begin(); argI != CI->arg_end(); ++argI)
          {
            if (*argI == curInstW->getInstruction())
            {
              if (!calleeFuncW)
                continue;
              auto calleeArgW = calleeFuncW->getArgWByIdx(argPos);
              if (!calleeArgW)
                continue;
              auto calleeSenArg = calleeFuncW->getArgWByIdx(argPos)->getArg();
              auto calleeAllocOfSenArg = getArgAllocaInst(*calleeSenArg); // get correspond alloc in the       callee and mark it sensitive
              if (!calleeAllocOfSenArg)
              {
                errs() << "[WARNING]: Cannot find arg alloc for " << caller->getName() << " - " << callee->getName() << " - " << argPos << "\n";
                continue;
              }
              retW = instMap[calleeAllocOfSenArg];
              break;
            }
            argPos++;
          }

          assert(retW != nullptr && "propagate to new callee fail. Null alloc is found");
          return retW;
        }

        // Analysis Usage, specify PDG at this time
        void getAnalysisUsage(AnalysisUsage &AU) const
        {
          AU.addRequired<DataDependencyGraph>();
          // AU.addRequired<AccessInfoTracker>();
          AU.setPreservesAll();
        }

      private:
        Module *module;
        std::set<llvm::Instruction *> lockCallSites;
        std::set<std::string> lockCallNameList;
        std::set<std::string> unlockCallNameList;
        std::set<std::string> definedFuncBoundary;
        std::map<llvm::Type*, std::set<unsigned>> kernelLockOffsetMap;
        std::map<llvm::Type*, std::set<unsigned>> driverLockOffsetMap;
        DataDependencyGraph *ddg;
        std::set<llvm::Function *> funcDDGSet;
        unsigned CSwithStores;
        unsigned CSwithCallInstSync;
        AccessInfoTracker *AIT;
    };

    char SLFinder::ID = 0;
    static RegisterPass<SLFinder> SLFinder("lock-find", "Shared Lock Finder", false, true);
    } // namespace pdg
#endif
