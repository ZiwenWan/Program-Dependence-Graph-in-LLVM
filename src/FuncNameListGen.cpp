#ifndef LLVM_TEST_H
#define LLVM_TEST_H

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/GlobalVariable.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"

#include <vector>
#include <set>
#include <string.h>
#include <map>
#include <fstream>
#include <sstream>

namespace {
    using namespace llvm;

    class llvmTest : public ModulePass {
    public:
        static char ID;

        llvmTest() : ModulePass(ID) {
        }

        StoreInst* getArgStore(Argument* arg) {
            for (auto UI = arg->user_begin(); UI != arg->user_end(); ++UI) {
                if (StoreInst *st = dyn_cast<StoreInst>(*UI)) {
                    return st;
                }
            }
            return nullptr;
        }

        AllocaInst* getArgAllocaInst(Argument *arg) {
            StoreInst* st = getArgStore(arg);
            if (st == nullptr) {
                return nullptr;
            } 

            if (AllocaInst* ai = dyn_cast<AllocaInst>(st->getPointerOperand())) {
                return ai;
            }

            return nullptr;
        }

        std::string getDIFieldName(DIType *ty)
        {
            if (ty == nullptr)
                return "void";
            switch (ty->getTag())
            {
                case dwarf::DW_TAG_member:
                    {
                        return ty->getName().str();
                    }
                case dwarf::DW_TAG_array_type:
                    {
                        ty = dyn_cast<DICompositeType>(ty)->getBaseType().resolve();
                        return "arr_" + ty->getName().str();
                    }
                case dwarf::DW_TAG_pointer_type:
                    {
                        std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
                        return s;
                    }
                case dwarf::DW_TAG_subroutine_type:
                    return "func ptr";
                case dwarf::DW_TAG_const_type:
                    {
                        std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
                        return s;
                    }
                default:
                    {
                        if (!ty->getName().str().empty())
                            return ty->getName().str();
                        return "no name";
                    }
            }
        }

        DIType *getBaseDIType(DIType *Ty) {
            if (Ty == nullptr)
                return nullptr;

            if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
                    Ty->getTag() == dwarf::DW_TAG_member ||
                    Ty->getTag() == dwarf::DW_TAG_typedef)
            {
                DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
                if (!baseTy)
                {
                    errs() << "Type : NULL - Nothing more to do\n";
                    return nullptr;
                }
                return baseTy;
            }
            return Ty;
        }

        DIType *getLowestDIType(DIType *Ty) 
        {
            if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
                    Ty->getTag() == dwarf::DW_TAG_member ||
                    Ty->getTag() == dwarf::DW_TAG_typedef ||
                    Ty->getTag() == dwarf::DW_TAG_const_type)
            {
                DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
                if (!baseTy)
                {
                    errs() << "Type : NULL - Nothing more to do\n";
                    return NULL;
                }

                //Skip all the DINodes with DW_TAG_typedef tag
                while ((baseTy->getTag() == dwarf::DW_TAG_typedef ||
                            baseTy->getTag() == dwarf::DW_TAG_const_type ||
                            baseTy->getTag() == dwarf::DW_TAG_pointer_type))
                {
                    if (DIType *temp = dyn_cast<DIDerivedType>(baseTy)->getBaseType().resolve())
                        return temp;
                    else
                        break;
                }
                return baseTy;
            }
            return Ty;
        }

        bool runOnModule(Module &M)
        {
            // unsigned instCount = 0;
            // for (auto &F : M)
            // {
            //     for (BasicBlock &bb : F)
            //         instCount += std::distance(bb.begin(), bb.end());
            // }
            // errs() << "Number of inst: " << instCount << "\n";

            for (auto &F : M)
            {
                for (const Use &use : F.uses()) {
                    const User *user = use.getUser();
                    if (isa<BlockAddress>(user))
                        continue;
                    const auto *Call = dyn_cast<CallInst>(user);
                    if (!Call)
                        continue;    
                    
                    if (Call->getCalledFunction())
                        continue;
                    /* if (!Call->isCallee(&use)) { */
                    errs() << "!  " << *Call << " - " << F.getName() << "\n";
                    /* } */
                }
            }
        

            std::ofstream imported_func("imported_func.txt");
            std::ofstream defined_func("defined_func.txt");
            std::ofstream static_funcptr("static_funcptr.txt");
            std::ofstream static_func("static_func.txt");
            std::ofstream lock_func("lock_func.txt");
            std::ofstream driver_globalvars("driver_globalvars.txt");

            lock_func << "spin_lock\n";
            lock_func << "spin_lock_irq\n";
            lock_func << "mutex_lock\n";

            // print driver globals
            for (auto &globalVar : M.globals())
            {
                if (!globalVar.isConstant())
                    driver_globalvars << globalVar.getName().str() << "\n";
            }

            driver_globalvars.close();

            for (auto &F : M) {
                auto funcName = F.getName().str();

                if (F.isIntrinsic())
                    continue;

                if (F.isDeclaration()) {
                    imported_func << funcName << "\n";

                    if (funcName.find("lock") != std::string::npos) 
                        lock_func << funcName << "\n";

                    continue;
                }

                defined_func << funcName << "\n";
            }

            // std::set<std::string> seenFuncName;
            // for (auto &F : M)
            // {
            //   for (auto &BB : F)
            //   {
            //     for (auto &inst : BB)
            //     {
            //       if (CallInst *ci = dyn_cast<CallInst>(&inst))
            //       {
            //         auto calledFunc = ci->getCalledFunction();
            //         if (calledFunc && calledFunc->isDeclaration() && !calledFunc->isIntrinsic())
            //         {
            //           auto funcName = calledFunc->getName().str();
            //           if (seenFuncName.find(funcName) != seenFuncName.end())
            //             continue;
            //           if (funcName.find("lock") != std::string::npos)
            //           {
            //             lock_func << funcName << "\n";
            //             continue;
            //           }
            //           imported_func << funcName << "\n";
            //           imported_func << F.getName().str() << "\n";
            //           seenFuncName.insert(funcName);
            //           seenFuncName.insert(F.getName().str());
            //         }
            //       }
            //     }
            //   }
            // }

            imported_func.close();
            defined_func.close();

            SmallVector<DIGlobalVariableExpression*, 4> sv;
            for (auto &Global : M.getGlobalList()) {
                DIGlobalVariable* gv = nullptr;
                Global.getDebugInfo(sv); 
                /* errs() << Global.getName() << " " << sv.size() << "\n"; */
                for (auto d : sv) {
                    if (d->getVariable()->getName() == Global.getName()) {
                        gv = d->getVariable();  // get global variable from global expression
                    }
                }

                if (gv == nullptr)
                    continue;

                auto gvDItype = gv->getType().resolve();
                if (getLowestDIType(gvDItype)->getTag() != dwarf::DW_TAG_structure_type) 
                    continue;

                const auto &typeArrRef = dyn_cast<DICompositeType>(getLowestDIType(gvDItype))->getElements();
                if (Global.getType()->isPointerTy()) {
                    auto pointerEle = Global.getType()->getPointerElementType();
                    /* errs() << typeArrRef.size() << " - " << pointerEle->getStructNumElements() << "\n"; */
                    if (pointerEle->isStructTy()) {
                        for (unsigned i = 0; i < pointerEle->getStructNumElements(); ++i) {
                            if (!Global.hasInitializer()) 
                                continue;
                            auto cons = Global.getInitializer()->getAggregateElement(i);
                            if (cons != nullptr) {
                                if (cons->hasName()) {
                                    static_funcptr << getDIFieldName(dyn_cast<DIType>(typeArrRef[i])) << "\n";
                                    static_func << cons->getName().str() << "\n";
                                }
                            }
                        }
                    }
                }
            }
            static_funcptr.close();
            static_func.close();

            /* aliasName = {"NoAlias", "MayAlias", "PartialAlias", "MustAlias"}; */
            /* for (Function &F : M)  { */
            /*     /1* if (F.getName() != "func2") *1/ */ 
            /*     /1*     continue; *1/ */
            /*     if (F.isDeclaration()) */
            /*         continue; */
            /*     AA = &getAnalysis<AAResultsWrapperPass>(F).getAAResults(); */
            /*     for (auto I1 = inst_begin(F); I1 != inst_end(F); ++I1) { */
            /*         for (auto GVI = M.global_begin(); GVI != M.global_end(); ++GVI) { */
            /*             if (Value *v1 = dyn_cast<Value>(&*I1)) { */
            /*                 if (GlobalVariable *v2 = dyn_cast<GlobalVariable>(&*GVI)) { */
            /*                     auto aaRes = AA->alias(v1, v2); */
            /*                     errs() << *v1 << " - " << *v2 << " " << aliasName[aaRes] << "\n"; */
            /*                 } */
            /*             } */
            /*         } */
            /*     } */
            /* } */

            return false;
        }

        // do main work here
        // bool runOnModule(Module &M)
        // {
        //     aliasName = {"NoAlias", "MayAlias", "PartialAlias", "MustAlias"};
        //     std::vector<Instruction*> storevec;
        //     std::vector<Instruction*> loadvec;
        //     /* std::vector<Instruction*> gepvec; */
        //     steensAA = &getAnalysis<CFLSteensAAWrapperPass>().getResult();
        //     andersAA = &getAnalysis<CFLAndersAAWrapperPass>().getResult();
        //     for (Function &F : M)
        //     {

        //         Function *func = &F;
        //         if (F.isDeclaration() or F.getName() == "main") {
        //             continue;
        //         }

        //         /* for (inst_iterator instI = inst_begin(func); instI != inst_end(func); ++instI) { */
        //         /*     if (CallInst *call_inst = dyn_cast<CallInst>(&*instI)) { */
        //         /*         errs() << call_inst->getCalledFunction()->getName() << "\n"; */
        //         /*         for (unsigned i = 0; i < call_inst->getNumArgOperands(); ++i) { */
        //         /*             //Type *ty = call_inst->getArgOperand(i)->getType(); */
        //         /*             //errs() << *(call_inst->getArgOperand(i)) << "\n"; */
        //         /*             if (PointerType *pt = dyn_cast<PointerType>((call_inst->getArgOperand(i))->getType())) { */
        //         /*                 errs() << *(call_inst->getArgOperand(i)) << "\n"; */
        //         /*                 errs() << pt->getElementType()->getTypeID() << "\n"; */
        //         /*                 /1* errs() << (call_inst->getArgOperand(i))->getType()-> << "\n"; *1/ */
        //         /*             } */
        //         /*             if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*(call_inst->getArgOperand(i)))) { */
        //         /*                 errs() << *gep << "\n"; */
        //         /*             } */
        //         /*         } */
        //         /*     } */
        //         /* } */

        //         if (func->getName() != "register_device") {
        //             continue;
        //         }

        //         //AA = &getAnalysis<AAResultsWrapperPass>(F).getAAResults();
        //         //steensAA = &getAnalysis<CFLSteensAAWrapperPass>(F).getResult();

        //         errs() << "[For function: " << F.getName() << "]\n";

        //         /* for (inst_iterator I = inst_begin(func), IE = inst_end(func); I != IE; ++I) */
        //         /* { */
        //         /*     if (CallInst* callInst = dyn_cast<CallInst>(&*I)) { */
        //         /*         for (auto arg_iter = callInst->arg_begin(); arg_iter != callInst->arg_end(); ++arg_iter) { */
        //         /*             if (Instruction* inst = dyn_cast<Instruction>(&*arg_iter)) { */
        //         /*                 errs() << *inst << "\n"; */
        //         /*             } */
        //         /*         } */
        //         /*     } */
        //         /* } */


        //         for (inst_iterator I = inst_begin(func), IE = inst_end(func); I != IE; ++I) {
        //             Instruction *pInstruction = &*I;

        //             if (isa<LoadInst>(pInstruction)) {
        //                 loadvec.push_back(pInstruction);
        //             }

        //             if (isa<StoreInst>(pInstruction)) {
        //                 storevec.push_back(pInstruction);
        //             }
        //             /* if (isa<GetElementPtrInst>(pInstruction)) { */
        //             /*     gepvec.push_back(pInstruction); */ 
        //             /* } */
        //         }

        //         /* for (inst_iterator I = inst_begin(func), IE = inst_end(func); I != IE; ++I) { */
        //         /*     Instruction *pInstruction = &*I; */ 
        //         /*     if (isa<DbgDeclareInst>(pInstruction)) { */
        //         /*         continue; */
        //         /*     } */
        //         /*     if (ImmutableCallSite CS = ImmutableCallSite(pInstruction)) { */
        //         /*         unsigned i = 0; */
        //         /*         errs() << CS.getCalledFunction()->getName() << "\n"; */
        //         /*         for (auto arg_iter = CS.arg_begin(); arg_iter != CS.arg_end(); ++arg_iter) { */
        //         /*             MemoryLocation arg_loc = MemoryLocation::getForArgument(CS, i, getAnalysis<TargetLibraryInfoWrapperPass>().getTLI()); */
        //         /*             for (Instruction* li : loadvec) { */
        //         /*                 MemoryLocation L_loc = MemoryLocation::get(li); */ 
        //         /*                 AliasResult AA_Result = steensAA->query(arg_loc, L_loc); */
        //         /*                 errs() << "For the " << i << "th argument" << "\n"; */
        //         /*                 errs() << *li << "\n"; */
        //         /*                 errs() << aliasName[AA_Result] << "\n"; */
        //         /*             } */
        //         /*             i++; */
        //         /*         } */
        //         /*     } */
        //         /* } */


        //         for (auto argI = func->arg_begin(); argI != func->arg_end(); ++argI)
        //         {
        //             StoreInst *argStore = getArgStore(&*argI);
        //             MemoryLocation argS_Loc = MemoryLocation::get(argStore);
        //             for (Instruction *si : storevec) {
        //                 /* if (si == argStore) { */
        //                 /*     continue; */
        //                 /* } */
        //                 MemoryLocation S_Loc = MemoryLocation::get(si);

        //                 for (Instruction *li : loadvec) {
        //                     MemoryLocation SS_Loc = MemoryLocation::get(li);
        //                     //AliasResult AA_Result = AA->alias(S_Loc, L_Loc);
        //                     AliasResult AA_Result = steensAA->alias(S_Loc, SS_Loc);
        //                     errs() << "------------------" << "\n";
        //                     errs() << *si << "\n";
        //                     errs() << *li << "\n";
        //                     errs() << aliasName[AA_Result] << "\n";
        //                 }
        //                 /* AliasResult AA_Result = AA->alias(argS_Loc, S_Loc); */
        //                 /* errs() << *argStore << "\n"; */
        //                 /* errs() << *si << "\n"; */
        //                 /* errs() << aliasName[AA_Result] << "\n"; */
        //             }
        //         }
        //     }
        //     return false;
        // }

        // Analysis Usage, specify PDG at this time
        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<AAResultsWrapperPass>();
            AU.addRequired<MemoryDependenceWrapperPass>();
            /* AU.addRequired<CFLSteensAAWrapperPass>(); */
            /* AU.addRequired<CFLAndersAAWrapperPass>(); */
            AU.addRequired<TargetLibraryInfoWrapperPass>();
            AU.setPreservesAll();
        }

        private:
        /* AliasAnalysis *AA; */
        /* CFLSteensAAResult *steensAA; */
        /* CFLAndersAAResult *andersAA; */
        std::vector<std::string> aliasName;
    };

    char llvmTest::ID = 0;
    static RegisterPass<llvmTest> llvmTest("gen-func-list", "LLVM TEST", false, true);
} // namespace
#endif
