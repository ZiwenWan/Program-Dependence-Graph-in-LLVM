#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "PDGUtils.hpp"
#include "ProgramDependencyGraph.hpp"

using namespace llvm;
namespace 
{
#ifndef CLASS_TEST_HELPERS_INCLUDED
#define CLASS_TEST_HELPERS_INCLUDED

class PDGTest {
public:
  void parseAssembly(const char *Assembly)
  {
    SMDiagnostic Error;
    M = parseAssemblyString(Assembly, Error, Context);

    std::string errMsg;
    raw_string_ostream os(errMsg);
    Error.print("", os);

    if (!M)
      report_fatal_error(os.str());
  }

  void testParaMeterTreeSize()
  {
    auto &pdgUtils = pdg::PDGUtils::getInstance();
    for (auto FI = M->begin(); FI != M->end(); ++FI)
    {
      if (FI->isDeclaration())
        continue;
      pdgUtils.collectGlobalInsts(*M);
      pdgUtils.constructFuncMap(*M);
      pdgUtils.categorizeInstInFunc(*FI);
      pdgUtils.constructInstMap(*FI);
    }
    REQUIRE(pdgUtils.getInstMap().size() == 8);
    REQUIRE(pdgUtils.getFuncMap().size() == 1);
    REQUIRE(pdgUtils.getCallMap().size() == 0);
    REQUIRE(pdgUtils.getGlobalInstsSet().size() == 1); // the format string is defined as global
    REQUIRE(pdgUtils.getFuncInstWMap().size() == 1);
  }

  LLVMContext Context;
  std::unique_ptr<Module> M;
};
#endif

TEST_CASE_METHOD(PDGTest, "PDG test build", "[pdg]") {
  std::string moduleStr = R"(
    @.str = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
    define i32 @main() #0 {
      %1 = alloca i32, align 4
      %2 = alloca i8*, align 8
      store i32 0, i32* %1, align 4
      %3 = call i8* @malloc(i64 10) #3
      store i8* %3, i8** %2, align 8
      %4 = load i8*, i8** %2, align 8
      %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32     0), i8* %4)
      ret i32 0
    }
    declare i8* @malloc(i64) 
    declare i32 @printf(i8*, ...) 
  )";
  parseAssembly(moduleStr.c_str());  
  testParaMeterTreeSize();      
}
}

