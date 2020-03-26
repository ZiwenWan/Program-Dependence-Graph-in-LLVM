#ifndef PTRSPLITIDLGEN_H_
#define PTRSPLITIDLGEN_H_
#include "llvm/IR/Module.h"
#include "llvm/PassAnalysisSupport.h"
#include "AccessInfoTracker.hpp"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"
#include <fstream>

namespace pdg
{
class PtrSplitIDLGen : public llvm::ModulePass
{
public:
  PtrSplitIDLGen() : llvm::ModulePass(ID) {}
  static char ID;
  bool runOnModule(llvm::Module &M);
  bool isOnlyUsedinProcessCall(ArgumentWrapper *argW);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
private:
  ProgramDependencyGraph *PDG;
  AccessInfoTracker *accInfoTracker;
  std::ofstream idl_file;
};
} // namespace

#endif