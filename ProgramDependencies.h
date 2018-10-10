#ifndef PROGRAMDEPENDENCIES_H
#define PROGRAMDEPENDENCIES_H

#define SENSITIVE 256
#define PDG_CONSTRUCTION 0

#include "AllPasses.h"
#include "llvm/Pass.h"
#include "ControlDependencies.h"
#include "DataDependencies.h"

#include "llvm/IR/Intrinsics.h" 
#include "llvm/IR/LLVMContext.h" 
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Analysis/CallGraph.h"

#include <vector>
#include <queue>
#include <list>
#include <iostream>
#include <string.h>
#include <time.h>
#include <chrono>

/*!
 * Program Dependencies Graph
 */
namespace pdg {
    enum ArgumentContainType {
        NOTCONTAINED = 0,
        CONTAINED,
        EQUAL
    };

    typedef DependencyGraph<InstructionWrapper> ProgramDepGraph;

    class ProgramDependencyGraph : public llvm::ModulePass {
    typedef std::map<llvm::Function *, std::map<std::string, bool>> ArgUseInfoMap;
    public:
        static char ID; // Pass ID, replacement for typeid
        ProgramDepGraph *PDG;
        static llvm::AliasAnalysis *Global_AA;

        ProgramDependencyGraph() : llvm::ModulePass(ID) {
            PDG = new ProgramDepGraph();
        }

        ~ProgramDependencyGraph() {
            releaseMemory();
            delete PDG;
        }

        tree<InstructionWrapper*>::iterator getInstInsertLoc(ArgumentWrapper *argW, TypeWrapper *tyW, TreeType treeType);

        bool isBasicTypeOrPtr(Type* ty);

        int getCallParamIdx(InstructionWrapper* instW, InstructionWrapper* callInstW);

        ArgumentWrapper* getArgWByIdx(pdg::FunctionWrapper* funcW, int argWIdx);

        void insertArgToTree(TypeWrapper *tyW, ArgumentWrapper *pArgW, TreeType treeTy, tree<InstructionWrapper*>::iterator insertLoc);

        int buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, int field_pos );

        void buildFormalTree(Argument *arg, TreeType treeTy, int field_pos);

        void buildFormalParameterTrees(Function *callee);

        void buildActualParameterTrees(CallInst *CI);

        void buildActualParameterTreesForIndirectCall(CallInst *CI, llvm::Function *candidate_func); 

        void drawFormalParameterTree(Function *func, TreeType treeTy);

        void drawActualParameterTree(CallInst *CI, TreeType treeTy);

        void drawDependencyTree(Function *func);

        std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>> getParameterTreeNodeWithCorrespondGEP(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI);

        //std::vector<InstructionWrapper*> getInitialStoreInsts(llvm::Argument *arg);

        int getArgOpType(llvm::Argument *arg);

        int getGEPOpType(InstructionWrapper *GEPInstW);

        void linkTypeNodeWithGEPInst(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI);

        void connectFunctionAndFormalTrees(Function *callee);

        int connectCallerAndCallee(InstructionWrapper *CInstW, llvm::Function *callee);

        // functions for getting R/W info for all typenodes

        llvm::Instruction* getArgAllocaInst(llvm::Argument *arg);

        std::vector<llvm::Instruction*> getArgStoreInsts(llvm::Argument *arg);

        void mergeTypeTreeReadAndWriteInfo(ArgumentWrapper* argW, tree<InstructionWrapper*>::iterator mergeTo, tree<InstructionWrapper*>::iterator mergeFrom);

        std::set<InstructionWrapper*> getAliasPtrForArgInFunc(ArgumentWrapper* argW);

        int getArgType(llvm::Argument *arg);

        int getAccessTypeForInstW(InstructionWrapper* instW);

        int getAccessTypeForGEPInstW(InstructionWrapper* instW);

        void collectRelevantCallInstsForArg(ArgumentWrapper* argW, InstructionWrapper* instW);

        void getReadWriteInfoSingleValPtr(ArgumentWrapper *argW);

        void getReadWriteInfoAggregatePtr(ArgumentWrapper *argW);

        void getIntraFuncReadWriteInfo(llvm::Function* F);

        void getInterFuncReadWriteInfo(llvm::Function* F);

        void mergeArgWReadWriteInfo(ArgumentWrapper* callerArgW, ArgumentWrapper* calleeArgW);

        unsigned getStructElementNum(llvm::Module &M, InstructionWrapper *curTyNode);

        const StructLayout* getStructLayout(llvm::Module &M, InstructionWrapper *curTyNode);

        std::set<pdg::InstructionWrapper *> getAllRelevantGEP(llvm::Argument *arg);

        void collectGlobalInstList();

        void categorizeInstInFunc(llvm::Function *func);

        bool processingCallInst(InstructionWrapper *instW);

        std::vector<llvm::Function *> collectIndirectCallCandidates(FunctionType *funcType);

        //void connectAllPossibleFunctions(InstructionWrapper *CInstW, FunctionType *funcTy);
        void connectAllPossibleFunctions(CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates);

        bool addNodeDependencies(InstructionWrapper *instW1);

        int getArgMatchType(llvm::Argument *arg1, llvm::Argument *arg2);

        bool isFuncTypeMatch(FunctionType *funcTy, FunctionType *indirectFuncCallTy);

        void printParameterTreeForFunc(llvm::Module &M, std::set<std::string> funcList);

        void printArgUseInfo(llvm::Module &M, std::set<std::string> funcNameList);

        //void printArgumentDependentInsts(llvm::Argument *arg);
        //void printSensitiveFunctions();
        bool runOnModule(llvm::Module &M);

        void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        llvm::StringRef getPassName() const { return "Program Dependency Graph"; }

        void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;

    private:
        llvm::Module *module;
        DataDependencyGraph *ddg;
        ControlDependencyGraph *cdg;
        std::map<llvm::Function *, std::map<std::string, bool>> global_arg_use_info_map;
        //std::vector<llvm::Value *> sensitive_values;
    };
}

namespace llvm
{
    template <> struct GraphTraits<pdg::ProgramDependencyGraph *>
            : public GraphTraits<pdg::DepGraph*> {
        static NodeRef getEntryNode(pdg::ProgramDependencyGraph *PG) {
            return *(PG->PDG->begin_children());
        }

        static nodes_iterator nodes_begin(pdg::ProgramDependencyGraph *PG) {
            return PG->PDG->begin_children();
        }

        static nodes_iterator nodes_end(pdg::ProgramDependencyGraph *PG) {
            return PG->PDG->end_children();
        }
    };
}


#endif