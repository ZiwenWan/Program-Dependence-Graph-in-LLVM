#include "ArgumentWrapper.hpp"
#include "PDGUtils.hpp"

using namespace llvm;

pdg::ArgumentWrapper::ArgumentWrapper(Argument *arg)
{
  this->arg = arg;
  this->Func = arg->getParent();
}

Argument *pdg::ArgumentWrapper::getArg()
{
  return arg;
}

tree<pdg::InstructionWrapper *> &pdg::ArgumentWrapper::getTree(TreeType treeTy)
{
  switch (treeTy)
  {
  case TreeType::FORMAL_IN_TREE:
    return formalInTree;
  case TreeType::FORMAL_OUT_TREE:
    return formalOutTree;
  case TreeType::ACTUAL_IN_TREE:
    return actualInTree;
  case TreeType::ACTUAL_OUT_TREE:
    return actualOutTree;
  default:
    return formalInTree;
  }
}

void pdg::ArgumentWrapper::copyTree(const tree<pdg::InstructionWrapper *> &srcTree, TreeType treeTy)
{
  if (srcTree.empty())
  {
    errs() << arg->getParent()->getName() << " arg : " << *arg << " srcTree is empty!\n";
    return;
  }

  GraphNodeType instWTy;
  //formal_out actual_in/out
  // choose new argument. CallInst should get arg from CallSite instead of formal function.
  Argument *newArg = nullptr;
  switch (treeTy)
  {
  case TreeType::FORMAL_IN_TREE:
    errs() << "FORMAL_IN_TREE can't be copied\n";
    break;
  case TreeType::FORMAL_OUT_TREE:
    formalOutTree = srcTree;
    newArg =  ((TreeTypeWrapper*)(*srcTree.begin()))->getArgument();
    instWTy = GraphNodeType::FORMAL_OUT;
    break;
  case TreeType::ACTUAL_IN_TREE:
    actualInTree = srcTree;
    newArg = this->getArg();
    instWTy = GraphNodeType::ACTUAL_IN;
    break;
  case TreeType::ACTUAL_OUT_TREE:
    actualOutTree = srcTree;
    newArg = this->getArg();
    instWTy = GraphNodeType::ACTUAL_OUT;
    break;
  }

  //    formalOutTree = formalInTree;
  tree<InstructionWrapper *>::iterator SI = srcTree.begin();
  tree<InstructionWrapper *>::iterator TI = getTree(treeTy).begin();
  for (; SI != srcTree.end() && TI != getTree(treeTy).end(); ++SI, ++TI)
  {
    InstructionWrapper *treeTypeW;
    InstructionWrapper *srcTreeW = (*SI);
    if (SI == srcTree.begin())
    {
      treeTypeW = new TreeTypeWrapper(srcTreeW->getFunction(), instWTy,
                                      newArg,
                                      ((TreeTypeWrapper *)srcTreeW)->getTreeNodeType(),
                                      ((TreeTypeWrapper *)srcTreeW)->getParentTreeNodeType(),
                                      ((TreeTypeWrapper *)srcTreeW)->getNodeOffset());
    }
    else
    {
      treeTypeW = new TreeTypeWrapper(srcTreeW->getFunction(), GraphNodeType::PARAMETER_FIELD,
                                      newArg,
                                      ((TreeTypeWrapper *)srcTreeW)->getTreeNodeType(),
                                      ((TreeTypeWrapper *)srcTreeW)->getParentTreeNodeType(),
                                      ((TreeTypeWrapper *)srcTreeW)->getNodeOffset());
    }
    *TI = treeTypeW;
    PDGUtils::getInstance().getFuncInstWMap()[srcTreeW->getFunction()].insert(treeTypeW);
  }
}

tree<pdg::InstructionWrapper *>::iterator pdg::ArgumentWrapper::tree_begin(TreeType treeTy)
{
  return getTree(treeTy).begin();
}

tree<pdg::InstructionWrapper *>::iterator pdg::ArgumentWrapper::tree_end(TreeType treeTy)
{
  return getTree(treeTy).end();
}