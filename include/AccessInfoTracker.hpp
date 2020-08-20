#ifndef ACCESSINFO_TRACKER_H_

#define ACCESSINFO_TRACKER_H_
#include "llvm/IR/Module.h"
#include "llvm/PassAnalysisSupport.h"
#include "ProgramDependencyGraph.hpp"
#include "llvm/Analysis/CallGraph.h" 
#include "DebugInfoUtils.hpp"
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
  void computeFuncAccessInfo(llvm::Function &F);
  void computeFuncAccessInfoBottomUp(llvm::Function &F);
  std::vector<llvm::Function *> computeBottomUpCallChain(llvm::Function &F);
  void computeArgAccessInfo(ArgumentWrapper *argW, TreeType treeTy);
  void computeIntraprocArgAccessInfo(ArgumentWrapper *argW, llvm::Function &F);
  void computeInterprocArgAccessInfo(ArgumentWrapper *argW, llvm::Function &F);
  std::map<std::string, AccessType> computeInterprocAccessedFieldMap(llvm::Function &callee, unsigned argNo);
  std::string getRegisteredFuncPtrName(std::string funcName);
  std::set<llvm::Value *> findAliasInDomainWithOffset(llvm::Value &V, llvm::Function &F, unsigned offset, std::set<llvm::Function *> receiverDomainTrans);
  std::set<llvm::Value *> findAliasInDomain(llvm::Value &V, llvm::Function &F, std::set<llvm::Function *> domainTransitiveClosure);
  std::set<llvm::Function *> getTransitiveClosureInDomain(llvm::Function &F, std::set<llvm::Function *> searchDomain);
  void getIntraFuncReadWriteInfoForCallee(llvm::Function &F);
  int getCallParamIdx(const InstructionWrapper *instW, const InstructionWrapper *callInstW);
  ArgumentMatchType getArgMatchType(llvm::Argument *arg1, llvm::Argument *arg2);
  void mergeArgAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper*>::iterator callerTreeI);
  AccessType getAccessTypeForInstW(const InstructionWrapper *instW);
  void printFuncArgAccessInfo(llvm::Function &F);
  void printArgAccessInfo(ArgumentWrapper *argW, TreeType ty);
  void generateIDLForCallInsts(llvm::Function &F);
  void generateIDLforFunc(llvm::Function &F);
  void generateIDLforFuncPtr(llvm::Type* ty, std::string funcName, llvm::Function& F);
  void generateIDLforFuncPtrWithDI(llvm::DIType *funcDIType, llvm::Module *module, std::string funcPtrName);
  void generateRpcForFunc(llvm::Function &F);
  void generateIDLForCallInstW(CallWrapper *CW);
  void generateIDLforArg(ArgumentWrapper *argW, TreeType ty, std::string funcName = "", bool handleFuncPtr = false);
  tree<InstructionWrapper *>::iterator generateIDLforStructField(ArgumentWrapper *argW, int subtreeSize, tree<InstructionWrapper *>::iterator treeI, std::stringstream &ss, TreeType ty);
  std::string getArgAccessInfo(llvm::Argument &arg);
  std::string getAllocAttribute(std::string projStr, bool isPassedToCallee);
  void computeAccessedFieldsInStructType(std::string structTypeName);
  // compute Shared Data Based On Type
  void computeSharedData();
  void computeSharedDataInFunc(llvm::Function &F);
  std::set<std::string> computeAccessedFieldsForDIType(tree<InstructionWrapper *> objectTree, llvm::DIType *rootDIType);
  std::set<std::string> computeSharedDataForType(llvm::DIType* dt);
  std::set<std::string> computeAccessedDataInDomain(llvm::DIType* dt, std::set<llvm::Function*> domain);
  void inferAsynchronousCalledFunction(std::set<llvm::Function *> crossDomainFuncs);
  bool isChildFieldShared(llvm::DIType* argDIType, llvm::DIType* fieldDIType);
  ProgramDependencyGraph *_getPDG() { return PDG; }
  std::map<std::string, std::set<std::string>> getSharedDataTypeMap() { return sharedDataTypeMap; }
  std::string getReturnAttributeStr(llvm::Function &F);
  bool mayAlias(llvm::Value &V1, llvm::Value &V2, llvm::Function &F);
  std::set<llvm::Instruction *> getIntraFuncAlias(llvm::Instruction *inst);
  uint64_t getArrayArgSize(llvm::Value &V, llvm::Function &F);
  int getCallOperandIdx(llvm::Value *operand, llvm::CallInst *callInst);
  std::string switchIndirectCalledPtrName(std::string funcptr);
  std::string inferFieldAnnotation(InstructionWrapper* instW);
  bool voidPointerHasMultipleCasts(InstructionWrapper *voidPtrW);
  void initializeNumStats();
  unsigned computeUsedGlobalNumInDriver();
  void printNumStats();

private:
  ProgramDependencyGraph *PDG;
  llvm::Module *module;
  llvm::CallGraph *CG;
  std::ofstream idl_file;
  std::set<llvm::Function *> kernelDomainFuncs;
  std::set<llvm::Function *> driverDomainFuncs;
  std::set<llvm::Function*> importedFuncs;
  std::set<std::string> driverExportFuncPtrNames;
  std::set<std::string> accessedFieldsInAsyncCalls;
  std::set<llvm::Function*> asyncCalls;
  std::map<std::string, std::string> driverExportFuncPtrNameMap;
  std::set<std::string> usedCallBackFuncs;
  std::map<std::string, std::set<std::string>> sharedDataTypeMap;
  std::map<std::string, llvm::DIType*> diTypeNameMap;
  std::map<std::string, AccessType> globalFieldAccessInfo;
  std::set<std::string> seenFuncOps;
  std::set<std::string> stringOperations;
  bool crossBoundary; // indicate whether transitive closure cross two domains
  unsigned numEliminatedPrivateFields;
  unsigned totalNumOfFields;
  unsigned numProjectedFields;
  unsigned savedSyncDataSize;
  unsigned privateDataSize;
  unsigned unionNum;
  unsigned unNamedUnionNum;
  unsigned voidPointerNum;
  unsigned unhandledVoidPointerNum;
  unsigned pointerArithmeticNum;
  unsigned sentialArrayNum;
  unsigned arrayNum;
  unsigned unHandledArrayNum;
  unsigned stringNum;
};

std::string getAccessAttributeName(tree<InstructionWrapper *>::iterator treeI);

} // namespace pdg
#endif