#include "PDGHelper.h"
using namespace llvm;
using namespace pdg;

std::map<const Function *, FunctionWrapper *> pdg::funcMap;
std::map<const CallInst *, CallWrapper *> pdg::callMap;
std::set<InstructionWrapper *> pdg::instnodes;
std::set<InstructionWrapper *> pdg::globalList;
std::map<const Instruction *, InstructionWrapper *> pdg::instMap;
std::map<const Function *, std::set<InstructionWrapper *>> pdg::funcInstWList;
std::map<AllocaInst*, std::pair<StructType*, std::vector<Type*>>> pdg::alloca_struct_map;
std::map<std::string, std::vector<std::string>> pdg::struct_fields_map;
std::map<AllocaInst*, int> pdg::seen_structs;

void pdg::constructInstMap(llvm::Function &F) {
        for (llvm::inst_iterator I = inst_begin(F), IE = inst_end(F); I != IE; ++I) {
            // llvm::errs() << "Current InstMap Size: " << instMap.size() << "\n";
            // if not in instMap yet, insert
            Instruction *tmpInst = &*I;

            if (instMap.find(&*I) == instMap.end()) {
                InstructionWrapper *iw = new InstructionWrapper(&*I, &F, INST);
                instnodes.insert(iw);
                // instMap is used to store instruction that have seen
                instMap[&*I] = iw;
                // added in funcInstWList, indicate which instruction belongs to which
                // function
                funcInstWList[&F].insert(iw);
            }
        }
}

void pdg::constructStructMap(llvm::Module &M,
                             llvm::Instruction *pInstruction,
                             std::map<llvm::AllocaInst *, std::pair<StructType *, std::vector<Type *>>> &alloca_struct_map)
{
        AllocaInst *allocaInst = dyn_cast<AllocaInst>(pInstruction);
        // constructing struct
        std::vector<llvm::StructType *> global_struct_list = M.getIdentifiedStructTypes();
        for (auto st : global_struct_list) {
            errs() << "Struct Name:" << st->getName().str().substr(7) << "\n";
            if (allocaInst->getAllocatedType()->getStructName() == st->getName()) {
                std::vector<Type *> fields;
                std::pair<StructType *, std::vector<Type *>> struct_pair;

                StructType::element_iterator SB = st->element_begin();
                StructType::element_iterator SE = st->element_end();
                while (SB != SE) {
                    // get the type for each field
                    auto type = *SB;
                    // add each field to vector
                    fields.push_back(type);
                    SB++;
                }
                // store the vector with corresponding in the map
                //structMap[st] = fields;
                struct_pair.first = st;
                struct_pair.second = fields;
                alloca_struct_map[allocaInst] = struct_pair;
            }
        }
        errs() << "Construct struct map success !" << "\n";
        errs() << "Struct Map size: " << alloca_struct_map.size() << "\n";
}

void pdg::constructFuncMap(Module &M, std::map<const Function *, FunctionWrapper *> &funcMap) {
        for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
            Function *f = dyn_cast<Function>(F);
            if (funcMap.find(f) == funcMap.end()) // if not in funcMap yet, insert
            {
                FunctionWrapper *fw = new FunctionWrapper(f);
                funcMap[f] = fw;
            }
        }
}
