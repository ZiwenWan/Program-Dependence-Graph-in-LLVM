#ifndef CONTROLDEPENDENCYGRAPH_H_
#define CONTROLDEPENDENCYGRAPH_H_
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Pass.h"

#include "DependencyGraph.hpp"
#include "CallWrapper.hpp"
#include "FunctionWrapper.hpp"

namespace pdg {
class ControlDependencyGraph : public llvm::FunctionPass
{
  public:
    static char ID;
    ControlDependencyGraph() : llvm::FunctionPass(ID)
    {
      CDG = new DependencyGraph<InstructionWrapper>();
    }

    ~ControlDependencyGraph()
    {
      delete CDG;
    }

    bool runOnFunction(llvm::Function &F);
    void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
    llvm::StringRef getPassName() const; 
    void computeDependencies(llvm::Function &F);
    void addInstToBBDependency(InstructionWrapper* from, llvm::BasicBlock *to, DependencyType depType);
    void addBBToBBDependency(llvm::BasicBlock *from, llvm::BasicBlock *to, DependencyType depType);

  private:
    DependencyGraph<InstructionWrapper> *CDG;
    llvm::PostDominatorTree *PDT;
    llvm::Function *Func;
};
}

#endif