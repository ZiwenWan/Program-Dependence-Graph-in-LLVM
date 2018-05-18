#include "ControlDependencies.h"

using namespace llvm;

int ControlDependencyGraph::getDependenceType(const BasicBlock *A,
                                              const BasicBlock *B) const {
  assert(A && B);
  if (const llvm::BranchInst *b = dyn_cast<BranchInst>(A->getTerminator())) {
    if (b->isConditional()) {
      if (b->getSuccessor(0) == B) {
        return ControlType::TRUE;
      } else if (b->getSuccessor(1) == B) {
        return ControlType::FALSE;
      } else {
        errs() << *A << "\n" << *B << "\n";
        assert(false &&
               "Asking for edge type between unconnected basic blocks!");
      }
    }
  }
  return ControlType::OTHER;
}

void ControlDependencyGraph::computeDependencies(llvm::Function &F,
                                                 llvm::PostDominatorTree *PDT) {
  errs() << "++++++++++++++++++++++++++++++ ControlDependency::runOnFunction "
            "+++++++++++++++++++++++++++++"
         << '\n';
  /// Zhiyuan: explicitly construct the dummy ENTRY NODE:
  if(funcMap[&F]->getEntry() != NULL) {
      return;
  }
  root = new InstructionWrapper(&F, ENTRY);
  instnodes.insert(root);
  funcInstWList[&F].insert(root);

  errs() << " CDG.cpp after insert nodes.size " << instnodes.size() << '\n'
         << " Function: " << F.getName().str() << '\n';

  funcMap[&F]->setEntry(root);

  // may have changed to DomTreeNodeBase
  DomTreeNodeBase<BasicBlock> *node = PDT->getNode(&F.getEntryBlock());

  while (node && node->getBlock()) {
    // Walking the path backward and adding dependencies.
    addDependency(root, node->getBlock(), CONTROL);
    node = node->getIDom(); // const DomTreeNodeBase<NodeT> *IDom;
  }

  std::vector<std::pair<BasicBlock *, BasicBlock *>> EdgeSet;

  for (Function::iterator BI = F.begin(), E = F.end(); BI != E; ++BI) {
    BasicBlock *I = dyn_cast<BasicBlock>(BI);
    for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI) {
      assert(I && *SI);
      if (!PDT->dominates(*SI, I)) {
        BasicBlock *B_second = dyn_cast<BasicBlock>(*SI);
        EdgeSet.push_back(std::make_pair(I, B_second));
      }
    }
  }

  typedef std::vector<std::pair<BasicBlock *, BasicBlock *>>::iterator EdgeItr;

  errs() << "computerDependencies DEBUG 1\n";

  for (EdgeItr I = EdgeSet.begin(), E = EdgeSet.end(); I != E; ++I) {
    std::pair<BasicBlock *, BasicBlock *> Edge = *I;
    BasicBlock *L = PDT->findNearestCommonDominator(Edge.first, Edge.second);
    int type = getDependenceType(Edge.first, Edge.second);

    // capture loop dependence
    if (L == Edge.first) {
      //	errs() << "\t find A == L: " << L->getName() << "\n";
      addDependency(Edge.first, L, type);
      //     errs() << "DepType: " << type << "\n";
    }
    //      DomTreeNode *domNode = PDT[Edge.second];
    DomTreeNode *domNode = PDT->getNode(Edge.second);

    if (domNode == nullptr) {
      errs() << "domNode is null!\n";
      continue;
    }

    while (domNode->getBlock() != L) {
      addDependency(Edge.first, domNode->getBlock(), type);
      domNode = domNode->getIDom();
    }
  }

  // std::vector<std::pair<BasicBlock *, BasicBlock *> > EdgeSet;
  EdgeSet.clear();

  for (Function::iterator FI = F.begin(), E = F.end(); FI != E; ++FI) {
    /// Zhiyuan comment: find adjacent BasicBlock pairs in CFG, but the
    /// predecessor does not dominate successor.
    BasicBlock *I = dyn_cast<BasicBlock>(FI);
    for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI) {
      assert(I && *SI);
      if (!PDT->dominates(*SI, I)) {
        BasicBlock *B_second = dyn_cast<BasicBlock>(*SI);
        EdgeSet.push_back(std::make_pair(I, B_second));
      }
    }
  }
  llvm::errs() << "Finish Control Depen Analysis"
               << "\n";
}

void ControlDependencyGraph::addDependency(InstructionWrapper *from,
                                           llvm::BasicBlock *to, int type) {
  for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie;
       ++ii) {
    if (llvm::Instruction *Ins = llvm::dyn_cast<llvm::Instruction>(ii)) {
      if (llvm::DebugFlag) {
        llvm::errs() << "[i_cdg debug] dependence from type ("
                     << from->getType() << ") to instruction (" << *Ins
                     << ")\n";
      }
      InstructionWrapper *iw = instMap[Ins];
      CDG->addDependency(from, iw, type);
    }
  }
}

void ControlDependencyGraph::addDependency(llvm::BasicBlock *from,
                                           llvm::BasicBlock *to, int type) {
  Instruction *Ins = from->getTerminator();
  assert(Ins);
  InstructionWrapper *iw = instMap[Ins];
  // self loop
  if (from == to) {

    if (llvm::DebugFlag) {
      llvm::errs() << "[i_cdg debug] loop dependence from (" << *from
                   << ") to (" << *to << ")\n";
      llvm::errs() << "Terminator: " << *Ins << "\n";
    }
    for (llvm::BasicBlock::iterator ii = from->begin(), ie = from->end();
         ii != ie; ++ii) {
      Instruction *inst = dyn_cast<Instruction>(ii);
      InstructionWrapper *iwTo = instMap[inst];
      CDG->addDependency(iw, iwTo, type);
    }
  } else {
    if (llvm::DebugFlag) {
      llvm::errs() << "[i_cdg debug] dependence from (" << *from << ") to ("
                   << *to << ")\n";
      llvm::errs() << "Terminator: " << *Ins << "\n";
    }
    for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie;
         ++ii) {
      Instruction *inst = dyn_cast<Instruction>(ii);
      InstructionWrapper *iwTo = instMap[inst];
      CDG->addDependency(iw, iwTo, type);
    }
  }
}

bool ControlDependencyGraph::runOnFunction(Function &F) {
  if (instMap.empty() == false ) {
    constructInstMap(F);
  }
  PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
  computeDependencies(F, PDT);
  return false;
}

void ControlDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  // AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<PostDominatorTreeWrapperPass>();
}

void ControlDependencyGraph::print(raw_ostream &OS, const Module *) const {
  const char *passname = getPassName().data();
  CDG->print(OS, passname);
}

StringRef ControlDependencyGraph::getPassName() const {
  return "Control Dependency Graph";
}

void ControlDependencyGraph::mockLibraryCall(llvm::Function &F) {
  llvm::errs() << "ControlDependencies.h - setRootFor " << F.getName().str()
               << "\n";
  root = new InstructionWrapper(&F, ENTRY);
  isLibrary = true;
}

ControlDependencyGraph *CreateControlDependencyGraphPass() {
  return new ControlDependencyGraph();
}

char ControlDependencyGraph::ID = 0;

static RegisterPass<ControlDependencyGraph>
    CDG("cdg", "Control Dependency Graph Construction", false, true);