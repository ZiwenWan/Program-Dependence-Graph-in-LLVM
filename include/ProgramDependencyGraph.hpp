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
#include <fstream>
#include <sstream>


namespace pdg
{
class ProgramDependencyGraph : public llvm::ModulePass
{
public:
  static char ID;
  ProgramDependencyGraph() : llvm::ModulePass(ID) { PDG = new DependencyGraph<InstructionWrapper>(); }
  ~ProgramDependencyGraph() { delete PDG; }
  bool runOnModule(llvm::Module &M);
  void buildPDGForFunc(llvm::Function *Func);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
  llvm::StringRef getPassName() { return "Program Dependency Graph"; }
  // PDG processing
  bool processCallInst(InstructionWrapper *instW);
  bool processIndirectCallInst(llvm::CallInst *CI, InstructionWrapper *instW);
  void addNodeDependencies(InstructionWrapper *instW);
  // parameter tree building
  std::vector<llvm::Function *> collectIndirectCallCandidates(llvm::FunctionType *functionType, llvm::Function &oriFunc, const std::set<std::string> &filterFuncs = std::set<std::string>());
  void copyFormalTreeToActualTree(llvm::CallInst *CI, llvm::Function* func);
  void buildActualParameterTrees(llvm::CallInst *CI);
  void drawActualParameterTree(llvm::CallInst *CI, TreeType treeTy);
  void buildFormalTreeForFunc(llvm::Function *Func);
  void buildFormalTreeForArg(llvm::Argument &arg, TreeType treeTy);
  bool hasRecursiveType(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator insert_loc);
  bool isFilePtrOrFuncTy(llvm::Type* ty);
  InstructionWrapper* buildPointerTypeNode(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator);
  InstructionWrapper *buildPointerTypeNodeWithDI(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator, llvm::DIType *dt);
  void buildTypeTree(llvm::Argument &arg, InstructionWrapper *treeTyW, TreeType TreeType);
  void buildTypeTreeWithDI(llvm::Argument &arg, InstructionWrapper *treeTyW, TreeType TreeType, llvm::DIType *argDIType);
  void drawFormalParameterTree(llvm::Function *Func, TreeType treeTy);
  void connectFunctionAndFormalTrees(llvm::Function *callee);
  void connectCallerAndActualTrees(llvm::Function* caller);
  bool connectAllPossibleFunctions(llvm::CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates);
  void connectActualTreeToFormalTree(llvm::CallInst *CI, llvm::Function *called_func);
  bool connectCallerAndCallee(InstructionWrapper *instW, llvm::Function *callee);
  // field sensitive related functions
  std::vector<llvm::Instruction *> getArgStoreInsts(llvm::Argument &arg);
  llvm::Instruction *getArgAllocaInst(llvm::Argument &arg);
  llvm::Value *getCallSiteParamVal(llvm::CallInst* CI, unsigned idx);
  // tree building helper functions
  std::vector<InstructionWrapper*> getReadInstsOnInst(llvm::Instruction* inst);
  std::vector<InstructionWrapper *> getAllAlias(llvm::Instruction *inst);
  bool isFuncTypeMatch(llvm::FunctionType *funcType1, llvm::FunctionType *funcType2);
  bool isTreeNodeGEPMatch(InstructionWrapper *treeNode, llvm::Instruction *GEP);
  bool isIndirectCallOrInlineAsm(llvm::CallInst *CI);
  bool nameMatch(std::string str1, std::string str2);
  tree<InstructionWrapper *>::iterator getInstInsertLoc(ArgumentWrapper *argW, InstructionWrapper *tyW, TreeType treeTy);
  //  dep printer related functions
  std::vector<DependencyNode<InstructionWrapper> *> getNodeSet() { return PDG->getNodeSet(); }
  DependencyGraph<InstructionWrapper> *_getPDG() { return PDG; }
  typename DependencyNode<InstructionWrapper>::DependencyLinkList getNodeDepList(llvm::Instruction *inst);
  typename DependencyNode<InstructionWrapper>::DependencyLinkList getNodesWithDepType(const InstructionWrapper* instW, DependencyType depType);
  llvm::Function* getCalledFunction(llvm::CallInst* CI);
  bool isFuncPointer(llvm::Type *ty);
  bool isStructPointer(llvm::Type *ty);

private:
  llvm::Module *module;
  DependencyGraph<InstructionWrapper> *PDG;
  ControlDependencyGraph *cdg;
  DataDependencyGraph *ddg;
};
} // namespace pdg

#endif