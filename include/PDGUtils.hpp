#ifndef PDGUTILS_H_
#define PDGUTILS_H_
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "FunctionWrapper.hpp"
#include "CallWrapper.hpp"
#include <set>
#include <map>

namespace pdg {
class PDGUtils final
{
  private:
    std::map<const llvm::Instruction *, InstructionWrapper *> G_instMap;
    std::set<InstructionWrapper *> G_globalInstsSet;
    std::map<const llvm::Function *, std::set<InstructionWrapper *>> G_funcInstWMap;
    std::map<const llvm::Function *, FunctionWrapper *> G_funcMap;
    std::map<const llvm::CallInst *, CallWrapper *> G_callMap;

  public:
    PDGUtils() = default;
    PDGUtils(const PDGUtils &) = delete;
    PDGUtils(PDGUtils &&) = delete;
    PDGUtils &operator=(const PDGUtils&) = delete;
    PDGUtils &operator=(PDGUtils &&) = delete;

    static PDGUtils& getInstance()
    {
      static PDGUtils pdgUtils { }; 
      return pdgUtils;
    };

    std::map<const llvm::Instruction *, InstructionWrapper *> &getInstMap() { return G_instMap; }
    std::set<InstructionWrapper *> &getGlobalInstsSet() { return G_globalInstsSet; }
    std::map<const llvm::Function *, std::set<InstructionWrapper *>> &getFuncInstWMap() { return G_funcInstWMap; }
    std::map<const llvm::Function *, FunctionWrapper *> &getFuncMap() { return G_funcMap; }
    std::map<const llvm::CallInst *, CallWrapper *> &getCallMap() { return G_callMap; }
    void collectGlobalInsts(llvm::Module &M);
    void categorizeInstInFunc(llvm::Function &F);
    void constructInstMap(llvm::Function &F);
    void constructFuncMap(llvm::Module &M);
};
} // namespace pdg
#endif