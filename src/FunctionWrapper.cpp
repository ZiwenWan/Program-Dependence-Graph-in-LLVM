#include "FunctionWrapper.hpp"
#include "llvm/ADT/Twine.h"

using namespace llvm;

pdg::FunctionWrapper::FunctionWrapper(Function *Func)
{
  this->Func = Func;
  this->entryW = nullptr;
  this->treeFlag = false;
  this->visited = false;
  for (Function::arg_iterator argIt = Func->arg_begin(), argE = Func->arg_end();
       argIt != argE;
       ++argIt)
  {
    ArgumentWrapper *argW = new ArgumentWrapper(&*argIt);
    argWList.push_back(argW);
  }

  const Twine t = "";
  Argument *ret = new Argument(Func->getReturnType(), t, Func, 100);
  this->retW = new ArgumentWrapper(ret);
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
  if (arg.getArgNo() == 100)
    return retW;

  for (auto argW : argWList)
  {
    if (argW->getArg() == &arg)
    {
      return argW;
    }
  }

  errs() << "ERROR: cannot find argW by arg in Function " << Func->getName() << "\n";
  return nullptr;
}

void pdg::FunctionWrapper::addStoreInst(Instruction *inst)
{
  storeInstList.push_back(dyn_cast<StoreInst>(inst));
}

void pdg::FunctionWrapper::addLoadInst(Instruction *inst)
{
    loadInstList.push_back(dyn_cast<LoadInst>(inst));
}

void pdg::FunctionWrapper::addCallInst(Instruction *inst)
{
  callInstList.push_back(dyn_cast<CallInst>(inst));
}

void pdg::FunctionWrapper::addCastInst(Instruction *inst)
{
  castInstList.push_back(dyn_cast<CastInst>(inst));
}

void pdg::FunctionWrapper::addIntrinsicInst(Instruction *inst)
{
  intrinsicInstList.push_back(dyn_cast<IntrinsicInst>(inst));
}

void pdg::FunctionWrapper::addDbgInst(Instruction *inst)
{
  dbgInstList.push_back(dyn_cast<DbgInfoIntrinsic>(inst));
}

void pdg::FunctionWrapper::addReturnInst(llvm::Instruction *inst)
{
  returnInstList.push_back(dyn_cast<ReturnInst>(inst));
}

void pdg::FunctionWrapper::setAST(AliasSetTracker* _AST)
{
  AST = _AST;
}


pdg::ArgumentWrapper *pdg::FunctionWrapper::getArgWByIdx(int idx)
{
  if (idx > argWList.size())
  {
    errs() << "request index excess argW list length... Return nullptr" << "\n";
    return nullptr;
  }
  return argWList[idx];
}