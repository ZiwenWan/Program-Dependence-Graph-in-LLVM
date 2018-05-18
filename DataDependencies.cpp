#include "DataDependencies.h"
//#include "FlowDependenceAnalysis.h"

using namespace llvm;

char DataDependencyGraph::ID = 0;

bool DataDependencyGraph::runOnFunction(llvm::Function &F) {
  AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  errs() << "++++++++++++++++++++++++++++++ DataDependency::runOnFunction "
          "+++++++++++++++++++++++++++++"
         << '\n';
  errs() << "Function name:" << F.getName().str() << '\n';

  if (instMap.empty() == false) {
    constructInstMap(F);
  }
  AliasAnalysis *AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  MemoryDependenceResults *MD =
          &getAnalysis<MemoryDependenceWrapperPass>().getMemDep();

  errs() << "After getAnalysis<MemoryDependenceAnalysis>()" << '\n';

  for (inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E;
       ++instIt) {
    DDG->getNodeByData(instMap[&*instIt]);

    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);

    // check for def-use dependencies
    for (Instruction::const_op_iterator cuit = pInstruction->op_begin();
         cuit != pInstruction->op_end(); ++cuit) {
      if (Instruction *pInst = dyn_cast<Instruction>(*cuit)) {
        //Value *tempV = dyn_cast<Value>(*cuit);
        DDG->addDependency(instMap[pInst], instMap[&*instIt],
                           DATA_DEF_USE);
      }
    }

    if(isa<CallInst>(pInstruction)) {
      errs() << "This is a call Inst (DDG)" << "\n";
    }

    if (isa<LoadInst>(pInstruction)) {
      // dealing with dependencies in a function
      std::vector<Instruction *> flowdep_set =
              getDependencyInFunction(F, pInstruction);

      for (int i = 0; i < flowdep_set.size(); i++) {
        DDG->addDependency(instMap[flowdep_set[i]], instMap[pInstruction],
                           DATA_RAW);
      }
      flowdep_set.clear();

      // dealing with non local pointer dependency, nonLocalPointer dep is
      // stored in result small vector
      SmallVector<NonLocalDepResult, 20> result;
      BasicBlock *BB = pInstruction->getParent();
      MemoryLocation Loc = MemoryLocation::get(dyn_cast<LoadInst>(pInstruction));
      // the return result is NonLocalDepResult. can use getAddress function
      MD->getNonLocalPointerDependency(pInstruction, result);
      // now result stores all possible
      for (SmallVector<NonLocalDepResult, 20>::iterator II = result.begin(),
                   EE = result.end();
           II != EE; ++II) {
        errs() << "SmallVecter size = " << result.size() << '\n';
        const MemDepResult nonLocal_res = II->getResult();
        InstructionWrapper *itInst = instMap[&*instIt];
        InstructionWrapper *parentInst = instMap[nonLocal_res.getInst()];

        if (nonLocal_res.getInst() != nullptr) {
          errs() << "nonLocal_res.getInst(): " << *nonLocal_res.getInst()
                 << '\n';
          DDG->addDependency(itInst, parentInst, DATA_GENERAL);
        } else {
          errs() << "nonLocal_res.getInst() is a nullptr" << '\n';
        }
      }

    }
  }
  return false;
}

void DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<AAResultsWrapperPass>();
  AU.addRequired<MemoryDependenceWrapperPass>();
  AU.setPreservesAll();
}

void DataDependencyGraph::print(raw_ostream &OS, const Module *) const {
  DDG->print(OS, (getPassName().data()));
}

static RegisterPass<DataDependencyGraph>
        DDG("ddg", "Data Dependency Graph Construction", false, true);

DataDependencyGraph *CreateDataDependencyGraphPass() {
  return new DataDependencyGraph();
}
