#ifndef PTRSPLITIDLGEN_H_
#define PTRSPLITIDLGEN_H_
#include "llvm/IR/Module.h"
#include "llvm/PassAnalysisSupport.h"
#include "AccessInfoTracker.hpp"
#include <fstream>

namespace pdg
{
class PtrSplitIDLGen : public llvm::ModulePass
{
public:
  PtrSplitIDLGen() : llvm::ModulePass(ID) {}
  static char ID;
  bool runOnModule(llvm::Module &M);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
private:
  AccessInfoTracker *acctracker;
  std::ofstream idl_file;
};
} // namespace

#endif