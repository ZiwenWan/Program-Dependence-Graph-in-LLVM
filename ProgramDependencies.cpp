#include "ProgramDependencies.h"

using namespace llvm;
using namespace std;

static void
constructFuncMap(Module &M,
                 std::map<const Function *, FunctionWrapper *> &funcMap) {
    for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
        Function *f = dyn_cast<Function>(F);
        if (funcMap.find(f) == funcMap.end()) // if not in funcMap yet, insert
        {
            FunctionWrapper *fw = new FunctionWrapper(f);
            funcMap[f] = fw;
        }
    }
}

std::map<const Function *, FunctionWrapper *> funcMap;
std::map<const CallInst *, CallWrapper *> callMap =
        std::map<const CallInst *, CallWrapper *>();

std::set<InstructionWrapper *> instnodes;
std::set<InstructionWrapper *> globalList;
std::map<const llvm::Instruction *, InstructionWrapper *> instMap;
std::map<const llvm::Function *, std::set<InstructionWrapper *>>
        funcInstWList;

static IRBuilder<> Builder();

char ProgramDependencyGraph::ID = 0;
// std::map<const llvm::BasicBlock *,BasicBlockWrapper *>
// BasicBlockWrapper::bbMap;
//AliasAnalysis *ProgramDependencyGraph::Global_AA = nullptr;

// std::map<const llvm::Function *,FunctionWrapper *> FunctionWrapper::funcMap;
// std::map<const llvm::CallInst *,CallWrapper * > CallWrapper::callMap;

void ProgramDependencyGraph::connectAllPossibleFunctions(
        InstructionWrapper *CInstW, FunctionType *funcTy) {
    std::map<const llvm::Function *, FunctionWrapper *>::iterator FI =
            funcMap.begin();
    std::map<const llvm::Function *, FunctionWrapper *>::iterator FE =
            funcMap.end();

    for (; FI != FE; ++FI) {
        if ((*FI).first->getFunctionType() == funcTy &&
            (*FI).first->getName() != "main") {
            errs() << (*FI).first->getName() << " function pointer! \n";
            // TODO:
            // color a ret node in callee(func ptr)randomly as long as we can combine
            // them together with caller
        }
        for (list<ArgumentWrapper *>::iterator
                     argI = funcMap[(*FI).first]->getArgWList().begin(),
                     argE = funcMap[(*FI).first]->getArgWList().end();
             argI != argE; ++argI) {

            InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
            InstructionWrapper *formal_outW = *(*argI)->getTree(FORMAL_OUT_TREE).begin();
            errs() << "Validating formal nodes:" << "\n";
            errs() << *instnodes.find(formal_inW) << "\n";
            errs() << *instnodes.find(formal_outW) << "\n";
            if(instnodes.find(formal_inW) == instnodes.end() || instnodes.find(formal_outW) == instnodes.end()) {
                continue;
            }
            // connect Function's EntryNode with formal in/out tree roots
            PDG->addDependency(funcMap[(*FI).first]->getEntry(), *instnodes.find(formal_inW), PARAMETER);
            PDG->addDependency(funcMap[(*FI).first]->getEntry(), *instnodes.find(formal_outW), PARAMETER);
        }
    }
}

void ProgramDependencyGraph::drawFormalParameterTree(Function *func,
                                                     TreeType treeTy) {
    for (list<ArgumentWrapper *>::iterator
                 argI = funcMap[func]->getArgWList().begin(),
                 argE = funcMap[func]->getArgWList().end();
         argI != argE; ++argI) {
        for (tree<InstructionWrapper *>::iterator
                     TI = (*argI)->getTree(treeTy).begin(),
                     TE = (*argI)->getTree(treeTy).end();
             TI != TE; ++TI) {
            for (int i = 0; i < TI.number_of_children(); i++) {
                InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

                if (nullptr == *instnodes.find(*TI))
                    errs() << "DEBUG LINE 84 InstW NULL\n";
                if (nullptr == *instnodes.find(childW))
                    errs() << "DEBUG LINE 85 InstW NULL\n";
                if (*instnodes.find(*TI) == *instnodes.find(childW)) {
                    errs() << "Same Instruction Wrapper, skip...\n";
                    continue;
                }

                errs() << "DEBUGGING ---- \n";
                errs() << (*instnodes.find(*TI) == *instnodes.find(childW)) << "\n";
                PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW), PARAMETER);
            }
        } // end for tree
    }   // end for list
}

void ProgramDependencyGraph::drawActualParameterTree(CallInst *CI,
                                                     TreeType treeTy) {
    for (list<ArgumentWrapper *>::iterator
                 argI = callMap[CI]->getArgWList().begin(),
                 argE = callMap[CI]->getArgWList().end();
         argI != argE; ++argI) {

        for (tree<InstructionWrapper *>::iterator
                     TI = (*argI)->getTree(treeTy).begin(),
                     TE = (*argI)->getTree(treeTy).end();
             TI != TE; ++TI) {
            for (int i = 0; i < TI.number_of_children(); i++) {
                InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

                if (nullptr == *instnodes.find(*TI))
                    errs() << "DEBUG LINE 103 InstW NULL\n";
                if (nullptr == *instnodes.find(childW))
                    errs() << "DEBUG LINE 104 InstW NULL\n";

                if (*instnodes.find(*TI) == *instnodes.find(childW)) {
                    errs() << "Same Instruction Wrapper, skip...\n";
                    continue;
                }

                errs() << "DEBUGGING ---- \n";
                errs() << (*instnodes.find(*TI) == *instnodes.find(childW)) << "\n";
                PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW), PARAMETER);
            }
        } // end for tree
    }   // end for list
}

int ProgramDependencyGraph::connectCallerAndCallee(InstructionWrapper *InstW,
                                                   llvm::Function *callee) {

    if (InstW == nullptr || callee == nullptr) {
        return 1;
    }

    // callInst in caller --> Entry Node in callee
    PDG->addDependency(InstW, funcMap[callee]->getEntry(), CONTROL);

    // ReturnInst in callee --> CallInst in caller
    for (std::list<Instruction *>::iterator
                 RI = funcMap[callee]->getReturnInstList().begin(),
                 RE = funcMap[callee]->getReturnInstList().end();
         RI != RE; ++RI) {

        for (std::set<InstructionWrapper *>::iterator nodeIt = instnodes.begin();
             nodeIt != instnodes.end(); ++nodeIt) {

            if ((*nodeIt)->getInstruction() == *RI) {
                if (nullptr !=
                    dyn_cast<ReturnInst>((*nodeIt)->getInstruction())->getReturnValue())
                    PDG->addDependency(*instnodes.find(*nodeIt), InstW, DATA_GENERAL);
                    // TODO: indirect call, function name??
                else
                    ; // errs() << "void ReturnInst: " << *(*nodeIt)->getInstruction()
                // << " Function: " << callee_func->getName();
            }
        }
    }

    // TODO: consider all kinds of connecting cases
    // connect caller InstW with ACTUAL IN/OUT parameter trees
    CallInst *CI = dyn_cast<CallInst>(InstW->getInstruction());

    for (std::list<ArgumentWrapper *>::iterator
                 argI = callMap[CI]->getArgWList().begin(),
                 argE = callMap[CI]->getArgWList().end();
         argI != argE; ++argI) {

        InstructionWrapper *actual_inW = *(*argI)->getTree(ACTUAL_IN_TREE).begin();
        InstructionWrapper *actual_outW = *(*argI)->getTree(ACTUAL_OUT_TREE).begin();

        if (nullptr == *instnodes.find(actual_inW))
            errs() << "DEBUG LINE 168 InstW NULL\n";
        if (nullptr == *instnodes.find(actual_outW))
            errs() << "DEBUG LINE 169 InstW NULL\n";

        if (InstW == *instnodes.find(actual_inW) || InstW == *instnodes.find(actual_outW)) {
            errs() << "DEBUG LINE 182 InstW duplicate";
            continue;
        }

        PDG->addDependency(InstW, *instnodes.find(actual_inW), PARAMETER);
        PDG->addDependency(InstW, *instnodes.find(actual_outW), PARAMETER);
    }

    // old way, process four trees at the same time, remove soon
    list<ArgumentWrapper *>::iterator formal_argI;
    formal_argI = funcMap[callee]->getArgWList().begin();

    list<ArgumentWrapper *>::iterator formal_argE;
    formal_argE = funcMap[callee]->getArgWList().end();

    list<ArgumentWrapper *>::iterator actual_argI;
    actual_argI = callMap[CI]->getArgWList().begin();

    list<ArgumentWrapper *>::iterator actual_argE;
    callMap[CI]->getArgWList().end();

    // increase formal/actual tree iterator simutaneously
    for (; formal_argI != formal_argE; ++formal_argI, ++actual_argI) {

        // intra-connection between ACTUAL/FORMAL IN/OUT trees
        for (tree<InstructionWrapper *>::iterator
                     actual_in_TI = (*actual_argI)->getTree(ACTUAL_IN_TREE).begin(),
                     actual_in_TE = (*actual_argI)->getTree(ACTUAL_IN_TREE).end(),
                     formal_in_TI =
                     (*formal_argI)->getTree(FORMAL_IN_TREE).begin(), // TI2
                     formal_out_TI =
                     (*formal_argI)->getTree(FORMAL_OUT_TREE).begin(), // TI3
                     actual_out_TI =
                     (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin(); // TI4
             actual_in_TI != actual_in_TE;
             ++actual_in_TI, ++formal_in_TI, ++formal_out_TI, ++actual_out_TI) {

            // connect trees: antual-in --> formal-in, formal-out --> actual-out
            PDG->addDependency(*instnodes.find(*actual_in_TI),
                               *instnodes.find(*formal_in_TI), PARAMETER);
            PDG->addDependency(*instnodes.find(*formal_out_TI),
                               *instnodes.find(*actual_out_TI), PARAMETER);

        } // end for(tree...) intra-connection between actual/formal

        // TODO: why removing this debugging infor will cause segmentation fault?

        // 3. ACTUAL_OUT --> LoadInsts in #Caller# function
        for (tree<InstructionWrapper *>::iterator
                     actual_out_TI = (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin(),
                     actual_out_TE = (*actual_argI)->getTree(ACTUAL_OUT_TREE).end();
             actual_out_TI != actual_out_TE; ++actual_out_TI) {

            // must handle nullptr case first
            if (nullptr == (*actual_out_TI)->getFieldType()) {
                errs() << "DEBUG ACTUAL_OUT->LoadInst: actual_out_TI->getFieldType() "
                        "is empty!\n";
                break;
            }

            if (nullptr != (*actual_out_TI)->getFieldType()) {

                std::list<LoadInst *>::iterator LI =
                        funcMap[InstW->getFunction()]->getLoadInstList().begin();
                std::list<LoadInst *>::iterator LE =
                        funcMap[InstW->getFunction()]->getLoadInstList().end();

                for (; LI != LE; ++LI) {
                    if ((*actual_out_TI)->getFieldType() == (*LI)->getType()) {
                        for (std::set<InstructionWrapper *>::iterator nodeIt =
                                instnodes.begin();
                             nodeIt != instnodes.end(); ++nodeIt) {

                            if ((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*LI))
                                PDG->addDependency(*instnodes.find(*actual_out_TI),
                                                   *instnodes.find(*nodeIt), DATA_GENERAL);
                        }
                    }
                } // end for(; LI != LE; ++LI)
            }   // end if(nullptr != (*TI3)->...)
        }     // end for(tree actual_out_TI = getTree FORMAL_OUT_TREE)

    } // end for argI iteration

    return 0;
}

void ProgramDependencyGraph::connectFunctionAndFormalTrees(
        llvm::Function *callee) {

    //  errs() << "DEBUG 152: In connectFunctionAndFormalTrees,
    //  callee->getName() : " << callee->getName() << "\n";

    for (list<ArgumentWrapper *>::iterator
                 argI = funcMap[callee]->getArgWList().begin(),
                 argE = funcMap[callee]->getArgWList().end();
         argI != argE; ++argI) {

        //    errs() << "arg : " << *(*argI)->getArg() << " " << (*argI)->getArg()
        //    << "\n";

        InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
        InstructionWrapper *formal_outW =
                *(*argI)->getTree(FORMAL_OUT_TREE).begin();

        //   errs() << "formal_in_tree.size = " <<
        //   (*argI)->getTree(FORMAL_IN_TREE).size() << "\n";
        // errs() << "formal_out_tree.size = " <<
        // (*argI)->getTree(FORMAL_OUT_TREE).size() << "\n";

        // connect Function's EntryNode with formal in/out tree roots
        PDG->addDependency(funcMap[callee]->getEntry(), *instnodes.find(formal_inW),
                           PARAMETER);
        PDG->addDependency(funcMap[callee]->getEntry(),
                           *instnodes.find(formal_outW), PARAMETER);

        // TODO: connect corresponding instructions with internal formal tree
        // nodes

        // two things: (1) formal-in --> callee's Store; (2) callee's Load -->
        // formal-out
        for (tree<InstructionWrapper *>::iterator
                     formal_in_TI = (*argI)->getTree(FORMAL_IN_TREE).begin(),
                     formal_in_TE = (*argI)->getTree(FORMAL_IN_TREE).end(),
                     formal_out_TI = (*argI)->getTree(FORMAL_OUT_TREE).begin();
             formal_in_TI != formal_in_TE; ++formal_in_TI, ++formal_out_TI) {

            // connect formal-in and formal-out nodes formal-in --> formal-out
            PDG->addDependency(*instnodes.find(*formal_in_TI),
                               *instnodes.find(*formal_out_TI), PARAMETER);

            // must handle nullptr case first
            if (nullptr == (*formal_in_TI)->getFieldType()) {
                errs() << "connectFunctionAndFormalTrees: "
                        "formal_in_TI->getFieldType() == nullptr !\n";
                break;
            }

            // connect formal-in-tree type nodes with storeinst in call_func,
            // approximation used here
            if (nullptr != (*formal_in_TI)->getFieldType()) {

                //	errs() << "formal_in_TI = " << *(*formal_in_TI)->getFieldType()
                //<< " " << (*formal_in_TI)->getFieldType() << "\n";

                std::list<StoreInst *>::iterator SI =
                        funcMap[callee]->getStoreInstList().begin();
                std::list<StoreInst *>::iterator SE =
                        funcMap[callee]->getStoreInstList().end();

                for (; SI != SE; ++SI) {
                    if ((*formal_in_TI)->getFieldType() ==
                        (*SI)->getValueOperand()->getType()) {

                        for (std::set<InstructionWrapper *>::iterator nodeIt =
                                instnodes.begin();
                             nodeIt != instnodes.end(); ++nodeIt) {

                            if ((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*SI))
                                PDG->addDependency(*instnodes.find(*formal_in_TI),
                                                   *instnodes.find(*nodeIt), DATA_GENERAL);
                        }
                    }
                } // end for(;SI != SE; ++SI)
            }   // end if nullptr == (*formal_in_TI)->getFieldType()

            // errs() << "DEBUG 207: (*formal_out_TI)->getFieldType() = " <<
            // *(*formal_out_TI)->getFieldType() << " " <<
            // (*formal_out_TI)->getFieldType() << "\n";
            // 2. Callee's LoadInsts --> FORMAL_OUT in Callee function
            // must handle nullptr case first
            if (nullptr == (*formal_out_TI)->getFieldType()) {
                errs() << "connectFunctionAndFormalTrees: LoadInst->FORMAL_OUT: "
                        "formal_out_TI->getFieldType() == nullptr!\n";
                break;
            }

            //      errs() << "DEBUG 214\n";
            if (nullptr != (*formal_out_TI)->getFieldType()) {

                std::list<LoadInst *>::iterator LI =
                        funcMap[callee]->getLoadInstList().begin();
                std::list<LoadInst *>::iterator LE =
                        funcMap[callee]->getLoadInstList().end();

                for (; LI != LE; ++LI) {
                    //	  errs() << "LI " << *(*LI) << "\n";
                    // errs() << "LI ptr type" <<
                    // *(*LI)->getPointerOperand()->getType()->getContainedType(0) <<
                    // "\n";

                    if ((*formal_out_TI)->getFieldType() ==
                        (*LI)->getPointerOperand()->getType()->getContainedType(0)) {
                        for (std::set<InstructionWrapper *>::iterator nodeIt =
                                instnodes.begin();
                             nodeIt != instnodes.end(); ++nodeIt) {

                            if ((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*LI)) {
                                PDG->addDependency(*instnodes.find(*nodeIt),
                                                   *instnodes.find(*formal_out_TI),
                                                   DATA_GENERAL);
                            }
                        }
                    }
                } // end for(; LI != LE; ++LI)
            }   // end if(nullptr != (*formal_out_TI)->...)

            //      errs() << "DEBUG 236\n";
        } // end for (tree formal_in_TI...)

        //    errs() << "DEBUG 240 arg: END\n";

    } // end for arg iteration...

    //  errs() << "DEBUG 243 connectFunctionAndFormalTrees END\n";
}

bool ProgramDependencyGraph::runOnModule(Module &M) {

    //Global_AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();

    errs() << "ProgramDependencyGraph::runOnModule" << '\n';

    constructFuncMap(M, funcMap);

    errs() << "funcMap size = " << funcMap.size() << '\n';

    for (auto &func : funcMap) {
        errs() << func.first->getName() << "\n";
    }
    // Get the Module's list of global variable and function identifiers

    errs() << "======Global List: ======\n";

    //    M.getGlobalList().dump();
    for (llvm::Module::global_iterator globalIt = M.global_begin();
         globalIt != M.global_end(); ++globalIt) {

        InstructionWrapper *globalW =
                new InstructionWrapper(nullptr, nullptr, &(*globalIt), GLOBAL_VALUE);
        instnodes.insert(globalW);
        globalList.insert(globalW);

        // find all global pointer values and insert them into a list
        if (globalW->getValue()->getType()->getContainedType(0)->isPointerTy()) {
            // errs() << " Pointer global value found! : " << *(globalW->getValue())
            // << "\n";
            gp_list.push_back(globalW);
        }
    }

    int funcs = 0;

    // process a module function by function
    for (Module::iterator FF = M.begin(), E = M.end(); FF != E; ++FF) {
        errs() << "Module Size: " << M.size() << "\n";

        Function *F = dyn_cast<Function>(FF);

        if ((*F).isDeclaration()) {
            errs() << (*F).getName() << " is defined outside!"
                   << "\n";
            continue;
        }

        funcs++; // label this author-defined function

        errs() << "PDG " << 1.0 * funcs / M.getFunctionList().size() * 100
               << "% completed\n";

        constructInstMap(*F);

        errs() << "DEBUGGING -----------\n\n";
        for (auto *instW : instnodes) {
            llvm::Instruction *tmp = instW->getInstruction();
            if (tmp != nullptr) {
                errs() << *tmp << "\n";
            }
        }
        errs() << "DEBUGGING -----------\n\n";
        // find all Load/Store instructions for each F, insert to F's
        // storeInstList and loadInstList
        for (inst_iterator I = inst_begin(F), IE = inst_end(F); I != IE; ++I) {

            Instruction *pInstruction = dyn_cast<Instruction>(&*I);

            if (isa<StoreInst>(pInstruction)) {
                StoreInst *SI = dyn_cast<StoreInst>(pInstruction);
                funcMap[&*F]->getStoreInstList().push_back(SI);

                funcMap[&*F]->getPtrSet().insert(SI->getPointerOperand());

                if (SI->getValueOperand()->getType()->isPointerTy()) {
                    funcMap[&*F]->getPtrSet().insert(SI->getValueOperand());
                }
            }
            if (isa<LoadInst>(pInstruction)) {
                LoadInst *LI = dyn_cast<LoadInst>(pInstruction);
                funcMap[&*F]->getLoadInstList().push_back(LI);
                funcMap[&*F]->getPtrSet().insert(LI->getPointerOperand());
            }

            if (isa<ReturnInst>(pInstruction))
                funcMap[&*F]->getReturnInstList().push_back(pInstruction);

            if (isa<CallInst>(pInstruction))
                funcMap[&*F]->getCallInstList().push_back(
                        dyn_cast<CallInst>(pInstruction));
        }

        // print PtrSet only
        ControlDependencyGraph &cdgGraph = getAnalysis<ControlDependencyGraph>(*F);
        DataDependencyGraph &ddgGraph = getAnalysis<DataDependencyGraph>(*F);
        // set Entries for Function, set up links between dummy entry nodes and their func*

        for (std::set<InstructionWrapper *>::iterator nodeIt =
                funcInstWList[&*F].begin();
             nodeIt != funcInstWList[&*F].end(); nodeIt++) {

            InstructionWrapper *InstW = *nodeIt;
            if (InstW->getType() == ENTRY) {
                errs() << "Find Entry"
                       << "\n";
                std::map<const llvm::Function *, FunctionWrapper *>::const_iterator FI =
                        funcMap.find(InstW->getFunction());

                if (FI != funcMap.end()) {
                    //   errs() << "find successful!" << "\n";
                    funcMap[InstW->getFunction()]->setEntry(InstW);
                }
            }
        } // end for set Entries...

        clock_t begin2 = clock();

        // the iteration should be done for the InstructionWrapperList, not original F
        for (std::set<InstructionWrapper *>::iterator nodeIt =
                funcInstWList[&*F].begin();
             nodeIt != funcInstWList[&*F].end(); ++nodeIt) {

            InstructionWrapper *InstW = *nodeIt;
            llvm::Instruction *pInstruction = InstW->getInstruction();

            // second iteration on nodes to add both control and data Dependency
            for (std::set<InstructionWrapper *>::iterator nodeIt2 =
                    funcInstWList[&*F].begin();
                 nodeIt2 != funcInstWList[&*F].end(); ++nodeIt2) {
                InstructionWrapper *InstW2 = *nodeIt2;

                if (InstW == InstW2) {
                    continue;
                }

                // process all globals see whether dependency exists
                if (InstW2->getType() == INST &&
                    isa<LoadInst>(InstW2->getInstruction())) {

                    LoadInst *LI2 = dyn_cast<LoadInst>(InstW2->getInstruction());

                    for (std::set<InstructionWrapper *>::const_iterator gi =
                            globalList.begin();
                         gi != globalList.end(); ++gi) {
                        //		errs() << "global v = " << *(*gi)->getValue() <<
                        //"\n";
                        if (LI2->getPointerOperand() == (*gi)->getValue()) {
                            //	  errs() << *(*gi)->getValue() << " used in " << *LI2 <<
                            //"\n";
                            PDG->addDependency(*gi, InstW2, GLOBAL_VALUE);
                        }
                    } // end searching globalList
                }   // end procesing load for global

                if (InstW->getType() == INST) {
                    if (ddgGraph.DDG->depends(InstW, InstW2)) {
                        // only StoreInst->LoadInst edge can be annotated
                        if (InstW2->getType() == INST &&
                            isa<StoreInst>(InstW->getInstruction()) &&
                            isa<LoadInst>(InstW2->getInstruction())) {
                            PDG->addDependency(InstW, InstW2, DATA_RAW);
                        } else
                            PDG->addDependency(InstW, InstW2, DATA_DEF_USE);
                    }

                    if (nullptr != InstW2->getInstruction()) {
                        if (cdgGraph.CDG->depends(InstW, InstW2)) {
                            PDG->addDependency(InstW, InstW2, CONTROL);
                        }
                    }
                } // end if(InstW->getType()== INST)

                if (InstW->getType() == ENTRY) {
                    if (nullptr != InstW2->getInstruction() &&
                        cdgGraph.CDG->depends(InstW, InstW2)) {
                        PDG->addDependency(InstW, InstW2, CONTROL);
                    }
                }

                // process call nodes, one call node will only be touched
                // once(!InstW->getAccess)
                if (pInstruction != nullptr && InstW->getType() == INST &&
                    isa<CallInst>(pInstruction) && !InstW->getAccess()) {
                    InstructionWrapper *CallInstW = InstW;
                    CallInst *CI = dyn_cast<CallInst>(pInstruction);
                    Function *callee = CI->getCalledFunction();

                    if (callee == nullptr) {
                        errs() << "call_func = null: " << *CI << "\n";

                        Type *t = CI->getCalledValue()->getType();
                        errs() << "indirect call, called Type t = " << *t << "\n";

                        FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
                        errs() << "after cast<FunctionType>, ft = " << *funcTy << "\n";
                        // here processing the indirect function. For now, this is not connected for simplicity(debug purpose).
                        //connectAllPossibleFunctions(InstW, funcTy);
                        continue;
                    }

                    // TODO: isIntrinsic or not? Consider intrinsics as common
                    // instructions for now, continue directly
                    // determine if the function start with llvm::
                    if (callee->isIntrinsic() || callee->isDeclaration()) {
                        // if it is a var_annotation, save the sensitive value by the way
                        if (callee->getIntrinsicID() == Intrinsic::var_annotation) {
                            Value *v = CI->getArgOperand(0);
                            errs() << "Intrinsic var_annotation: " << *v << "\n";

                            if (isa<BitCastInst>(v)) {
                                Instruction *tempI = dyn_cast<Instruction>(v);
                                errs() << "******** BitInst opcode: " << *tempI << "BitCast \n";

                                for (llvm::Use &U : tempI->operands()) {
                                    Value *v_for_cast = U.get();
                                    //sensitive_values.push_back(v_for_cast);
                                }
                            }
//              else
//                sensitive_values.push_back(v);
                        }
                        continue;
                    }

                    // TODO: tail call processing
                    if (CI->isTailCall()) {
                        continue;
                    }

                    // special cases done, common function
                    CallWrapper *callW = new CallWrapper(CI);
                    callMap[CI] = callW;

                    if (!callee->arg_empty()) {

                        if (true != funcMap[callee]->hasTrees()) {
                            //	  errs() << "DEBUG 456 New call for tree construction: "
                            //<< *InstW->getInstruction() << "\n";  build formal trees in memory
                            buildFormalParameterTrees(callee);
                            // add tree edges in PDG
                            drawFormalParameterTree(callee, FORMAL_IN_TREE);
                            drawFormalParameterTree(callee, FORMAL_OUT_TREE);
                            connectFunctionAndFormalTrees(callee);
                        }
                        // TODO: We temporarily use this logic since we process F one by
                        // one, use a better logic later  if callee has parameter trees
                        // already, just build actual trees
                        buildActualParameterTrees(CI);
                        drawActualParameterTree(CI, ACTUAL_IN_TREE);
                        drawActualParameterTree(CI, ACTUAL_OUT_TREE);
                    } // end if !callee

                    if (0 == connectCallerAndCallee(InstW, callee)) {
                        InstW->setAccess(true);
                    }
                } // end callnode

            } // end second iteration for PDG->addDependency...
        }   // end the iteration for finding CallInst

    } // end for(Module...

    errs() << "\n\n PDG construction completed! ^_^\n\n";
    errs() << "funcs = " << funcs << "\n";
    errs() << "+++++++++++++++++++++++++++++++++++++++++++++\n";

    // clean up
    funcMap.clear();
    callMap.clear();
    instnodes.clear();
    globalList.clear();
    instMap.clear();
    funcInstWList.clear();
    return false;
}

void ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<ControlDependencyGraph>();
    AU.addRequired<DataDependencyGraph>();
    AU.setPreservesAll();
}

void ProgramDependencyGraph::print(llvm::raw_ostream &OS, const llvm::Module *) const {
    PDG->print(OS, (getPassName().data()));
}

ProgramDependencyGraph *CreateProgramDependencyGraphPass() {
    return new ProgramDependencyGraph();
}

static RegisterPass<ProgramDependencyGraph>
        PDG("pdg", "Program Dependency Graph Construction", false, true);
