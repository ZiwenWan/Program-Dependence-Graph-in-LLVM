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
    static llvm::DIType* getLowestDIType(llvm::DIType* dt);
    static llvm::DIType* getBaseDIType(llvm::DIType* dt);
    static llvm::DIType *getArgDIType(llvm::Argument &arg);
    static llvm::DIType *getFuncRetDIType(llvm::Function &F);
    static std::string getDIFieldName(llvm::DIType *dt);
    static std::string getDITypeName(llvm::DIType *dt);
    static std::string getFuncDITypeName(llvm::DIType *dt, std::string funcName);
    static std::string getArgName(llvm::Argument &arg, std::vector<llvm::DbgDeclareInst*> dbgInstList);
    static std::string getArgTypeName(llvm::Argument &arg);
    static std::string getFuncSigName(llvm::DIType *ty, std::string funcName = "", bool callFromDev = true);
};
}
#endif