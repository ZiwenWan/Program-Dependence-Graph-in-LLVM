#ifndef PDGUTILS_H_
#define PDGUTILS_H_
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InlineAsm.h"
#include "FunctionWrapper.hpp"
#include "CallWrapper.hpp"
#include "sea_dsa/CallGraphUtils.hh"
#include "sea_dsa/DsaAnalysis.hh"
#include "sea_dsa/Global.hh"
#include "sea_dsa/Local.hh"
#include "sea_dsa/Info.hh"
#include "sea_dsa/support/Debug.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <map>

namespace pdg {
class PDGUtils final
{
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
    void setDsaAnalysis(sea_dsa::DsaAnalysis *_m_dsa) { m_dsa = _m_dsa; }
    sea_dsa::DsaAnalysis *getDsaAnalysis() { return m_dsa; }
    // functions used for PDG construction optimization
    std::set<llvm::Function *> computeCrossDomainFuncs(llvm::Module &M);
    std::set<llvm::Function *> computeDriverDomainFuncs(llvm::Module &M);
    std::set<llvm::Function *> computeKernelDomainFuncs(llvm::Module &M);
    std::set<llvm::Function *> computeTransitiveClosure(llvm::Function &F);
    std::set<std::string> computeDriverExportFuncPtrName();
    std::map<std::string, std::string> computeDriverExportFuncPtrNameMap();
    std::set<llvm::Function *> getTransitiveClosureInDomain(llvm::Function &F, std::set<llvm::Function *> searchDomain);

  private:
    std::map<const llvm::Instruction *, InstructionWrapper *> G_instMap;
    std::set<InstructionWrapper *> G_globalInstsSet;
    std::map<const llvm::Function *, std::set<InstructionWrapper *>> G_funcInstWMap;
    std::map<const llvm::Function *, FunctionWrapper *> G_funcMap;
    std::map<const llvm::CallInst *, CallWrapper *> G_callMap;
    sea_dsa::DsaAnalysis *m_dsa;
};
} // namespace pdg
#endif