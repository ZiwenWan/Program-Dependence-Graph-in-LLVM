#include "CallWrapper.hpp"

using namespace llvm;

pdg::CallWrapper::CallWrapper(llvm::CallInst *CI)
{
  this->CI = CI;
  Function *calledFunc = CI->getCalledFunction();
  if (calledFunc == nullptr) {
    if (Function *f = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts()))
    {
      calledFunc = f;
    } else {
      errs() << "CallWrapper fail on initializing called functions, aborting..." << "\n";
      exit(0);
    }
  }

  for (Function::arg_iterator argIt = calledFunc->arg_begin();
       argIt != calledFunc->arg_end(); ++argIt)
  {
    Argument *arg = &*argIt;
    ArgumentWrapper *argW = new ArgumentWrapper(arg);
    argWList.push_back(argW);
  }

  retW = nullptr;
  if (calledFunc == nullptr)
  {
    errs() << "Called Function is null. Should use indirect call constructor...\n";
    exit(0);
  }

  const Twine t = ""; 
  Argument *ret = new Argument(calledFunc->getReturnType(), t, calledFunc, RETVALARGNO);
  retW = new ArgumentWrapper(ret);
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
  const Twine t = ""; 
  Argument *ret = new Argument(candidate_func->getReturnType(), t, candidate_func, RETVALARGNO); // create an argument wrapper for return value
  retW = new ArgumentWrapper(ret);
}
