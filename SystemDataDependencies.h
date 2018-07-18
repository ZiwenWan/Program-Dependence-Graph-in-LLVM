#ifndef SYSTEMDATADEPENDENCIES_H
#define SYSTEMDATADEPENDENCIES_H
#include "DataDependencies.h"

using namespace llvm;

namespace pdg {
    typedef DependencyGraph<InstructionWrapper> DataDepGraph;

/*!
 * Data Dependency Graph
 */
    class SystemDataDependencyGraph : public llvm::ModulePass {
    public:
        static char ID; // Pass ID, replacement for typeid
        DataDepGraph *SystemDG;
        // static std::set<BasicBlockWrapper*> ExtraNodes;

        SystemDataDependencyGraph() : ModulePass(ID) {
            SystemDG = NULL;
            //      DDG = new DataDepGraph();
            //      errs()<< "SystemDataDependencyGraph()" << '\n';
        }

        ~SystemDataDependencyGraph() {
            //      BasicBlockWrapper::releaseMemory();
            //    delete DDG;
        }

        virtual bool runOnModule(llvm::Module &M);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        virtual llvm::StringRef getPassName() const {
            return "SystemData Dependency Graph";
        }

        virtual void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;
    };
}


namespace llvm
{
    template <> struct GraphTraits<pdg::SystemDataDependencyGraph *>
            : public GraphTraits<pdg::DepGraph*> {
        static NodeRef getEntryNode(pdg::SystemDataDependencyGraph *DG) {
            return *(DG->SystemDG->begin_children());
        }

        static nodes_iterator nodes_begin(pdg::SystemDataDependencyGraph *DG) {
            return DG->SystemDG->begin_children();
        }

        static nodes_iterator nodes_end(pdg::SystemDataDependencyGraph *DG) {
            return DG->SystemDG->end_children();
        }
    };

}

#endif