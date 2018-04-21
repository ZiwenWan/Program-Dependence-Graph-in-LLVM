#ifndef SYSTEMCONTROLDEPENDENCEGRAPH_H_INCLUDED
#define SYSTEMCONTROLDEPENDENCEGRAPH_H_INCLUDED

#include "ControlDependencies.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Module.h"
#include <map>
#include <set>

typedef DependencyGraph<InstructionWrapper> SystemControlDepGraph;
typedef DependencyNode<InstructionWrapper> SCDGNode;
typedef std::vector<DependencyNode<InstructionWrapper> *>::iterator
    nodes_iterator;

class SystemControlDependenceGraph : public llvm::ModulePass {
private:
  const llvm::Function *mainFunction;
  SCDGNode *root;
  std::set<const llvm::Function *> libFunctions;

  void generateLibCallCDG(llvm::Function *func);
  void connectCDG(llvm::Module &M);
  void addCallEdge(const InstructionWrapper *from,
                   const InstructionWrapper *to);

public:
  static char ID;
  /// Zhiyuan: can reuse the data structure of CDG for the functions
  SystemControlDepGraph *SCDG;
  std::map<const llvm::Function *, InstructionWrapper *> entryNodes;

  SystemControlDependenceGraph() : ModulePass(ID), SCDG(nullptr) {}
  ~SystemControlDependenceGraph() {
    std::map<const llvm::Function *, InstructionWrapper *>::iterator itr =
        entryNodes.begin();
    while (itr != entryNodes.end()) {
      entryNodes.erase(itr++);
    }
    std::set<const llvm::Function *>::iterator str = libFunctions.begin();
    while (str != libFunctions.end()) {
      libFunctions.erase(str++);
    }
  }
  virtual bool runOnModule(llvm::Module &M);

  virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.addRequired<ControlDependencyGraph>();
    AU.addRequired<llvm::CallGraphWrapperPass>();
    AU.setPreservesAll();
  }

  SCDGNode *getRoot() { return root; }

  void setRoot(SCDGNode *node) { root = node; }

  bool isLibraryCall(const llvm::Function *func) {
    return (libFunctions.find(func) != libFunctions.end());
  }
};

namespace llvm
{

    template <> struct GraphTraits<SystemControlDependenceGraph *>
            : public GraphTraits<DepGraph*> {
        static NodeRef getEntryNode(SystemControlDependenceGraph *sg) {
            return sg->getRoot();
        }

        static nodes_iterator nodes_begin(SystemControlDependenceGraph *sg) {
            assert(sg->getRoot());
            return sg->SCDG->begin_children();
        }

        static nodes_iterator nodes_end(SystemControlDependenceGraph *sg) {
            assert(sg->getRoot());
            return sg->SCDG->end_children();
        }
    };

}//namespace llvm

#endif