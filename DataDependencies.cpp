#include "DataDependencies.h"
//#include "FlowDependenceAnalysis.h"

using namespace llvm;

char pdg::DataDependencyGraph::ID = 0;

bool pdg::DataDependencyGraph::runOnFunction(llvm::Function &F) {
  AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  errs() << "++++++++++++++++++++++++++++++ DataDependency::runOnFunction "
          "+++++++++++++++++++++++++++++"
         << '\n';
  errs() << "Function name:" << F.getName().str() << '\n';
  constructFuncMap(*F.getParent(), funcMap);

  if (funcMap[&F]->getEntry() == NULL) {
    InstructionWrapper *root = new InstructionWrapper(&F, ENTRY);
    instnodes.insert(root);
    funcInstWList[&F].insert(root);
    funcMap[&F]->setEntry(root);
  }

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
        Type *alloca_type = allocainst->getAllocatedType();
        if (alloca_type->isStructTy() || alloca_type->isPointerTy()) {
          // check if the allocainst has been stored
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
                DDG->addDependency(instW, instMap[pInstruction], DATA_DEF_USE);
              }
            }
          } else {
            errs() << "Cast to Inst fail for GEP instruction" << "\n";
          }
        }
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
        if (DbgDeclareInst *ddi = dyn_cast<DbgDeclareInst>(pInstruction)) {
          errs() << "This is a dbg declare Inst (DDG)" << "\n";
          DILocalVariable *div = ddi->getVariable();
          errs() << div->getRawName()->getString().str() << "\n";
          // fetch metadata associate with the dbg inst
          MDNode *mdnode = dyn_cast<MDNode>(div->getRawType());
          DICompositeType *dct = dyn_cast<DICompositeType>(mdnode);

          std::string struct_name = dct->getName().str();
          for (auto node : dct->getElements()) {
            // retrive the name in the struct
            DIDerivedType *didt = dyn_cast<DIDerivedType>(node);
            std::string var_name = didt->getName().str();
            if (struct_fields_map.find(struct_name) == struct_fields_map.end()) {
              struct_fields_map[struct_name] = std::vector<std::string>();
              struct_fields_map[struct_name].push_back(var_name);
            } else {
              struct_fields_map[struct_name].push_back(var_name);
            }
          }
        }
      errs() << "This is a call Inst (DDG)" << "\n";
    }

    if (isa<LoadInst>(pInstruction)) {
      // dealing with dependencies in a function
      std::vector<Instruction *> flowdep_set =
              getDependencyInFunction(F, pInstruction);

      for (int i = 0; i < flowdep_set.size(); i++) {
        errs() << "Debugging flowdep_set:" << "\n";
        errs() << *flowdep_set[i] << "\n";
        //DDG->addDependency(instMap[flowdep_set[i]], instMap[pInstruction], DATA_RAW);
        DDG->addDependency(instMap[pInstruction], instMap[flowdep_set[i]], DATA_RAW);
        errs() << *pInstruction << "\n";
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
      errs() << "SmallVecter size = " << result.size() << '\n';
//      for (SmallVector<NonLocalDepResult, 20>::iterator II = result.begin(),
//                   EE = result.end();
//           II != EE; ++II) {

      for (NonLocalDepResult &I : result) {
        const MemDepResult &nonLocal_res = I.getResult();
        InstructionWrapper *itInst = instMap[&*instIt];
        InstructionWrapper *parentInst = instMap[nonLocal_res.getInst()];

        if (nullptr != nonLocal_res.getInst()) {
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

pdg::DataDependencyGraph *CreateDataDependencyGraphPass() {
  return new pdg::DataDependencyGraph();
}
