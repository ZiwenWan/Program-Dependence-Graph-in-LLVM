#ifndef ACCESSINFO_TRACKER_H_
#define ACCESSINFO_TRACKER_H_
#include "llvm/IR/Module.h"
#include "llvm/PassAnalysisSupport.h"
#include "ProgramDependencyGraph.hpp"
#include "DebugInfoUtils.hpp"
#include <fstream>
#include <sstream>
#include <map>

namespace pdg
{
class AccessInfoTracker : public llvm::ModulePass
{
public:
  AccessInfoTracker() : llvm::ModulePass(ID) {}
  static char ID;
  bool runOnModule(llvm::Module &M);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
  std::vector<llvm::Instruction *> getArgStoreInsts(llvm::Argument &arg);
  std::set<InstructionWrapper *> getAliasStoreInstsForArg(ArgumentWrapper *argW);
  std::set<InstructionWrapper *> getAliasLoadInstsForArg(ArgumentWrapper *argW);
  void getIntraFuncReadWriteInfoForCallInsts(llvm::Function &Func, std::set<std::string> func_list);
  void printRetValueAccessInfo(llvm::Function &Func, std::set<std::string> func_list);
  void getIntraFuncReadWriteInfoForRetVal(CallWrapper *callW);
  void getIntraFuncReadWriteInfoForArg(ArgumentWrapper *argW);
  void getIntraFuncReadWriteInfoForFunc(llvm::Function &F);
  int getCallParamIdx(InstructionWrapper *instW, InstructionWrapper *callInstW);
  ArgumentMatchType getArgMatchType(llvm::Argument *arg1, llvm::Argument *arg2);
  std::vector<llvm::Instruction *> getArgStackAddrs(llvm::Argument *arg);
  void collectParamCallInstWForArg(ArgumentWrapper *argW);
  void mergeArgWAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW);
  void mergeTypeTreeAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper *>::iterator mergeTo, tree<InstructionWrapper *>::iterator mergeFrom);
  void getInterFuncReadWriteInfo(llvm::Function &F);
  AccessType getAccessTypeForInstW(InstructionWrapper *instW);
  void propergateAccessInfoToParent(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator treeI);
  void printFuncArgAccessInfo(llvm::Function &F);
  void printArgAccessInfo(ArgumentWrapper *argW, TreeType ty);
  void generateIDLForCallInsts(llvm::Function &F);
  void generateIDLforFunc(llvm::Function &F);
  void generateRpcForFunc(llvm::Function &F);
  void generateIDLForCallInstW(CallWrapper *CW);
  void generateIDLforArg(ArgumentWrapper *argW, TreeType ty);
  tree<InstructionWrapper *>::iterator generateIDLforStructField(ArgumentWrapper *argW, int subtreeSize, tree<InstructionWrapper *>::iterator treeI, std::stringstream &ss, TreeType ty);
  bool reach(InstructionWrapper *instW1, InstructionWrapper *instW2);
  std::string getArgAccessInfo(llvm::Argument &arg);
  ProgramDependencyGraph *_getPDG() { return PDG; }

private:
  ProgramDependencyGraph *PDG;
  std::ofstream idl_file;
  std::set<std::string> seen_projections;
};

bool isFuncPointer(llvm::Type *ty);
bool isStructPointer(llvm::Type *ty);
std::string getAccessAttribute(tree<InstructionWrapper *>::iterator treeI);

} // namespace pdg
#endif