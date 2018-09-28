#include "PDGHelper.h"
using namespace llvm;
using namespace pdg;

std::map<const Function *, FunctionWrapper *> pdg::funcMap;
std::map<const CallInst *, CallWrapper *> pdg::callMap;
std::set<InstructionWrapper *> pdg::instnodes;
std::set<InstructionWrapper *> pdg::globalList;
std::map<const Instruction *, InstructionWrapper *> pdg::instMap;
std::map<const Function *, std::set<InstructionWrapper *>> pdg::funcInstWList;

void pdg::constructInstMap(llvm::Function &F) {
        for (llvm::inst_iterator I = inst_begin(F), IE = inst_end(F); I != IE; ++I) {
            // llvm::errs() << "Current InstMap Size: " << instMap.size() << "\n";
            // if not in instMap yet, insert

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

void pdg::constructFuncMap(Module &M) {
        for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
            Function *f = dyn_cast<Function>(F);
            constructInstMap(*f);
            if (funcMap.find(f) == funcMap.end()) // if not in funcMap yet, insert
            {
                FunctionWrapper *fw = new FunctionWrapper(f);
                funcMap[f] = fw;
            }
        }
}

llvm::Instruction* pdg::getArgStoreInst(Argument *arg)
{
    for (auto UI = arg->user_begin(); UI != arg->user_end(); ++UI)
    {
        if (isa<StoreInst>(*UI))
        {
            Instruction* st = dyn_cast<Instruction>(*UI); 
            return st;
        }
    }
    return nullptr;
}

llvm::Instruction* pdg::getArgAllocaInst(Argument *arg)
{
    Instruction *inst = getArgStoreInst(arg);
    if (inst == nullptr)
    {
        return nullptr;
    }

    StoreInst *st = dyn_cast<StoreInst>(inst);
    if (isa<AllocaInst>(st->getPointerOperand()))
    {
        Instruction* ai = dyn_cast<Instruction>(st->getPointerOperand());
        return ai;
    }

    return nullptr;
}

void pdg::cleanupGlobalVars() {
    funcMap.clear();
    callMap.clear();
    instnodes.clear();
    globalList.clear();
    instMap.clear();
    funcInstWList.clear();
}

