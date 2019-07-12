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
  void getIntraFuncReadWriteInfoForCallInsts(llvm::Function &Func);
  void printRetValueAccessInfo(llvm::Function &Func);
  void getIntraFuncReadWriteInfoForRetVal(CallWrapper *callW);
  void getIntraFuncReadWriteInfoForArg(ArgumentWrapper *argW);
  void getIntraFuncReadWriteInfoForFunc(llvm::Function &F);
  int getCallParamIdx(const InstructionWrapper *instW, const InstructionWrapper *callInstW);
  ArgumentMatchType getArgMatchType(llvm::Argument *arg1, llvm::Argument *arg2);
  void mergeArgAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper*>::iterator treeI);
  bool getInterFuncReadWriteInfo(llvm::Function &F);
  AccessType getAccessTypeForInstW(const InstructionWrapper *instW);
  void printFuncArgAccessInfo(llvm::Function &F);
  void printArgAccessInfo(ArgumentWrapper *argW, TreeType ty);
  void generateIDLForCallInsts(llvm::Function &F);
  void generateIDLforFunc(llvm::Function &F);
  void generateIDLforFuncPtr(llvm::Type* ty, std::string funcName, llvm::Function& F);
  void generateIDLforFuncPtrWithDI(llvm::DIType* funcDIType, llvm::Module* module);
  void generateRpcForFunc(llvm::Function &F);
  void generateIDLForCallInstW(CallWrapper *CW);
  void generateIDLforArg(ArgumentWrapper *argW, TreeType ty, std::string funcName = "", bool handleFuncPtr = true, bool passToCallee = true);
  tree<InstructionWrapper *>::iterator generateIDLforStructField(ArgumentWrapper *argW, int subtreeSize, tree<InstructionWrapper *>::iterator treeI, std::stringstream &ss, TreeType ty);
  std::string getArgAccessInfo(llvm::Argument &arg);
  std::string getAllocAttribute(std::string projStr, bool isPassedToCallee);
  ProgramDependencyGraph *_getPDG() { return PDG; }

private:
  ProgramDependencyGraph *PDG;
  std::ofstream idl_file;
  std::set<std::string> deviceObjStore;
  std::set<std::string> kernelObjStore; 
  std::set<std::string> importedFuncList;
  std::set<std::string> kernelFuncList;
  std::set<std::string> definedFuncList;
  std::set<std::string> blackFuncList;
  std::set<std::string> staticFuncptrList;
  std::set<std::string> staticFuncList;
  std::map<std::string, std::string> driverFuncPtrCallTargetMap;
};

std::string getAccessAttributeName(tree<InstructionWrapper *>::iterator treeI);

} // namespace pdg
#endif