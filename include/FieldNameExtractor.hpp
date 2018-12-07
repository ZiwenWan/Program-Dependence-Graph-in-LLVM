#ifndef FIELDNAMEEXTRACTOR_H_
#define FIELDNAMEEXTRACTOR_H_

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"

#include "llvm/ADT/Statistic.h"

#include <vector>
#include <set>
#include <map>
#include <string.h>

namespace pdg
{
class FieldNameExtractor : public llvm::ModulePass
{
public:
  static char ID;
  FieldNameExtractor() : llvm::ModulePass(ID){};
  std::map<std::string, std::vector<std::string>> struct_field_name_map;
  typedef std::map<unsigned, std::pair<std::string, llvm::DIType *>> offsetNames;
  std::map<std::string, offsetNames> seenStructs;
  // store argument offset correspond to arg field names. (pointer type)
  std::map<llvm::Function *, std::map<unsigned, offsetNames>> funcArgOffsetMap;
  //std::map<Function*, offsetNames> funcArgOffsetMap;
  std::string moduleName = "[dsa-gen]";
  llvm::DIType *getBaseType(llvm::DIType *Ty);
  llvm::DIType *getLowestDINode(llvm::DIType *Ty);
  std::map<llvm::Function *, std::map<unsigned, offsetNames>> getFuncArgOffsetNames() { return funcArgOffsetMap; }
  std::string getStructName(llvm::DIType *Ty);
  int getAllNames(llvm::DIType *Ty, std::set<std::string> seen_names, offsetNames &of, int prev_off, std::string baseName, std::string indent, llvm::StringRef argName, std::string &structName);
  offsetNames getArgFieldNames(llvm::Function *F, unsigned argNumber, llvm::StringRef argName, std::string &structName);
  std::vector<llvm::DbgDeclareInst *> getDbgDeclareInstInFunction(llvm::Function *F);
  std::vector<llvm::MDNode *> getParameterNodeInFunction(llvm::Function *F);
  bool runOnModule(llvm::Module &M);
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const { AU.setPreservesAll(); }
};
} // namespace pdg
#endif