#ifndef INSTRUCTIONWRAPPER_H_
#define INSTRUCTIONWRAPPER_H_
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Argument.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Support/raw_ostream.h"
#include "PDGEnums.hpp"

namespace pdg
{
class InstructionWrapper {
  public:
    InstructionWrapper(llvm::Function *Func, GraphNodeType nodetype)
    {
      this->Inst = nullptr;
      this->Func = Func;
      this->BB = nullptr;
      this->value = nullptr;
      this->nodetype = nodetype;
      this->isVisited = false;
      this->access_type = AccessType::NOACCESS;
    }

    InstructionWrapper(llvm::Instruction *Inst, GraphNodeType nodeType)
    {
      this->Inst = Inst;
      this->Func = Inst->getFunction();
      this->BB = Inst->getParent();
      this->value = llvm::cast<llvm::Value>(Inst);
      this->nodetype = nodeType;
      this->isVisited = false;
      this->access_type = AccessType::NOACCESS;
    }

    InstructionWrapper(llvm::Value *value, GraphNodeType nodeType)
    {
      this->Inst = nullptr;
      this->Func = nullptr;
      this->BB = nullptr;
      this->value = value;
      this->nodetype = nodeType;
      this->isVisited = false;
      this->access_type = AccessType::NOACCESS;
    }

    llvm::BasicBlock *getBasicBlock() const { return BB; }
    llvm::Instruction *getInstruction() const { return Inst; }
    llvm::Function *getFunction() const { return Func; }
    llvm::Value *getValue() const { return value; }
    GraphNodeType getGraphNodeType() const { return nodetype; }
    void setGraphNodeType(GraphNodeType _nodeType) { this->nodetype = _nodeType; }
    bool getVisited() const { return isVisited; }
    void setVisited(const bool _isVisited) { isVisited = _isVisited; }
    AccessType getAccessType() { return access_type; }
    void setAccessType(AccessType _access_type) { access_type = _access_type; }
    virtual llvm::Type *getLLVMType() const { return nullptr; }
    virtual llvm::Type *getParentLLVMType() const { return nullptr; }
    virtual llvm::Argument *getArgument() const { return nullptr; }
    virtual int getNodeOffset() const { return -1; }
    virtual llvm::DIType *getDIType() const { return nullptr; }
    virtual std::vector<std::string> getBitFieldsName() const { return std::vector<std::string>(); }

  private:
    llvm::Instruction *Inst;
    llvm::Value *value;
    llvm::BasicBlock *BB;
    llvm::Function *Func;
    GraphNodeType nodetype;
    bool isVisited;
    AccessType access_type;
};

class TreeTypeWrapper : public InstructionWrapper
{
  public:
    TreeTypeWrapper(llvm::Function *Func,
                    GraphNodeType nodetype,
                    llvm::Argument *arg,
                    llvm::Type *treeNodeType,
                    llvm::Type *parentTreeNodeType, 
                    int node_offset) : InstructionWrapper(Func, nodetype)
    {
      this->arg = arg;
      this->treeNodeType = treeNodeType;
      this->parentTreeNodeType = parentTreeNodeType;
      this->node_offset = node_offset;
      this->dt = nullptr;
    }

    TreeTypeWrapper(llvm::Function *Func,
                    GraphNodeType nodetype,
                    llvm::Argument *arg,
                    llvm::Type *treeNodeType,
                    llvm::Type *parentTreeNodeType, 
                    int node_offset,
                    llvm::DIType* dt) : InstructionWrapper(Func, nodetype)
    {
      this->arg = arg;
      this->treeNodeType = treeNodeType;
      this->parentTreeNodeType = parentTreeNodeType;
      this->node_offset = node_offset;
      this->dt = dt;
    }

    TreeTypeWrapper(llvm::Value *val,
                    GraphNodeType nodetype,
                    int node_offset,
                    llvm::DIType *dt) : InstructionWrapper(val, nodetype)
    {
      this->treeNodeType = nullptr;
      this->parentTreeNodeType = nullptr;
      this->node_offset = node_offset;
      this->dt = dt;
    }

    llvm::Type *getLLVMType() const override { return treeNodeType; }
    llvm::Type *getParentLLVMType() const override { return parentTreeNodeType; }
    llvm::Argument *getArgument() const override { return arg; }
    int getNodeOffset() const override { return node_offset; }
    llvm::DIType *getDIType() const override { return dt; }

  private:
    llvm::Argument *arg;
    llvm::Type *treeNodeType;
    llvm::Type *parentTreeNodeType;
    int node_offset;
    llvm::DIType *dt;
};

} // namespace pdg
#endif