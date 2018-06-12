#ifndef PDGHELPER_H
#define PDGHELPER_H

#include "DependencyGraph.h"
#include "FunctionWrapper.h"

namespace pdg {
    void constructInstMap(llvm::Function &F);

    void constructStructMap(llvm::Module &M, llvm::Instruction *pInstruction,
                            std::map <llvm::AllocaInst*, std::pair<StructType*, std::vector<Type*>>> &alloca_struct_map);

    void constructFuncMap(Module &M, std::map<const llvm::Function *, FunctionWrapper *> &funcMap);
}
#endif //PROJECT_PDGHELPER_H
