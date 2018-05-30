#ifndef DATADEPENDENCIES_H
#define DATADEPENDENCIES_H

//#include "AllPasses.h"
#include "DependencyGraph.h"
#include "FunctionWrapper.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Pass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Support/raw_ostream.h"

//#include "llvm/Analysis/AliasAnalysis.h"
extern std::map<const Function *, FunctionWrapper *> funcMap;
extern std::set<InstructionWrapper *> instnodes;
extern std::set<InstructionWrapper *> instnodes;
extern std::set<InstructionWrapper *> globalList;
extern std::map<const llvm::Instruction *, InstructionWrapper *> instMap;
extern std::map<const llvm::Function *, std::set<InstructionWrapper *>>
    funcInstWList;

static llvm::ModRefInfo GetLocation(const llvm::Instruction *Inst,
                                    llvm::MemoryLocation &Loc,
                                    llvm::AliasAnalysis *AA);

llvm::ModRefInfo GetLocation(const llvm::Instruction *Inst,
                             llvm::MemoryLocation &Loc,
                             llvm::AliasAnalysis *AA);

typedef DependencyGraph<InstructionWrapper> DataDepGraph;

static std::vector<InstructionWrapper *> gp_list;

class DataDependencyGraph : public llvm::FunctionPass {
public:
  static char ID; // Pass ID, replacement for typeid
  DataDepGraph *DDG;
  
  DataDependencyGraph() : FunctionPass(ID) { DDG = new DataDepGraph(); }

  ~DataDependencyGraph() {
    releaseMemory();
    delete DDG;
  }

    virtual bool runOnFunction(llvm::Function &F);

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

//    virtual std::string getFieldName(MDNode* meta,int offset){
//        if(!meta){
//            errs()<<"The meta is NULL\n";
//            return "";
//        }
//        DIVariable div(meta);
//        DIType dit=div.getType();
//        DIDerivedType didt=dyn_cast<DIDerivedType>(dit);
//        DICompositeType dict=dyn_cast<DICompositeType>(didt.getTypeDerivedFrom());
//        DIArray dia=dict.getTypeArray();
//        assert(offset<dia.getNumElements());
//        DIType field=dyn_cast<DIType>(dia.getElement(offset));
//        //errs()<<"Field'name is "<<field.getName()<<"\n";
//        return field.getName();
//    }

    virtual llvm::StringRef getPassName() const {
        return "Data Dependency Graph";
    }

    virtual std::vector<Instruction *>
    getDependencyInFunction(Function &F, Instruction *pLoadInst) {
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
            // errs() << "SI.Loc = " << SI_Loc.Ptr << " LI.Loc = " << LI_Loc.Ptr << '
            // ';
            AliasResult AA_result =
                    AA->alias(LI_Loc, SI_Loc);
            //      errs() << "@@@@@@Test Global AA :" << LI_Loc.Ptr << '\n';
            if (AA_result != NoAlias) {
                // errs() << "store instruction alias found! SI = " << *SI << " LI = "
                // << *LI << '\n';
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

    virtual void print(llvm::raw_ostream &OS, const llvm::Module *M = 0) const;

private:
    AliasAnalysis *AA;
};

namespace llvm {

    template <>
    struct GraphTraits<DataDependencyGraph *> : public GraphTraits<DepGraph *> {
        static NodeRef getEntryNode(DataDependencyGraph *DG) {
            return *(DG->DDG->begin_children());
        }

        static nodes_iterator nodes_begin(DataDependencyGraph *DG) {
            return DG->DDG->begin_children();
        }

        static nodes_iterator nodes_end(DataDependencyGraph *DG) {
            return DG->DDG->end_children();
        }
    };

} // namespace llvm

#endif