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

  //if (instMap.size() == 0) {
  constructInstMap(F);
  //}
  //FlowDependenceAnalysis &MDA = getAnalysis<FlowDependenceAnalysis>(F);
  //errs() << "After getAnalysis<FlowDependenceAnalysis>()" << '\n';

  for (inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E;
       ++instIt) {

    DDG->getNodeByData(instMap[&*instIt]);

    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);

    if (isa<LoadInst>(pInstruction)) {
      // std::vector<Instruction *> flowdep_set =
      //     MDA.getDependencyInFunction(F, pInstruction);

      std::vector<Instruction *> flowdep_set =
          getDependencyInFunction(F, pInstruction);
      for (int i = 0; i < flowdep_set.size(); i++) {
        DDG->addDependency(instMap[flowdep_set[i]],
                           instMap[pInstruction], DATA_RAW);
      }
      flowdep_set.clear();
    }

    std::vector<Instruction *> flowdep_set;

    for (Instruction::const_op_iterator cuit = instIt->op_begin();
         cuit != instIt->op_end(); ++cuit) {
      if (Instruction *pInstruction = dyn_cast<Instruction>(*cuit)) {
        Value *tempV = dyn_cast<Value>(*cuit);
        DDG->addDependency(instMap[pInstruction], instMap[&*instIt],
                           DATA_DEF_USE);
      }
    }
  }
  return false;
}

void DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<AAResultsWrapperPass>(); 
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
