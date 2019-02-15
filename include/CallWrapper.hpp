#ifndef CALLWRAPPER_H_
#define CALLWRAPPER_H_

#define RETVALARGNO 100
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "InstructionWrapper.hpp"
#include "ArgumentWrapper.hpp"

namespace pdg
{
class CallWrapper
{
public:
  CallWrapper() = delete;
  CallWrapper(llvm::CallInst *CI);
  CallWrapper(llvm::CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates);
  llvm::CallInst *getCallInst() { return CI; }
  std::vector<ArgumentWrapper *> &getArgWList() { return argWList; }
  ArgumentWrapper *getRetW() { return retW; }

private:
  llvm::CallInst *CI;
  std::vector<ArgumentWrapper *> argWList;
  ArgumentWrapper *retW; // wrapper for return type
};
} // namespace pdg

#endif