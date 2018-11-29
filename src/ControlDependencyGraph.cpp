#include "ControlDependencyGraph.hpp"

using namespace llvm;

StringRef pdg::ControlDependencyGraph::getPassName() const
{
  return "Control Dependency Graph";
}

void pdg::ControlDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<PostDominatorTreeWrapperPass>();
  AU.setPreservesAll();
}

void pdg::ControlDependencyGraph::addInstToBBDependency(InstructionWrapper *from, BasicBlock *to, DependencyType depType)
{
  for (BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii)
  {
    if (Instruction *Ins = dyn_cast<Instruction>(ii))
    {
      InstructionWrapper *iw = PDGUtils::getInstance().getInstMap()[Ins];
      CDG->addDependency(from, iw, depType);
    }
  }
}

void pdg::ControlDependencyGraph::addBBToBBDependency(BasicBlock *from, BasicBlock *to, DependencyType type)
{
  Instruction *Ins = from->getTerminator();
  InstructionWrapper *iw = PDGUtils::getInstance().getInstMap()[Ins];
  // self loop
  if (from == to)
  {
    for (BasicBlock::iterator ii = from->begin(), ie = from->end(); ii != ie; ++ii)
    {
      Instruction *inst = dyn_cast<Instruction>(ii);
      InstructionWrapper *iwTo = PDGUtils::getInstance().getInstMap()[inst];
      CDG->addDependency(iw, iwTo, type);
    }
  }
  else
  {
    for (BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii)
    {
      Instruction *inst = dyn_cast<Instruction>(ii);
      InstructionWrapper *iwTo = PDGUtils::getInstance().getInstMap()[inst];
      CDG->addDependency(iw, iwTo, type);
    }
  }
}

void pdg::ControlDependencyGraph::computeDependencies(Function &F)
{
  PDGUtils::getInstance().constructFuncMap(*F.getParent());
  if (PDGUtils::getInstance().getFuncMap()[&F]->getEntryW() != nullptr)
  {
    return;
  }

  InstructionWrapper *entryW = new InstructionWrapper(&F, GraphNodeType::ENTRY);
  PDGUtils::getInstance().getFuncInstWMap()[&F].insert(entryW);
  PDGUtils::getInstance().getFuncMap()[&F]->setEntryW(entryW);

  DomTreeNodeBase<BasicBlock> *node = PDT->getNode(&F.getEntryBlock());
  while (node && node->getBlock()) {
    // Walking the path backward and adding dependencies.
    addInstToBBDependency(entryW, node->getBlock(), DependencyType::CONTROL);
    node = node->getIDom();
  }
  std::vector<std::pair<BasicBlock *, BasicBlock *>> EdgeSet;

  for (Function::iterator BI = F.begin(), E = F.end(); BI != E; ++BI)
  {
    BasicBlock *I = dyn_cast<BasicBlock>(BI);
    for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI)
    {
      assert(I && *SI);
      if (!PDT->dominates(*SI, I))
      {
        BasicBlock *B_second = dyn_cast<BasicBlock>(*SI);
        EdgeSet.push_back(std::make_pair(I, B_second));
      }
    }
  }

  for (auto Edge : EdgeSet) {
    BasicBlock *L = PDT->findNearestCommonDominator(Edge.first, Edge.second);

    // capture loop dependence
    if (L == Edge.first)
    {
      addBBToBBDependency(Edge.first, L, DependencyType::CONTROL);
    }
    DomTreeNode *domNode = PDT->getNode(Edge.second);
    if (domNode == nullptr)
    {
      continue;
    }
    while (domNode->getBlock() != L)
    {
      addBBToBBDependency(Edge.first, domNode->getBlock(), DependencyType::CONTROL);
      domNode = domNode->getIDom();
    }
  }

  EdgeSet.clear();
  for (Function::iterator FI = F.begin(), E = F.end(); FI != E; ++FI)
  {
    // Zhiyuan comment: find adjacent BasicBlock pairs in CFG, but the predecessor
    //does not dominate successor.
    BasicBlock *I = dyn_cast<BasicBlock>(FI);
    for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI)
    {
      assert(I && *SI);
      if (!PDT->dominates(*SI, I))
      {
        BasicBlock *B_second = dyn_cast<BasicBlock>(*SI);
        EdgeSet.push_back(std::make_pair(I, B_second));
      }
    }
  }
}

bool pdg::ControlDependencyGraph::runOnFunction(Function &F)
{
  PDGUtils::getInstance().constructInstMap(F);
  PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
  computeDependencies(F); 
  return false;
}

ControlDependencyGraph *CreateControlDependencyGraphPass()
{
  return new ControlDependencyGraph();
}

char ControlDependencyGraph::ID = 0;

static RegisterPass<pdg::ControlDependencyGraph>
    CDG("cdg", "Control Dependency Graph Construction", false, true);
