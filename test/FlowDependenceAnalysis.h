#ifndef FLOWDEPENDENCEANALYSIS_H
#define FLOWDEPENDENCEANALYSIS_H

#include "llvm/Analysis/AliasAnalysis.h"
#include "ProgramDependencies.h"
//#include "DataDependencies.h"


using namespace llvm;

class FlowDependenceAnalysis : public llvm::FunctionPass {
public:
  FlowDependenceAnalysis();
  ~FlowDependenceAnalysis();
  static char ID;

  virtual bool runOnFunction(llvm::Function &F);

  virtual void releaseMemory();

  virtual void getAnalysisUsage(AnalysisUsage &AU) const;

  std::vector<Instruction *> getDependencyInFunction(Function &F,
                                                     Instruction *pLoadInst);
};

#endif