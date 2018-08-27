#ifndef FUNCTIONWRAPPER_H
#define FUNCTIONWRAPPER_H

#include "llvm/Pass.h"
#include "DependencyGraph.h"
#include <list>
#include <vector>

//using namespace std;
using namespace llvm;

namespace pdg {
    enum TreeType {
        ACTUAL_IN_TREE = 0,
        ACTUAL_OUT_TREE,
        FORMAL_IN_TREE,
        FORMAL_OUT_TREE
    };

    static int id = 0;

    class TypeWrapper {
    private:
        llvm::Type *ty;
        int id;

    public:
        TypeWrapper(llvm::Type *ty, int id) {
            this->ty = ty;
            this->id = id;
            // this->flag = false;
        }

        llvm::Type *getType() { return ty; }

        int getId() { return id; }
    };

    class ArgumentWrapper {
    private:
        Argument *arg;
        //formal in/out trees should be together with each function body
        tree<InstructionWrapper *> formalInTree;
        tree<InstructionWrapper *> formalOutTree;
        //actual in/out trees should be together with each call wrapper
        tree<InstructionWrapper *> actualInTree;
        tree<InstructionWrapper *> actualOutTree;
        std::set<InstructionWrapper *> GEPList;
    public:
        ArgumentWrapper(Argument *arg) {
            this->arg = arg;
            //    this->recursive_types = {};

            llvm::Instruction *allocInst = findAllocaInst(arg);
            if (allocInst != nullptr) {
                getDependentGEP(allocInst);
            }
        }

        Argument *getArg() {
            return arg;
        }

        //TreeType: 0-ACTUAL_IN 1-ACTUAL_OUT 2-FORMAL_IN 3-FORMAL_OUT
        tree<InstructionWrapper *> &getTree(TreeType treeTy) {
            switch (treeTy) {
                case FORMAL_IN_TREE:
                    return formalInTree;
                case FORMAL_OUT_TREE:
                    return formalOutTree;
                case ACTUAL_IN_TREE:
                    return actualInTree;
                case ACTUAL_OUT_TREE:
                    return actualOutTree;
                default:
                    return formalInTree;
            }
        }

        llvm::Instruction *findAllocaInst(Argument *arg);

        void getDependentGEP(Instruction *allocaInst);

        void copyTree(const tree<InstructionWrapper *> &srcTree, TreeType treeTy);

        std::set<InstructionWrapper *> &getGEPList() {
            return GEPList;
        }
    };

    class CallWrapper {
    private:
        CallInst *CI;
        std::list<ArgumentWrapper *> argWList;

    public:
        CallWrapper(CallInst *CI) {
          this->CI = CI;
          for (Function::arg_iterator argIt =
                   CI->getCalledFunction()->arg_begin();
               argIt != CI->getCalledFunction()->arg_end(); ++argIt) {

            Argument *arg = &*argIt;
            ArgumentWrapper *argW = new ArgumentWrapper(arg);
            argWList.push_back(argW);
          }
        }

        CallWrapper(CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates) {
          this->CI = CI;
          // constructor for indirect call instruction
          llvm::Function *candidate_func = indirect_call_candidates[0];
          for (Function::arg_iterator argIt = candidate_func->arg_begin();
               argIt != candidate_func->arg_end(); ++argIt) {
                Argument *arg = &*argIt;
                ArgumentWrapper *argW = new ArgumentWrapper(arg);
                argWList.push_back(argW);
            }
        }

        CallInst *getCallInstruction() { return CI; }

        std::list<ArgumentWrapper *> &getArgWList() { return argWList; }
    };

    // FunctionWrapper
    class FunctionWrapper {

    private:
      Function *Func;
      InstructionWrapper *entryW;
      std::list<llvm::StoreInst *> storeInstList;
      std::list<llvm::LoadInst *> loadInstList;
      std::list<llvm::Instruction *> returnInstList;
      std::list<llvm::CallInst *> callInstList;
      std::list<ArgumentWrapper *> argWList;
      std::set<llvm::Function *> dependent_funcs;
      std::set<llvm::Value *> ptrSet;

      bool treeFlag = false;
      bool visited = false;

    public:
      FunctionWrapper(Function *Func) {
        this->Func = Func;
        this->entryW = NULL;
        for (Function::arg_iterator argIt = Func->arg_begin(),
                                    argE = Func->arg_end();
             argIt != argE; ++argIt) {

          ArgumentWrapper *argW = new ArgumentWrapper(&*argIt);
          argWList.push_back(argW);
        }
      }

      bool hasTrees() { return treeFlag; }

      void setTreeFlag(bool flag) { this->treeFlag = flag; }

      bool isVisited() { return visited; }

      void setVisited(bool flag) { this->visited = flag; }

      Function *getFunction() { return Func; }

      void setEntry(InstructionWrapper *entry) { this->entryW = entry; }

      InstructionWrapper *getEntry() { return entryW; }

      std::list<ArgumentWrapper *> &getArgWList() { return argWList; }

      std::list<llvm::StoreInst *> &getStoreInstList() { return storeInstList; }

      std::list<llvm::LoadInst *> &getLoadInstList() { return loadInstList; }

      std::list<llvm::Instruction *> &getReturnInstList() {
        return returnInstList;
      }

      std::list<llvm::CallInst *> &getCallInstList() { return callInstList; }

      std::set<llvm::Value *> &getPtrSet() { return ptrSet; }

      bool hasFuncOrFilePrt();
    };
    } // namespace pdg

    namespace pdg {
    extern std::map<const Function *, FunctionWrapper *> funcMap;
    extern std::map<const CallInst *, CallWrapper *> callMap;
    } // namespace pdg

#endif // FUNCTIONWRAPPER_H
