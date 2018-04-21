#ifndef CONNECTFUNCTIONS_H_
#define CONNECTFUNCTIONS_H_

#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"

#include "ProgramDependencies.h"
#include "SystemDataDependencies.h"
#include "SystemControlDependenceGraph.h"

//#include "AllPasses.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include <map>
#include <list>
#include <vector>
#include <iostream>

using namespace llvm;

extern std::map<const Function *, FunctionWrapper *> funcMap;
extern std::map<const CallInst *, CallWrapper *> callMap;
extern std::set<InstructionWrapper *> instnodes;
extern std::set<InstructionWrapper *> globalList;
extern std::map<const llvm::Instruction *, InstructionWrapper *> instMap;
extern std::map<const llvm::Function *, std::set<InstructionWrapper *>>
    funcInstWList;

int buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy);

int buildActualTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, CallInst *CI);

void buildFormalTree(Argument *arg, TreeType treeTy);

void buildActualTree(CallInst* CI, Argument *arg, TreeType treeTy);

void buildFormalParameterTrees(Function* Func);

void buildActualParameterTrees(CallInst *CI);

#endif