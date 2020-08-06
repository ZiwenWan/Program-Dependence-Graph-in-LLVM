#ifndef LARGEALLOCSITE
#define LARGEALLOCSITE

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Pass.h"
#include "DebugInfoUtils.hpp"
#include "sea_dsa/CallGraphUtils.hh"
#include "sea_dsa/DsaAnalysis.hh"
#include "sea_dsa/Global.hh"
#include "sea_dsa/Local.hh"
#include "sea_dsa/Info.hh"
#include "sea_dsa/support/Debug.h"

#include <set>
#include <queue>
#include <fstream>
#include <sstream> 
/*
The general lock finding algorithm is as follow:
1. Find common alloctors sites: kzalloc, kmalloc etc
2. Obtian the size parameter
3. Check if the size parameter is computed from additional arithmic.
4. Print Warning accordingly.
*/
namespace pdg
{
using namespace llvm;
class LargeAllocSite : public ModulePass
{
public:
  static char ID;
  LargeAllocSite() : ModulePass(ID)
  {
  }

  bool runOnModule(Module &M)
  {
    std::set<std::string> allocFuncs {
      "kzalloc",
      "kmalloc",
      "kcalloc"
    };

    m_dsa = &getAnalysis<sea_dsa::DsaAnalysis>();
    unsigned warningNum = 0;
    for (auto &F : M)
    {
      if (F.isDeclaration() || F.empty())
        continue;

      if (isAllocFunc(F, allocFuncs))
        continue;

      for (auto instI = inst_begin(F); instI != inst_end(F); ++instI)
      {
        Instruction* curInst = &*instI;
        if (CallInst *ci = dyn_cast<CallInst>(curInst))
        {
          Function* calledFunc = ci->getCalledFunction();
          if (!calledFunc)
            continue;

          bool isAllocator = false;
          std::string calledFuncName = calledFunc->getName().str();
          for (auto allocFunc : allocFuncs)
          {
            if (calledFuncName.find(allocFunc) != std::string::npos)
            {
              isAllocator = true;
              break;
            }
          }

          if (isAllocator)
          {
            Value* sizeParam = ci->getOperand(0);
            if (Instruction *i = dyn_cast<Instruction>(sizeParam))
            {
              errs() << "[WARNING LARGE ALLOC SITE - " << warningNum << " ] \n";
              errs() << "Alloc Site: " << *ci << "\n";
              errs() << "Function: " << F.getName() << "\n";
              errs() << "File: " << F.getSubprogram()->getFilename() << "\n";
              errs() << "Line Number: " << ci->getDebugLoc()->getLine() << "\n";
              errs() << " ----------------------------------------------------- \n";
              warningNum++;
            }
          }
        }
      }
    }
    return false;
  }

  std::set<Instruction *> getIntraFuncAlias(Instruction *inst)
  {
    Function *F = inst->getFunction();
    std::set<Instruction *> aliasSet;
    for (auto instI = inst_begin(F); instI != inst_end(F); instI++)
    {
      if (&*instI == inst)
        continue;
      if (mayAlias(*inst, *instI, *F))
        aliasSet.insert(&*instI);
    }
    return aliasSet;
  }

  bool mayAlias(Value &V1, Value &V2, Function &F)
  {
    sea_dsa::Graph *G = &m_dsa->getDsaAnalysis().getGraph(F);
    if (!G)
      return false;
    if (!G->hasCell(V1) || !G->hasCell(V2))
      return false;
    auto const &c1 = G->getCell(V1);
    auto const &c2 = G->getCell(V2);
    auto const &s1 = c1.getNode()->getAllocSites();
    auto const &s2 = c2.getNode()->getAllocSites();
    for (auto const a1 : s1)
    {
      if (s2.find(a1) != s2.end())
        return true;
    }
    return false;
  }

  bool isAllocFunc(Function &F, std::set<std::string> allocFuncs)
  {
    for (auto allocFunc : allocFuncs)
    {
      std::string funcName = F.getName().str();
      if (funcName.find(allocFunc) != std::string::npos)
        return true;
    }
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const
  {
    AU.addRequired<sea_dsa::DsaAnalysis>();
  }

  private:
    sea_dsa::DsaAnalysis *m_dsa;
};
char LargeAllocSite::ID = 0;
static RegisterPass<LargeAllocSite> LargeAllocSite("large-alloc", "Find memory allocation for multiple objects", false, true);
} // namespace pdg

#endif