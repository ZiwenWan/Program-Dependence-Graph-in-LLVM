#include "CallWrapper.hpp"

using namespace llvm;

pdg::CallWrapper::CallWrapper(llvm::CallInst *CI)
{
  this->CI = CI;
  for (Function::arg_iterator argIt = CI->getCalledFunction()->arg_begin();
       argIt != CI->getCalledFunction()->arg_end(); ++argIt)
  {
    Argument *arg = &*argIt;
    ArgumentWrapper *argW = new ArgumentWrapper(arg);
    argWList.push_back(argW);
  }
}

pdg::CallWrapper::CallWrapper(CallInst *CI, std::vector<Function *> indirect_call_candidates)
{
  this->CI = CI;
  // constructor for indirect call instruction
  Function *candidate_func = indirect_call_candidates[0];
  for (Function::arg_iterator argIt = candidate_func->arg_begin();
       argIt != candidate_func->arg_end(); ++argIt)
  {
    Argument *arg = &*argIt;
    ArgumentWrapper *argW = new ArgumentWrapper(arg);
    argWList.push_back(argW);
  }
}
