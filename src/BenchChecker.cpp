#ifndef BenchCheck
#define BenchCheck

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Pass.h"
#include "DebugInfoUtils.hpp"
#include "PDGUtils.hpp"
#include "AccessInfoTracker.hpp"
#include <set>
#include <queue>
#include <fstream>
#include <sstream> 
/*
The general lock finding algorithm is as follow:
1. find shared objects. 
2. find all critical sections.
3. find all the store instruction within critical sections. 
4. check if the modified pointer may points to shared objects. 
*/
namespace pdg
{
using namespace llvm;
class BenchChecker : public ModulePass
{
public:
  static char ID;
  BenchChecker() : ModulePass(ID)
  {
  }

  bool runOnModule(Module &M)
  {
    auto &pdgUtils = PDGUtils::getInstance();
    auto crossDomainFuncs = pdgUtils.computeCrossDomainFuncs(M);
    bool checkPass = true;
    for (auto crossDomainFunc : crossDomainFuncs)
    {
      DIType* funcDIType = DIUtils::getFuncDIType(crossDomainFunc);
      if (!funcSigTypeCheckPass(funcDIType))
      {
        checkPass = false;
        errs() << "func sig check fail for " << crossDomainFunc->getName() << "\n";
        errs() << "--------------------------------------------------------------\n";
      }
    }

    if (checkPass) 
      errs() << "Congrats! Validity Check Pass!" << "\n";
    return false;
  }

  bool funcSigTypeCheckPass(DIType* funcDIType)
  {
    if (DISubroutineType *subRoutine = dyn_cast<DISubroutineType>(funcDIType))
    {
      const auto &typeRefArr = subRoutine->getTypeArray();
      int i = 0; 
      int size = typeRefArr.size();
      while (i < size)
      {
        DIType* dt = typeRefArr[i].resolve();
        i++;
        if (dt == nullptr)
          continue;
        std::string invalidTypeStr = DIUtils::getInvalidTypeStr(dt);
        if (!invalidTypeStr.empty())
        {
          errs() << invalidTypeStr << " in arg " << (i - 1) << "\n";
          return false;
        }
      }
      return true;
    }
    return false;
  }

  private:
};
char BenchChecker::ID = 0;
static RegisterPass<BenchChecker> BenchChecker("bench-check", "Check benchmark viability", false, true);
} // namespace pdg

#endif