#ifndef CONNECTFUNCTIONS_H_
#define CONNECTFUNCTIONS_H_

#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"

//#include "ProgramDependencies.h"
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

namespace pdg {
    //int buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, DataDependencyGraph &ddg);

    int buildActualTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, CallInst *CI);

    //void buildFormalTree(Argument *arg, TreeType treeTy, DataDependencyGraph &ddg);

    void buildActualTree(CallInst *CI, Argument *arg, TreeType treeTy);

    //void buildFormalParameterTrees(Function *Func, DataDependencyGraph &ddg);

    void buildActualParameterTrees(CallInst *CI);

    tree<InstructionWrapper*>::iterator getInstInsertLoc(ArgumentWrapper *argW, TypeWrapper *tyW, TreeType treeType);

    void insertArgToTree(TypeWrapper *tyW, ArgumentWrapper *pArgW, TreeType treeTy, tree<InstructionWrapper*>::iterator insertLoc);
}
#endif