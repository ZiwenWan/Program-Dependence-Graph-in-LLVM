#ifndef DATADEPENDENCIES_H
#define DATADEPENDENCIES_H

//#include "AllPasses.h"
//#include "DependencyGraph.h"
//#include "FunctionWrapper.h"
#include "PDGHelper.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Pass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Support/raw_ostream.h"

namespace pdg {
    typedef DependencyGraph<InstructionWrapper> DataDepGraph;

    static std::vector<InstructionWrapper *> gp_list;

    class DataDependencyGraph : public llvm::FunctionPass {
    public:
        static char ID; // Pass ID, replacement for typeid
        DataDepGraph *DDG;

        DataDependencyGraph() : FunctionPass(ID) { DDG = new DataDepGraph(); }

        ~DataDependencyGraph() {
            releaseMemory();
            delete DDG;
        }

        void initializeMemoryDependencyPasses();

        void constructFuncMapAndCreateFunctionEntry();

        void collectDefUseDependency(llvm::Instruction *inst);

        void collectCallInstDependency(llvm::Instruction *inst);

        void collectWriteToDependency(llvm::Instruction *pLoadInst);

        std::vector<llvm::Instruction *> getRAWDepList(llvm::Instruction *pLoadInst);

        void collectRAWDependency(llvm::Instruction *inst);

        void collectReadFromDependency(llvm::Instruction *inst);

        void collectNonLocalDependency(llvm::Instruction *inst);

        void collectDataDependencyInFunc();

        void collectAliasInst();

        virtual bool runOnFunction(llvm::Function &F);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        virtual llvm::StringRef getPassName() const {
            return "Data Dependency Graph";
        }

        virtual void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;

    private:
        llvm::Function *func;
        AliasAnalysis *AA;
        CFLSteensAAResult *steenAA;
        MemoryDependenceResults *MD;
    };
}

namespace llvm {
    template <>
    struct GraphTraits<pdg::DataDependencyGraph *> : public GraphTraits<pdg::DepGraph *> {
        static NodeRef getEntryNode(pdg::DataDependencyGraph *DG) {
            return *(DG->DDG->begin_children());
        }

        static nodes_iterator nodes_begin(pdg::DataDependencyGraph *DG) {
            return DG->DDG->begin_children();
        }

        static nodes_iterator nodes_end(pdg::DataDependencyGraph *DG) {
            return DG->DDG->end_children();
        }
    };

} // namespace llvm

#endif