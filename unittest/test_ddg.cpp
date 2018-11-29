#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "llvm/Pass.h"
#include "llvm/AsmParser/Parser.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"

#include "../AllPasses.h"
#include "../DataDependencies.h"


using namespace llvm;

namespace {
	class DataDependencyGraphTest {
		public:
		// this function is used to parse IR
		void parseAssembly(StringRef AssemblyString) {
			SMDiagnostic Err;
			M = parseAssemblyString(AssemblyString, Err, C);
			std::string errMsg;
			raw_string_ostream os(errMsg);
			Err.print("", os);

			if (!M) {
				report_fatal_error(os.str().c_str());
			}

			Function *F = M->getFunction("test");
	    if (F == nullptr)
	      report_fatal_error("Test must have a function named @test");
			
			A = B = nullptr;
			for (inst_iterator I = inst_begin(F); I != inst_end(F); ++I) {
				if (I->hasName()) {
					if (I->getName() == "d") {
						A = &*I;
					}

					if (I->getName() == "call") {
						B = &*I;
					}
				}
			}

			if (A == nullptr) {
				report_fatal_error("@Test must have instruction A!");
			}

			if (B == nullptr) {
				report_fatal_error("@Test must have instruction B!");
			}
		}

		void expectDependency(int expectedDepType) {
			static char ID;
			class DataDependencyTestPass : public FunctionPass {
				public: 
					DataDependencyTestPass(bool expectedDepType, Instruction* A, Instruction* B) : FunctionPass(ID), ExpectedDepType(expectedDepType), A(A), B(B) {}

				  static int initialize() {
							PassInfo *PI = new PassInfo("Data Dependency Graph testing pass", "", &ID, nullptr, true, true);
        			PassRegistry::getPassRegistry()->registerPass(*PI, false);
        			initializeDataDependencyGraphPass(*PassRegistry::getPassRegistry());
        			return 0;
					}

					void getAnalysisUsage(AnalysisUsage &AU)	const override {
							AU.setPreservesAll();
							AU.addRequired<pdg::DataDependencyGraph>();
					}

					bool runOnFunction(Function &F) override {
						if (!F.hasName() || F.getName() != "test")
		          return false;

						pdg::DataDependencyGraph *ddg = &getAnalysis<pdg::DataDependencyGraph>(F);
						REQUIRE(ExpectedDepType == ddg->getDependencyType(A, B));
						return false;
					}	

					int ExpectedDepType;
					Instruction *A, *B;
			};

			// static int initialize = DataDependencyTestPass::initialize();
	    // (void)initialize;

			DataDependencyTestPass *P = new DataDependencyTestPass(expectedDepType, A, B);
			legacy::PassManager PM;
			PM.add(P);
			PM.run(*M);
		}

		LLVMContext C;
		std::unique_ptr<Module> M;
		Instruction *A, *B;
	};
}

TEST_CASE("Data Dependency Graph test", "Test Def-Use Dependency Build") {
  StringRef ModuleString = R"(
		target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
    target triple = "x86_64-unknown-linux-gnu"
    
    @.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
    
    ; Function Attrs: noinline nounwind optnone uwtable
    define i32 @test() #0 {
  entry:
  	%a = alloca i32, align 4
  	%b = alloca i32*, align 8
  	%c = alloca i32, align 4
  	store i32 0, i32* %a, align 4
  	store i32* %a, i32** %b, align 8
  	%0 = load i32, i32* %a, align 4
  	%add = add nsw i32 %0, 3
  	store i32 %add, i32* %c, align 4
  	%1 = load i32*, i32** %b, align 8
 	  %d = load i32, i32* %1, align 4
  	%call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %d)
  	ret i32 0
  }
  
 	declare i32 @printf(i8*, ...) #1
)";
	
	DataDependencyGraphTest DT;
	DT.parseAssembly(ModuleString);
	REQUIRE(DT.M == nullptr);
	DT.expectDependency(pdg::DATA_DEF_USE);
	DT.expectDependency(pdg::DATA_RAW);
	DT.expectDependency(pdg::DATA_ALIAS);
}
