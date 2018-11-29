#ifndef ARGUMENTWRAPPER_H_
#define ARGUMENTWRAPPER_H_
#include "llvm/IR/Argument.h"
#include "InstructionWrapper.hpp"
#include "PDGEnums.hpp"
#include "tree.hh"

namespace pdg
{
class ArgumentWrapper
{
private:
  llvm::Argument *arg;
  llvm::Function *Func;
  tree<InstructionWrapper *> formalInTree;
  tree<InstructionWrapper *> formalOutTree;
  tree<InstructionWrapper *> actualInTree;
  tree<InstructionWrapper *> actualOutTree;

public:
  ArgumentWrapper() = delete;
  explicit ArgumentWrapper(llvm::Argument *arg);
  llvm::Argument *getArg();
  tree<InstructionWrapper *> &getTree(TreeType treeTy);
  void copyTree(const tree<InstructionWrapper *> &srcTree, TreeType treeTy);
};

} // namespace pdg

#endif