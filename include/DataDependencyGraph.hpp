#ifndef DATADEPENDENCYGRAPH_H_
#define DATADEPENDENCYGRAPH_H_
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "DependencyGraph.hpp"
#include "CallWrapper.hpp"
#include "FunctionWrapper.hpp"
#include "sea_dsa/CompleteCallGraph.hh"
#include "sea_dsa/CallGraphUtils.hh"
#include "sea_dsa/DsaAnalysis.hh"
#include "sea_dsa/Global.hh"

namespace pdg
{
class DataDependencyGraph : public llvm::FunctionPass
{
public:
  static char ID;
  DataDependencyGraph() : FunctionPass(ID)
  {
    DDG = new DependencyGraph<InstructionWrapper>();
  }

  ~DataDependencyGraph()
  {
    delete DDG;
  }

  void initializeMemoryDependencyPasses();
  void constructFuncMapAndCreateFunctionEntry();
  void collectDefUseDependency(llvm::Instruction *inst);
  void collectCallInstDependency(llvm::Instruction *inst);
  void collectWriteToDependency(llvm::Instruction *loadInst);
  std::vector<llvm::Instruction *> getRAWDepList(llvm::Instruction *loadInst);
  void collectRAWDependency(llvm::Instruction *inst);
  void collectReadFromDependency(llvm::Instruction *inst);
  void collectNonLocalDependency(llvm::Instruction *inst);
  void collectDataDependencyInFunc();
  void collectAliasDependencies();
  bool isMustAlias(llvm::Value &V1, llvm::Value &V2, llvm::Function &F);
  bool isMayAlias(llvm::Value &V1, llvm::Value &V2, llvm::Function &F);
  DependencyNode<InstructionWrapper> *getNodeByData(llvm::Instruction *inst);
  typename DependencyNode<InstructionWrapper>::DependencyLinkList getNodeDepList(llvm::Instruction *inst);
  typename DependencyNode<InstructionWrapper>::DependencyLinkList getNodeDepList(InstructionWrapper *instW);
  virtual bool runOnFunction(llvm::Function &Func);
  virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
  virtual llvm::StringRef getPassName() const
  {
    return "Data Dependency Graph";
  }

private:
  DependencyGraph<InstructionWrapper> *DDG;
  llvm::Function *Func;
  llvm::AliasAnalysis* AA;
};
} // namespace pdg

#endif