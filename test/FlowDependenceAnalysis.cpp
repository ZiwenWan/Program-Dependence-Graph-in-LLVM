#include "FlowDependenceAnalysis.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"


bool FlowDependenceAnalysis::runOnFunction(Function &F) {
    llvm::errs() << "Start run function flow ana!\n";
    return false;
}

void FlowDependenceAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

std::vector<Instruction *>
FlowDependenceAnalysis::getDependencyInFunction(Function &F,
                                                Instruction *pLoadInst) {
  //  AliasAnalysis &AA = getAnalysis<AliasAnalysis>();
  std::vector<Instruction *> _flowdep_set;
  // find and save all Store/Load instructions
  std::vector<Instruction *> StoreVec;
  for (inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E;
       ++instIt) {
    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);
    if (isa<StoreInst>(pInstruction))
      StoreVec.push_back(pInstruction);
  }
  // for each Load Instruction, find related Store Instructions(alias
  // considered)
  errs() << "LoadInst: " << *pLoadInst << '\n';
  LoadInst *LI = dyn_cast<LoadInst>(pLoadInst);

  MemoryLocation LI_Loc = MemoryLocation::get(LI);

  // TODO: global??
  //  errs() << "gp lise size = " << gp_list.size() << "\n";
  // errs() << "&&&&& location " << *gp_list[0]->getValue() << "&&&&&&\n";
  for (int j = 0; j < StoreVec.size(); j++) {
    StoreInst *SI = dyn_cast<StoreInst>(StoreVec[j]);
    MemoryLocation SI_Loc = MemoryLocation::get(SI);
    //      errs() << "SI =" << *SI << ' ';
    // errs() << "SI.Loc = " << SI_Loc.Ptr << " LI.Loc = " << LI_Loc.Ptr << ' ';
    AliasResult AA_result =
        ProgramDependencyGraph::Global_AA->alias(LI_Loc, SI_Loc);
    //      errs() << "@@@@@@Test Global AA :" << LI_Loc.Ptr << '\n';
    if (AA_result != NoAlias) {
      // errs() << "store instruction alias found! SI = " << *SI << " LI = " <<
      // *LI << '\n';
      _flowdep_set.push_back(StoreVec[j]);
    }
    // following code just for debugging
    // if(AA_result == AliasAnalysis::MayAlias)
    //	errs() << "AA_result == MayAlias! " << '\n';
    // for debugging
    //      if(AA_result == AliasAnalysis::MustAlias)
    //	errs() << "AA_result == MustAlias! " << '\n';
  }
  return _flowdep_set;
}


