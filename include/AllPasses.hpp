#ifndef ALLPASS_H_
#define ALLPASS_H_
#include "llvm/Pass.h"

namespace pdg
{
class ControlDependencyGraph;
class DataDependencyGraph;
class ProgramDependencyGraph;

ControlDependencyGraph *CreateControlDependencyGraphPass();
DataDependencyGraph *CreateDataDependencyGraphPass();
ProgramDependencyGraph *CreateProgramDependencyGraphPass();
} // namespace pdg

namespace llvm {
void initializeControlDependencyGraphPass(PassRegistry &Registry);
class PassRegistry;
void initializeControlDependencyGraphPass(PassRegistry &Registry);
void initializeDataDependencyGraphPass(PassRegistry &Registry);
void initializeProgramDependencyGraphPass(PassRegistry &Registry);
void initializeProgramDependencyPrinterPass(PassRegistry &Registry);
}

#endif