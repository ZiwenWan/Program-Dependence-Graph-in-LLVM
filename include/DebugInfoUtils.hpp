#ifndef DEBUGINFOTOOLS_H_
#define DEBUGINFOTOOLS_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "PDGExceptions.hpp"

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
    static std::string getArgName(llvm::Argument &arg);
    static std::string getArgTypeName(llvm::Argument &arg);
};
}
#endif