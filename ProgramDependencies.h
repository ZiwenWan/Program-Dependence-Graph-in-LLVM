#ifndef PROGRAMDEPENDENCIES_H
#define PROGRAMDEPENDENCIES_H

#define NULL 0
#define SENSITIVE 256
#define PDG_CONSTRUCTION 0

#include "AllPasses.h"
#include "llvm/Pass.h"
#include "ControlDependencies.h"
#include "DataDependencies.h"
#include "ConnectFunctions.h"


#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"

#include <map>
#include <set>
#include <vector>
#include <deque>
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

        void drawFormalParameterTree(Function *func, TreeType treeTy);

        void drawActualParameterTree(CallInst *CI, TreeType treeTy);

        //    void drawParameterTree(llvm::Function* call_func, TreeType treeTy);

//  void connectAllPossibleFunctions(InstructionWrapper *CInstW,
//                                   FunctionType *funcTy);

        void connectFunctionAndFormalTrees(Function *callee);

        int connectCallerAndCallee(InstructionWrapper *CInstW,
                                   llvm::Function *callee);
        //    int connectCallerAndCallee(CallInst *CI, llvm::Function *callee);

        bool runOnModule(llvm::Module &M);

        void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        llvm::StringRef getPassName() const { return "Program Dependency Graph"; }

        void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;
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