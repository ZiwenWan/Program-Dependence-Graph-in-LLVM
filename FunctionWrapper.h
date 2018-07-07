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
        //this flag is true means this type contains a field(or grandfield) that is function pointer or FILE*
        // bool flag;

    public:
        TypeWrapper(llvm::Type *ty, int id) {
            this->ty = ty;
            this->id = id;
            // this->flag = false;
        }

        llvm::Type *getType() { return ty; }

        int getId() { return id; }
        // bool getUnseparatedFlag(){return flag;}
        //void setUnseparatedFlag(){this->flag = true;}

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
                    break;
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
            //  Function::ArgumentListType& callee_args = Func->getArgumentList();
            int arg_pos_in_call_site = 0;
            for (Function::arg_iterator argIt = CI->getCalledFunction()->arg_begin(),
                         argE = CI->getCalledFunction()->arg_end(); argIt != argE; ++argIt) {

                Argument *arg = &*argIt;
                ArgumentWrapper *argW = new ArgumentWrapper(arg);
                argWList.push_back(argW);
                arg_pos_in_call_site++;
            }

        }

        CallInst *getCallInstruction() {
            return CI;
        }

        std::list<ArgumentWrapper *> &getArgWList() {
            return argWList;
        }
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
        std::map<InstructionWrapper*, bool > visitedMap;
        std::set<llvm::Value *> ptrSet;

        bool treeFlag = false;
        bool visited = false;

    public:
        FunctionWrapper(Function *Func) {

            this->Func = Func;
            this->entryW = NULL;
            //  Function::ArgumentListType& callee_args = Func->getArgumentList();
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

        void setEntry(InstructionWrapper *entry) {
            //errs() << "Hello \n";
            this->entryW = entry;
        }

        InstructionWrapper *getEntry() { return entryW; }

        std::list<ArgumentWrapper *> &getArgWList() { return argWList; }

        std::list<llvm::StoreInst *> &getStoreInstList() { return storeInstList; }

        std::list<llvm::LoadInst *> &getLoadInstList() { return loadInstList; }

        std::list<llvm::Instruction *> &getReturnInstList() { return returnInstList; }

        std::list<llvm::CallInst *> &getCallInstList() { return callInstList; }

        std::set<llvm::Value *> &getPtrSet() { return ptrSet; }

        std::map<InstructionWrapper*, bool> &getVisitedMap() {return visitedMap;};

        bool hasFuncOrFilePrt();
    };
}

namespace pdg {
    //extern std::map<AllocaInst *, std::pair<StructType *, std::vector<Type *>>> alloca_struct_map;
    //extern std::map<std::string, std::vector<std::string>> struct_fields_map;
    //extern std::map<AllocaInst *, int> seen_structs;
    extern std::map<const Function *, FunctionWrapper *> funcMap;
    extern std::map<const CallInst *, CallWrapper *> callMap;
}

#endif // FUNCTIONWRAPPER_H
