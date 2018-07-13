#include "DataDependencies.h"
//#include "FlowDependenceAnalysis.h"

using namespace llvm;

char pdg::DataDependencyGraph::ID = 0;

void pdg::DataDependencyGraph::initializeMemoryDependencyPasses() {
  AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  MD = &getAnalysis<MemoryDependenceWrapperPass>().getMemDep();
}

void pdg::DataDependencyGraph::constructFuncMapAndCreateFunctionEntry() {
  if (funcMap[func]->getEntry() == NULL) {
    InstructionWrapper *root = new InstructionWrapper(func, ENTRY);
    instnodes.insert(root);
    funcInstWList[func].insert(root);
    funcMap[func]->setEntry(root);
  }
}

void pdg::DataDependencyGraph::collectDefUseDependency(llvm::Instruction *inst) {
  // check for def-use dependencies
  for (Instruction::const_op_iterator cuit = inst->op_begin();
       cuit != inst->op_end(); ++cuit) {
    if (Instruction *pInst = dyn_cast<Instruction>(*cuit)) {
      //Value *tempV = dyn_cast<Value>(*cuit);
      DDG->addDependency(instMap[pInst], instMap[inst], DATA_DEF_USE);
    }
  }
}

void pdg::DataDependencyGraph::collectCallInstDependency(llvm::Instruction *inst) {
  if(isa<CallInst>(inst)) {
    DEBUG(dbgs() << "This is a call Inst (DDG)" << "\n");
  }
}

std::vector<Instruction *> pdg::DataDependencyGraph::getDependencyInFunction(Instruction *pLoadInst) {
  std::vector<Instruction *> _flowdep_set;
  std::list<StoreInst *> StoreVec = funcMap[func]->getStoreInstList();

  // for each Load Instruction, find related Store Instructions(alias considered)
  LoadInst *LI = dyn_cast<LoadInst>(pLoadInst);

  MemoryLocation LI_Loc = MemoryLocation::get(LI);
//  for (int j = 0; j < StoreVec.size(); j++) {
  for (StoreInst* SI : StoreVec) {
//    StoreInst *SI = dyn_cast<StoreInst>(StoreVec[j]);
    MemoryLocation SI_Loc = MemoryLocation::get(SI);
    AliasResult AA_result = AA->alias(LI_Loc, SI_Loc);
    if (AA_result != NoAlias) {
      _flowdep_set.push_back(SI);
    }
  }
  return _flowdep_set;
}

void pdg::DataDependencyGraph::collectRAWDependency(llvm::Instruction *inst) {
  // dealing with dependencies in a function
  std::vector<Instruction *> flowdep_set = getDependencyInFunction(inst);
  for (int i = 0; i < flowdep_set.size(); i++) {
    DEBUG(dbgs() << "Debugging flowdep_set:" << "\n");
    DEBUG(dbgs() << *flowdep_set[i] << "\n");
    DDG->addDependency(instMap[flowdep_set[i]], instMap[inst], DATA_RAW);
  }
  flowdep_set.clear();
}

void pdg::DataDependencyGraph::collectNonLocalDependency(llvm::Instruction *inst) {
  // dealing with non local pointer dependency, nonLocalPointer dep is stored in result small vector
  SmallVector<NonLocalDepResult, 20> result;
  BasicBlock *BB = inst->getParent();
  MemoryLocation Loc = MemoryLocation::get(dyn_cast<LoadInst>(inst));
  // the return result is NonLocalDepResult. can use getAddress function
  MD->getNonLocalPointerDependency(inst, result);
  // now result stores all possible
  DEBUG(dbgs() << "SmallVecter size = " << result.size() << '\n');

  for (NonLocalDepResult &I : result) {
    const MemDepResult &nonLocal_res = I.getResult();
    InstructionWrapper *itInst = instMap[inst];
    InstructionWrapper *parentInst = instMap[nonLocal_res.getInst()];

    if (nullptr != nonLocal_res.getInst()) {
      DEBUG(dbgs() << "nonLocal_res.getInst(): " << *nonLocal_res.getInst()
                   << '\n');
      DDG->addDependency(itInst, parentInst, DATA_GENERAL);
    } else {
      DEBUG(dbgs() << "nonLocal_res.getInst() is a nullptr" << '\n');
    }
  }

}

void pdg::DataDependencyGraph::collectDataDependencyInFunc() {
  for (inst_iterator instIt = inst_begin(func), E = inst_end(func); instIt != E;
       ++instIt) {
    DDG->getNodeByData(instMap[&*instIt]);
    llvm::Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);
    collectDefUseDependency(pInstruction);
    collectCallInstDependency(pInstruction);
    if (isa<LoadInst>(pInstruction)) {
      collectRAWDependency(pInstruction);
      collectNonLocalDependency(pInstruction);
    }
  }
}

bool pdg::DataDependencyGraph::runOnFunction(llvm::Function &F) {
  DEBUG(dbgs() << "++++++++++++++++++++++++++++++ DataDependency::runOnFunction "
          "+++++++++++++++++++++++++++++" << '\n');
  DEBUG(dbgs() << "Function name:" << F.getName().str() << '\n');
  func = &F;
  constructFuncMap(*func->getParent());
  initializeMemoryDependencyPasses();
  constructFuncMapAndCreateFunctionEntry();
  constructInstMap(F);
  collectDataDependencyInFunc();
  return false;
}

void pdg::DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<AAResultsWrapperPass>();
  AU.addRequired<MemoryDependenceWrapperPass>();
  AU.setPreservesAll();
}

void pdg::DataDependencyGraph::print(raw_ostream &OS, const Module *) const {
  DDG->print(OS, (getPassName().data()));
}

static RegisterPass<pdg::DataDependencyGraph>
        DDG("ddg", "Data Dependency Graph Construction", false, true);

//pdg::DataDependencyGraph *CreateDataDependencyGraphPass() {
//  return new pdg::DataDependencyGraph();
//}
