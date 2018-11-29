#ifndef PROGRAMDEPENDENCYGRAPH_H_
#define PROGRAMDEPENDENCYGRAPH_H_
#include "llvm/IR/Module.h"
#include "DependencyGraph.hpp"
#include "llvm/PassAnalysisSupport.h"
#include "ControlDependencyGraph.hpp"
#include "DataDependencyGraph.hpp"
#include "AllPasses.hpp"

namespace pdg
{
class ProgramDependencyGraph : public llvm::ModulePass
{
public:
  static char ID;
  ProgramDependencyGraph() : llvm::ModulePass(ID) { PDG = new DependencyGraph<InstructionWrapper>(); }
  ~ProgramDependencyGraph() { delete PDG; }
  bool runOnModule(llvm::Module &M);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
  llvm::StringRef getPassName() { return "Program Dependency Graph"; }
  bool processCallInst(InstructionWrapper *instW);
  bool processIndirectCallInst(llvm::CallInst *CI, InstructionWrapper *instW);
  void addNodeDependencies(InstructionWrapper *instW);
  std::vector<llvm::Function *> collectIndirectCallCandidates(llvm::FunctionType *funcType);
  bool isFuncTypeMatch(llvm::FunctionType *funcTy1, llvm::FunctionType *funcTy2);
  void buildActualParameterTrees(llvm::CallInst *CI);
  void drawActualParameterTree(llvm::CallInst *CI, TreeType treeTy);
  void buildFormalTreeForFunc(llvm::Function* Func);
  void buildFormalTreeForArg(llvm::Argument *arg, TreeType treeTy);
  void buildTypeTree(llvm::Argument *arg, InstructionWrapper *treeTyW, TreeType TreeType);
  void drawFormalParameterTree(llvm::Function *Func, TreeType treeTy);
  void connectFunctionAndFormalTrees(llvm::Function *callee);
  bool connectAllPossibleFunctions(llvm::CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates);
  bool connectCallerAndCallee(InstructionWrapper* instW, llvm::Function* callee);
  // field sensitive related functions
  std::set<pdg::InstructionWrapper *> getAllRelevantGEP(llvm::Argument *arg);
  std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>> getTreeNodeGEPPair(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI);
  void linkTypeNodeWithGEPInst(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI);
  std::vector<llvm::Instruction*> getArgStoreInsts(llvm::Argument *arg);
  tree<InstructionWrapper*>::iterator getInstInsertLoc(ArgumentWrapper *argW, InstructionWrapper *tyW, TreeType treeTy);
  std::vector<DependencyNode<InstructionWrapper> *> getNodeSet() { return PDG->getNodeSet(); }
  DependencyGraph<InstructionWrapper> *_getPDG() { return PDG; }

private:
  llvm::Module *module;
  DependencyGraph<InstructionWrapper> *PDG;
  ControlDependencyGraph *cdg;
  DataDependencyGraph *ddg;
};
} // namespace pdg

#endif