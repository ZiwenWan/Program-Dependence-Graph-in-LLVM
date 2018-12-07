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
  std::vector<std::pair<InstructionWrapper*, InstructionWrapper*>> paramCallInstPairs;
public:
  ArgumentWrapper() = delete;
  explicit ArgumentWrapper(llvm::Argument *arg);
  llvm::Argument *getArg();
  tree<InstructionWrapper *> &getTree(TreeType treeTy);
  std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>> getParamCallInstPair() const { return paramCallInstPairs; }
  void addParamCallInstW(std::pair<InstructionWrapper *, InstructionWrapper *> paramCallPair) { paramCallInstPairs.push_back(paramCallPair); }
  void copyTree(const tree<InstructionWrapper *> &srcTree, TreeType treeTy);
  tree<InstructionWrapper *>::iterator tree_begin(TreeType treeTy);
  tree<InstructionWrapper *>::iterator tree_end(TreeType treeTy);
};

} // namespace pdg

#endif