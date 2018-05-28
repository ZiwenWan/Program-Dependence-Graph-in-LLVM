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

  constructInstMap(F);

  AliasAnalysis *AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  MemoryDependenceResults *MD =
          &getAnalysis<MemoryDependenceWrapperPass>().getMemDep();

  errs() << "After getAnalysis<MemoryDependenceAnalysis>()" << '\n';

  for (inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E;
       ++instIt) {
    DDG->getNodeByData(instMap[&*instIt]);

    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);

    if (auto allocainst = dyn_cast<AllocaInst>(pInstruction)) {
        // mark processed struct
      if (seen_structs[allocainst] == 0) {
        std::vector<Type *> fields = alloca_struct_map[allocainst].second;
        int k = 0;
        for (Type *field : fields) {
          // construct new InstructionWrapper with position info stored in field_id field.
          errs() << "Extracting fields ... " << "\n";
          InstructionWrapper *typeFieldW = new InstructionWrapper(pInstruction, &F, k, STRUCT_FIELD, field);
          instnodes.insert(typeFieldW);
          funcInstWList[&F].insert(typeFieldW);
          DDG->addDependency(typeFieldW, instMap[pInstruction], STRUCT_FIELDS);
          k++;
        }
      }
      seen_structs[allocainst] = 1;
    }

    // if find a getElementPtr type, it could be accessing the struct
    if (auto *gepI = dyn_cast<GetElementPtrInst>(pInstruction)) {
      if (gepI->getSourceElementType()->isStructTy()) {
        int operand_num = gepI->getNumOperands();
        // get the last operand, which is the position of the field
        Value *last_idx = gepI->getOperand(operand_num - 1);
        if (ConstantInt* constInt = dyn_cast<ConstantInt>(last_idx)) {
          // get the integer value of index
          int field_idx = constInt->getSExtValue();
          if (Instruction *source_alloca_inst = dyn_cast<Instruction>(gepI->getOperand(0))) {
            for (InstructionWrapper* instW : instnodes) {
              if (instW->getType() == STRUCT_FIELD && instW->getInstruction() == source_alloca_inst && instW->getFieldId() == field_idx) {
                //errs() << *(instW->getInstruction()) << "\n";
//                errs() << *pInstruction << "\n";
                //DDG->addDependency(instMap[pInstruction], instW, DATA_DEF_USE);
                DDG->addDependency(instW, instMap[pInstruction], DATA_DEF_USE);
              }
            }
          } else {
            errs() << "Cast to Inst fail for GEP instruction" << "\n";
          }
        }
        continue;
      }
    }

    // check for def-use dependencies
    for (Instruction::const_op_iterator cuit = pInstruction->op_begin();
         cuit != pInstruction->op_end(); ++cuit) {
      if (Instruction *pInst = dyn_cast<Instruction>(*cuit)) {
          //Value *tempV = dyn_cast<Value>(*cuit);
            DDG->addDependency(instMap[pInst], instMap[&*instIt], DATA_DEF_USE);
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
        errs() << "Debuggin flowdep_set:" << "\n";
        errs() << *flowdep_set[i] << "\n";
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
