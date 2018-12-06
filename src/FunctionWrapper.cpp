#include "FunctionWrapper.hpp"

using namespace llvm;

pdg::FunctionWrapper::FunctionWrapper(Function *Func)
{
  this->Func = Func;
  this->entryW = nullptr;
  for (Function::arg_iterator argIt = Func->arg_begin(), argE = Func->arg_end();
       argIt != argE; 
       ++argIt)
  {
    ArgumentWrapper *argW = new ArgumentWrapper(&*argIt);
    argWList.push_back(argW);
  }
}

bool pdg::FunctionWrapper::hasTrees()
{
  return treeFlag;
}

bool pdg::FunctionWrapper::isVisited()
{
  return visited;
}

pdg::ArgumentWrapper *pdg::FunctionWrapper::getArgWByArg(Argument &arg)
{
  for (auto argW : argWList) 
  {
    if (argW->getArg() == &arg) {
      return argW;
    }
  }

  errs() << "ERROR: cannot find argW by arg in Function " << Func->getName() << "\n";
  return nullptr;
}

pdg::ArgumentWrapper *pdg::FunctionWrapper::getArgWByIdx(int idx) {
  if (idx > argWList.size())
  {
    errs() << "request index excess argW list length... Return nullptr" << "\n";
    return nullptr;
  }
  return argWList[idx];
}