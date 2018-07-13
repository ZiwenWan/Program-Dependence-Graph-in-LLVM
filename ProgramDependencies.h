#ifndef PROGRAMDEPENDENCIES_H
#define PROGRAMDEPENDENCIES_H

#define NULL 0
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

#include <vector>
#include <queue>
#include <list>
#include <iostream>
#include <string.h>
#include <time.h>

/*!
 * Program Dependencies Graph
 */
namespace pdg {
    typedef DependencyGraph<InstructionWrapper> ProgramDepGraph;

    class ProgramDependencyGraph : public llvm::ModulePass {
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

        void insertArgToTree(TypeWrapper *tyW, ArgumentWrapper *pArgW, TreeType treeTy, tree<InstructionWrapper*>::iterator insertLoc);

        int buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, int field_pos);

        void buildFormalTree(Argument *arg, TreeType treeTy, int field_pos);

        void buildFormalParameterTrees(Function *callee);

        void buildActualParameterTrees(CallInst *CI);

        void drawFormalParameterTree(Function *func, TreeType treeTy);

        void drawActualParameterTree(CallInst *CI, TreeType treeTy);

        void drawDependencyTree(Function *func);

        void linkTypeNodeWithGEPInst(std::list<ArgumentWrapper *>::iterator argI, tree<InstructionWrapper *>::iterator formal_in_TI);

        void connectFunctionAndFormalTrees(Function *callee);

        int connectCallerAndCallee(InstructionWrapper *CInstW, llvm::Function *callee);

        void printArgUseInfo(llvm::Module &M, std::set<std::string> funcNameList);

        void collectGlobalInstList();

        void categorizeInstInFunc(llvm::Function *func);

        bool processingCallInst(InstructionWrapper *instW);

        bool addNodeDependencies(InstructionWrapper *instW1, InstructionWrapper *instW2);

        bool runOnModule(llvm::Module &M);

        void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        llvm::StringRef getPassName() const { return "Program Dependency Graph"; }

        void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;

    private:
        llvm::Module *module;
        DataDependencyGraph *ddg;
        ControlDependencyGraph *cdg;
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