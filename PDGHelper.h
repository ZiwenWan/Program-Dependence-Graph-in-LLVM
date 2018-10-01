#ifndef PDGHELPER_H
#define PDGHELPER_H

#include "DependencyGraph.h"
#include "FunctionWrapper.h"

namespace pdg {
    void constructInstMap(llvm::Function &F);

    void constructFuncMap(Module &M);

    void cleanupGlobalVars();
}
#endif //PROJECT_PDGHELPER_H
