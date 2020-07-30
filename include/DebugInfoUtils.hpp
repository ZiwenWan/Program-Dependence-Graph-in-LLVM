#ifndef DEBUGINFOTOOLS_H_
#define DEBUGINFOTOOLS_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IntrinsicInst.h"
#include "PDGExceptions.hpp"
#include <set>

namespace pdg
{
class DIUtils
{
  public:
    static llvm::DIType *getLowestDIType(llvm::DIType *dt);
    static llvm::DIType *getBaseDIType(llvm::DIType *dt);
    static llvm::DIType *getArgDIType(llvm::Argument &arg);
    static llvm::DIType *getFuncRetDIType(llvm::Function &F);
    static llvm::DIType *getGlobalVarDIType(llvm::GlobalVariable &globalVar);
    static std::string getDIFieldName(llvm::DIType *dt);
    static std::string getDITypeName(llvm::DIType *dt);
    // static std::string getFuncDITypeName(llvm::DIType *dt, std::string funcName);
    static std::string getArgName(llvm::Argument &arg, std::vector<llvm::DbgInfoIntrinsic *> dbgInstList);
    static std::string getArgTypeName(llvm::Argument &arg);
    static std::string getFuncSigName(llvm::DIType *ty, llvm::Function *F = nullptr, std::string funcPtrName = "", std::string funcName = "", bool callFromDev = true);
    static void printStructFieldNames(llvm::DINodeArray DINodeArr);
    static bool isPointerType(llvm::DIType *dt);
    static bool isStructPointerTy(llvm::DIType *dt);
    static bool isStructTy(llvm::DIType *dt);
    static bool isFuncPointerTy(llvm::DIType *dt);
    static llvm::DIType *stripMemberTag(llvm::DIType *dt);
    static llvm::DIType *getFuncDIType(llvm::Function* func);
    static llvm::DIType *getInstDIType(llvm::Instruction* inst, std::vector<llvm::DbgInfoIntrinsic*> dbgInstList);
    static llvm::DbgDeclareInst *getDbgInstForInst(llvm::Instruction *inst, std::set<llvm::DbgDeclareInst *> dbgInstList);
    static std::set<llvm::DbgInfoIntrinsic *> collectDbgInstInFunc(llvm::Function &F);
    static std::vector<llvm::Function *> collectIndirectCallCandidatesWithDI(llvm::DIType *funcDIType, llvm::Module *module, std::map<std::string, std::string> funcptrTargetMap);
    static std::set<std::string> computeSharedDataType(llvm::Module &M, std::set<llvm::Function *> crossDomainFunctions);
    static std::string computeFieldID(llvm::DIType *rootType, llvm::DIType *fieldType);
    static std::string getInvalidTypeStr(llvm::DIType *dt);
    static bool isUnionType(llvm::DIType *dt);
    static bool isArrayType(llvm::DIType *dt);
};
}
#endif