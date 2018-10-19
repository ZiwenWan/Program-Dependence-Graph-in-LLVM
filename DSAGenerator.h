#ifndef DSAGEN_H_
#define DSAGEN_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"

#include "ProgramDependencies.h"
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


class DSAGenerator : public llvm::ModulePass {
public:
    static char ID;

    std::map<std::string, std::vector<std::string>> struct_field_name_map;

    typedef std::map<unsigned, std::pair<std::string, DIType *>> offsetNames;

    std::map<std::string, offsetNames> seenStructs;

    // store argument offset correspond to arg field names. (pointer type)
    std::map<Function*, std::map<unsigned, offsetNames>> funcArgOffsetMap;

    //std::map<Function*, offsetNames> funcArgOffsetMap;

    std::string moduleName = "[dsa-gen]";

    DSAGenerator() : llvm::ModulePass(ID) {
        //DSAGenerator *DSA = new DSAGenerator();
    }

    DIType* getBaseType(DIType* Ty);

    DIType* getLowestDINode(DIType* Ty);

    std::map<Function*, std::map<unsigned, offsetNames>> getFuncArgOffsetNames() {
        return funcArgOffsetMap;
    }

    std::string getStructName(DIType* Ty);

    int getAllNames(DIType *Ty, std::set<std::string> seen_names, offsetNames &of, int prev_off, std::string baseName, std::string indent, StringRef argName, std::string &structName);

    offsetNames getArgFieldNames(Function *F, unsigned argNumber, StringRef argName, std::string& structName);

    std::vector<DbgDeclareInst *> getDbgDeclareInstInFunction(Function *F);

    std::vector<MDNode*> getParameterNodeInFunction(Function *F);

    void dumpOffsetNames(offsetNames &of);

    bool runOnModule(llvm::Module &M);

    void getAnalysisUsage(AnalysisUsage &AU) const {
        AU.setPreservesAll();
    }
};

#endif