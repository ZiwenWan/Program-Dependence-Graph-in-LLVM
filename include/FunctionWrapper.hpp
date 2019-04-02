#ifndef FUNCTIONWRAPPER_H_
#define FUNCTIONWRAPPER_H_
#include "llvm/IR/Function.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "InstructionWrapper.hpp"
#include "tree.hh"
#include <vector>
#include "ArgumentWrapper.hpp"

namespace pdg
{
class FunctionWrapper
{
public:
  FunctionWrapper() = delete;
  FunctionWrapper(llvm::Function *Func);
  bool hasTrees();
  void setTreeFlag(bool treeFlag) { this->treeFlag = treeFlag; }
  bool isVisited();
  void setVisited(bool visited) { this->visited = visited; }
  void setEntryW(InstructionWrapper *entryW) { this->entryW = entryW; }
  InstructionWrapper *getEntryW() { return entryW; }
  std::vector<llvm::StoreInst *> &getStoreInstList() { return storeInstList; }
  std::vector<llvm::LoadInst *> &getLoadInstList() { return loadInstList; }
  std::vector<llvm::Instruction *> &getReturnInstList() { return returnInstList; }
  std::vector<llvm::CallInst *> &getCallInstList() { return callInstList; }
  std::vector<llvm::CastInst *> &getCastInstList() { return castInstList; }
  std::vector<llvm::IntrinsicInst *> &getIntrinsicInstList() { return intrinsicInstList; }
  std::vector<llvm::DbgDeclareInst *> &getDbgDeclareInstList() { return dbgDeclareInst; }
  std::vector<ArgumentWrapper *> &getArgWList() { return argWList; }
  void addStoreInst(llvm::Instruction *inst);
  void addLoadInst(llvm::Instruction *inst); 
  void addReturnInst(llvm::Instruction *inst) { returnInstList.push_back(inst); }
  void addCallInst(llvm::Instruction *inst);
  void addCastInst(llvm::Instruction *inst);
  void addIntrinsicInst(llvm::Instruction *inst);
  void addDbgInst(llvm::Instruction *inst);
  ArgumentWrapper *getArgWByArg(llvm::Argument &arg);
  ArgumentWrapper *getArgWByIdx(int idx);
  ArgumentWrapper *getRetW() { return retW; }

private:
  llvm::Function *Func;
  InstructionWrapper *entryW;
  std::vector<llvm::StoreInst *> storeInstList;
  std::vector<llvm::LoadInst *> loadInstList;
  std::vector<llvm::Instruction *> returnInstList;
  std::vector<llvm::CallInst *> callInstList;
  std::vector<llvm::CastInst *> castInstList;
  std::vector<llvm::IntrinsicInst *> intrinsicInstList;
  std::vector<llvm::DbgDeclareInst *> dbgDeclareInst;
  std::vector<ArgumentWrapper *> argWList;
  std::set<llvm::Function *> dependent_funcs;
  std::set<llvm::Value *> ptrSet;
  ArgumentWrapper* retW; 
  bool treeFlag;
  bool RWFlag;
  bool visited;
};
} // namespace pdg

#endif