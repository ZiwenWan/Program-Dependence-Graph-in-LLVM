#ifndef PROGRAMDEPENDENCYGRAPH_H_
#define PROGRAMDEPENDENCYGRAPH_H_
#include "llvm/IR/Module.h"
#include "DependencyGraph.hpp"
#include "llvm/PassAnalysisSupport.h"
#include "ControlDependencyGraph.hpp"
#include "DataDependencyGraph.hpp"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"
#include "DebugInfoUtils.hpp"
#include "PDGCommandLineOptions.hpp"

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
  // PDG processing
  bool processCallInst(InstructionWrapper *instW);
  bool processIndirectCallInst(llvm::CallInst *CI, InstructionWrapper *instW);
  void addNodeDependencies(InstructionWrapper *instW);
  // parameter tree building
  std::vector<llvm::Function *> collectIndirectCallCandidates(llvm::FunctionType *funcType);
  void buildActualParameterTrees(llvm::CallInst *CI);
  void drawActualParameterTree(llvm::CallInst *CI, TreeType treeTy);
  void buildFormalTreeForFunc(llvm::Function *Func);
  void buildFormalTreeForArg(llvm::Argument &arg, TreeType treeTy);
  bool hasRecursiveType(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator insert_loc);
  bool isFilePtrOrFuncTy(llvm::Type* ty);
  InstructionWrapper* buildPointerTypeNode(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator);
  InstructionWrapper *buildPointerTypeNodeWithDI(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator, llvm::DIType *dt);
  void buildTypeTree(llvm::Argument &arg, InstructionWrapper *treeTyW, TreeType TreeType);
  void buildTypeTreeWithDI(llvm::Argument &arg, InstructionWrapper *treeTyW, TreeType TreeType, llvm::DIType* argDIType);
  void drawFormalParameterTree(llvm::Function *Func, TreeType treeTy);
  void connectFunctionAndFormalTrees(llvm::Function *callee);
  bool connectAllPossibleFunctions(llvm::CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates);
  bool connectCallerAndCallee(InstructionWrapper *instW, llvm::Function *callee);
  // field sensitive related functions
  void linkGEPsWithTree(llvm::CallInst* CI);
  InstructionWrapper *getActualTreeNodeGEP(InstructionWrapper* callInstW, unsigned field_offset, llvm::Type *treeNodeTy, llvm::Type *parentNodeTy);
  std::set<pdg::InstructionWrapper *> getReachableGEPs(InstructionWrapper *instW);
  std::set<pdg::InstructionWrapper *> getAllRelevantGEP(llvm::Argument &arg);
  InstructionWrapper *getTreeNodeGEP(llvm::Argument &arg, unsigned field_offset, llvm::Type *treeNodeTy, llvm::Type *parentNodeTy);
  std::vector<llvm::Instruction *> getArgStoreInsts(llvm::Argument &arg);
  // tree building helper functions
  bool isFuncTypeMatch(llvm::FunctionType *funcTy1, llvm::FunctionType *funcTy2);
  tree<InstructionWrapper *>::iterator getInstInsertLoc(ArgumentWrapper *argW, InstructionWrapper *tyW, TreeType treeTy);
  //  dep printer related functions
  std::vector<DependencyNode<InstructionWrapper> *> getNodeSet() { return PDG->getNodeSet(); }
  DependencyGraph<InstructionWrapper> *_getPDG() { return PDG; }
  typename DependencyNode<InstructionWrapper>::DependencyLinkList getNodeDepList(llvm::Instruction *inst);

private:
  llvm::Module *module;
  DependencyGraph<InstructionWrapper> *PDG;
  ControlDependencyGraph *cdg;
  DataDependencyGraph *ddg;
};
} // namespace pdg

#endif