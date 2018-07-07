#include "ProgramDependencies.h"

#define SUCCEED 0
#define NULLPTR 1
#define RECURSIVE_TYPE 2

using namespace llvm;
using namespace std;

static IRBuilder<> Builder();

char pdg::ProgramDependencyGraph::ID = 0;

static std::set<Type*> recursive_types;
static std::set<Type*> unseparated_types;

tree<pdg::InstructionWrapper*>::iterator pdg::getInstInsertLoc(pdg::ArgumentWrapper *argW, TypeWrapper *tyW, TreeType treeTy) {
    tree<pdg::InstructionWrapper*>::iterator insert_loc;
    for(tree<pdg::InstructionWrapper*>::iterator treeIt = argW->getTree(treeTy).begin(),
                treeE = argW->getTree(treeTy).end(); treeIt != treeE; ++treeIt){
        if((*treeIt)->getFieldId() == tyW->getId()){
            insert_loc = treeIt;
            break;
        }
    }//end for tree iteration
    return insert_loc;
}

pdg::ArgumentWrapper* getArgWrapperFromFunction(pdg::FunctionWrapper *funcW, Argument *arg){
    for(std::list<pdg::ArgumentWrapper*>::iterator argWI = funcW->getArgWList().begin(),
                argWE = funcW->getArgWList().end(); argWI != argWE; ++argWI){
        if( (*argWI)->getArg() == arg)
            return *argWI;
    }
    return nullptr;
}

int pdg::ProgramDependencyGraph::buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, int field_pos) {
    if(arg == nullptr){
        DEBUG(dbgs() << "In buildTypeTree, incomming arg is a nullptr\n");
        return NULLPTR;
    }
    //kinds of complex processing happen when pointers come(recursion, FILE*, )...
    if(tyW->getType()->isPointerTy()){
        //find inserting loction
        tree<InstructionWrapper*>::iterator insert_loc;

        ArgumentWrapper *pArgW = getArgWrapperFromFunction(funcMap[arg->getParent()], arg);

        if(pArgW == nullptr){
            DEBUG(dbgs() << "getArgWrapper returns a NULL pointer!" << "\n");
            return NULLPTR;
        }
        else{
            if(pArgW->getArg() == arg){
                //TODO: find a better way to do optimization, this way sucks, check historic record of recursive types to avoid redundant processing tree node
                if(recursive_types.find(tyW->getType()) != recursive_types.end() ){
                    DEBUG(dbgs() << *tyW->getType() << " is a recursive type found in historic record!\n ");
                    return RECURSIVE_TYPE;
                }

                insert_loc = getInstInsertLoc(pArgW, tyW, treeTy);

                // need to process the integer/float pointer differently
                if (tyW->getType()->getContainedType(0)->getNumContainedTypes() == 0) {
                    insertArgToTree(tyW, pArgW, treeTy, insert_loc);
                }
                //if ty is a pointer, its containedType [ty->getContainedType(0)] means the type ty points to
                llvm::Type *parent_type = nullptr;
                for(unsigned int i = 0; i < tyW->getType()->getContainedType(0)->getNumContainedTypes(); i++){
                    TypeWrapper *tempTyW = new TypeWrapper(tyW->getType()->getContainedType(0)->getContainedType(i),field_pos);
                    parent_type = tyW->getType();
                    errs() << parent_type->isPointerTy() << "\n";
                    // build a new instWrapper for each single argument and then insert the inst to instnodes.
                    InstructionWrapper *typeFieldW = new InstructionWrapper(arg->getParent(), arg, tempTyW->getType(), parent_type, field_pos++, PARAMETER_FIELD);
                    instnodes.insert(typeFieldW);
                    funcInstWList[arg->getParent()].insert(typeFieldW);
                    // start inserting formal tree instructions
                    pArgW->getTree(treeTy).append_child(insert_loc, typeFieldW);

                    //recursion, e.g. linked list, do backtracking along the path until reaching the root, if we can find an type that appears before,
                    //use 1-limit to break the tree construction when insert_loc is not the root, it means we need to do backtracking the check recursion
                    if(pArgW->getTree(treeTy).depth(insert_loc) != 0){
                        bool recursion_flag = false;
                        tree<InstructionWrapper*>::iterator backTreeIt = pArgW->getTree(treeTy).parent(insert_loc);

                        while(pArgW->getTree(treeTy).depth(backTreeIt) != 0){

                            backTreeIt = pArgW->getTree(treeTy).parent(backTreeIt);

                            if((*insert_loc)->getFieldType() == (*backTreeIt)->getFieldType()){
                                recursion_flag = true;
                                recursive_types.insert((*insert_loc)->getFieldType());
                                break;
                            }
                        }
                        if(recursion_flag == true){//process next contained type, because this type brings in a recursion
                            continue;
                        }
                    }

                    //function ptr, FILE*, complex composite struct...
                    if(tempTyW->getType()->isPointerTy()){
                        //if field is a function Ptr
                        if(tempTyW->getType()->getContainedType(0)->isFunctionTy()){
                            string Str;
                            raw_string_ostream OS(Str);
                            OS << *tempTyW->getType();

                            //if not in the unseparated_types yet, add this type into the record
                            if(unseparated_types.find(tempTyW->getType()) == unseparated_types.end()){
                                unseparated_types.insert(tempTyW->getType());
                            }

                            continue;
                        }

                        if(tempTyW->getType()->getContainedType(0)->isStructTy()){
                            string Str;
                            raw_string_ostream OS(Str);
                            OS << *tempTyW->getType();
                            //FILE*, bypass, no need to buildTypeTree
                            if("%struct._IO_FILE*" == OS.str() || "%struct._IO_marker*" == OS.str()){
                                if(unseparated_types.find(tempTyW->getType()) == unseparated_types.end()){
                                    unseparated_types.insert(tempTyW->getType());
                                }
                                continue;
                            }
                        }
                    }//end function ptr, FILE*, complex composite struct...
                    buildFormalTypeTree(arg, tempTyW, treeTy, 1);
                }//end for ty getContainedType

            }// end if pArgW->getArg() == arg
        }// end else (pArgW != nullptr)
    }//end if isPointerTy
    else{
        if(arg != nullptr)
            DEBUG(dbgs() << *tyW->getType() << " is a Non-pointer type. arg = " << *arg << "\n");
        else
            DEBUG(dbgs() << "arg is nullptr!\n");
    }
    return SUCCEED;
}

void pdg::ProgramDependencyGraph::buildFormalTree(Argument *arg, TreeType treeTy, int field_pos) {
    InstructionWrapper *treeTyW = nullptr;
    llvm::Type *parent_type = nullptr;
    treeTyW = new InstructionWrapper(arg->getParent(), arg, arg->getType(), parent_type, field_pos, FORMAL_IN);

    if(treeTyW != nullptr) {
        instnodes.insert(treeTyW);
    }
    else{
        DEBUG(dbgs() << "treeTyW is a null pointer!" <<"\n");
        exit(1);
    }

    //find the right arg, and set tree root
    std::list<pdg::ArgumentWrapper *>::iterator argWLoc;
    //find root node for formal trees(funcMap)
    for(auto argWI = funcMap[arg->getParent()]->getArgWList().begin(),
                argWE = funcMap[arg->getParent()]->getArgWList().end(); argWI != argWE; ++argWI){
        if((*argWI)->getArg() == arg)
            argWLoc = argWI;
    }

    auto treeRoot = (*argWLoc)->getTree(treeTy).set_head(treeTyW);

    //  errs() << "treeRoot = " << *(*treeRoot)->getFieldType() << "\n";

    TypeWrapper *tyW = new TypeWrapper(arg->getType(),field_pos++);

    //TODO: function ptr case...
    //avoid FILE*
    string Str;
    raw_string_ostream OS(Str);
    OS << *tyW->getType();
    //FILE*, bypass, no need to buildTypeTree
    if("%struct._IO_FILE*" == OS.str() || "%struct._IO_marker*" == OS.str()){
        errs() << "OS.str() = " << OS.str() << " FILE* appears, stop buildTypeTree\n";
    }
    else if(tyW->getType()->isPointerTy() && tyW->getType()->getContainedType(0)->isFunctionTy()){
        errs() << *arg->getParent()->getFunctionType() << " DEBUG 312: in buildFormalTree: function pointer arg = " << *tyW->getType() << "\n";
    }
    else {
        buildFormalTypeTree(arg, tyW, treeTy, field_pos);
    }
    id = 0;
}

void pdg::ProgramDependencyGraph::buildFormalParameterTrees(llvm::Function *callee) {
    auto argI = funcMap[callee]->getArgWList().begin();
    auto argE = funcMap[callee]->getArgWList().end();

    DEBUG(dbgs() << "Function: " << callee->getName() << " " << *callee->getFunctionType() << "\n");

    for(; argI != argE; ++argI){
        int field_pos = 0;
        buildFormalTree((*argI)->getArg(), FORMAL_IN_TREE, field_pos);
        field_pos++;
        DEBUG(dbgs() << " F formalInTree size = " << (*argI)->getTree(FORMAL_IN_TREE).size() << "&&\n");
        //we use this copy way just for saving time for the tree construction
        (*argI)->copyTree((*argI)->getTree(FORMAL_IN_TREE), FORMAL_OUT_TREE);
    }

    funcMap[callee]->setTreeFlag(true);
}

void pdg::ProgramDependencyGraph::printArgUseInfo(llvm::Module &M) {
    for (llvm::Function &Func : M) {
        FunctionWrapper *funcW = funcMap[&Func];
        if(funcW->getArgWList().size() == 0) {
            continue;
        }
        for (ArgumentWrapper *argW : funcW->getArgWList()) {
            Argument *arg = argW->getArg();
            llvm::Type *argTy = arg->getType();
            // if find a struct type, check GEPList to see which field is used
            //if (argTy->isStructTy() || argTy->isPointerTy()) {
            if (argTy->isPointerTy()) {
                llvm::PointerType *pt = dyn_cast<llvm::PointerType>(argTy);
                if (pt->getElementType()->isStructTy()) {
                    errs() << "\nFor Struct " << pt->getElementType()->getStructName() << " at pos " << arg->getArgNo() << "\n";
                }
                for (auto GEPInst : argW->getGEPList()) {
                    int operand_num = GEPInst->getInstruction()->getNumOperands();
                    llvm::Value *last_idx = GEPInst->getInstruction()->getOperand(operand_num - 1);
                    if (llvm::ConstantInt* constInt = dyn_cast<ConstantInt>(last_idx)) {
                        int field_idx = constInt->getSExtValue();
                        auto GEP = dyn_cast<GetElementPtrInst>(GEPInst->getInstruction());
                        if (GEP->getSourceElementType()->isPointerTy()) {
                            if (GEP->getSourceElementType()->getPointerElementType()->isStructTy()) {
                                errs() << "Struct Name: " << GEP->getSourceElementType()->getPointerElementType()->getStructName() << "\n";
                            }
                        }
                        errs() << "Field at pos " << field_idx << "  is used" << "\n";
                    }
                }
            } else {
                // if only normal type
                for (auto userIter = arg->user_begin(); userIter != arg->user_end(); ++userIter) {
                    if (llvm::Instruction *tmpInst = dyn_cast<Instruction>(*userIter)) {
                        errs() << "Arg at pos " << arg->getArgNo() << "is used" << "\n";
                    }
                }
            }
        }
    }
}

#if 0
void ProgramDependencyGraph::connectAllPossibleFunctions(
        InstructionWrapper *CInstW, FunctionType *funcTy) {
    std::map<const llvm::Function *, FunctionWrapper *>::iterator FI =
            funcMap.begin();
    std::map<const llvm::Function *, FunctionWrapper *>::iterator FE =
            funcMap.end();

    for (; FI != FE; ++FI) {
        if ((*FI).first->getFunctionType() == funcTy &&
            (*FI).first->getName() != "main") {
            LLVM_DEBUG(dbgs() << (*FI).first->getName() << " function pointer! \n");
        }
        for (list<ArgumentWrapper *>::iterator
                     argI = funcMap[(*FI).first]->getArgWList().begin(),
                     argE = funcMap[(*FI).first]->getArgWList().end();
             argI != argE; ++argI) {

            InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
            InstructionWrapper *formal_outW = *(*argI)->getTree(FORMAL_OUT_TREE).begin();
            LLVM_DEBUG(dbgs() << "Validating formal nodes:" << "\n");
            LLVM_DEBUG(dbgs() << *instnodes.find(formal_inW) << "\n");
            LLVM_DEBUG(dbgs() << *instnodes.find(formal_outW) << "\n");
            if(instnodes.find(formal_inW) == instnodes.end() || instnodes.find(formal_outW) == instnodes.end()) {
                continue;
            }
            // connect Function's EntryNode with formal in/out tree roots
            PDG->addDependency(funcMap[(*FI).first]->getEntry(), *instnodes.find(formal_inW), PARAMETER);
            PDG->addDependency(funcMap[(*FI).first]->getEntry(), *instnodes.find(formal_outW), PARAMETER);
        }
    }
}
#endif

void pdg::ProgramDependencyGraph::drawFormalParameterTree(Function *func,
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
                for (auto inst : funcInstWList[func]) {
                    //if()
                }
                DEBUG(dbgs() << "TreeType: ");
                DEBUG(dbgs() << treeTy);
                DEBUG(dbgs() << "\n");
                if (nullptr == *instnodes.find(*TI))
                    DEBUG(dbgs() << "DEBUG LINE 84 InstW NULL\n");
                if (nullptr == *instnodes.find(childW))
                    DEBUG(dbgs() << "DEBUG LINE 85 InstW NULL\n");
                PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW), PARAMETER);
            }
        } // end for tree
    }   // end for list
}

void pdg::ProgramDependencyGraph::drawActualParameterTree(CallInst *CI,
                                                          TreeType treeTy) {
    int ARG_POS = 0;
    for (list<ArgumentWrapper *>::iterator
                 argI = callMap[CI]->getArgWList().begin(),
                 argE = callMap[CI]->getArgWList().end();
         argI != argE; ++argI) {

        llvm::Value *tmp_val = CI->getOperand(ARG_POS); // get the corresponding argument
        if (llvm::Instruction *tmpInst = dyn_cast<llvm::Instruction>(tmp_val)) {
            auto TreeBegin = (*argI)->getTree(ACTUAL_IN_TREE).begin();
            PDG->addDependency(*instnodes.find(instMap[tmpInst]),
                               *instnodes.find(*TreeBegin), PARAMETER);
        }
        ARG_POS++;

        for (tree<InstructionWrapper *>::iterator
                     TI = (*argI)->getTree(treeTy).begin(),
                     TE = (*argI)->getTree(treeTy).end();
             TI != TE; ++TI) {
            for (int i = 0; i < TI.number_of_children(); i++) {
                InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

                DEBUG(llvm::dbgs() << "TreeType:");
                DEBUG(llvm::dbgs() << treeTy);
                DEBUG(llvm::dbgs() << "\n");

                if (nullptr == *instnodes.find(*TI))
                    DEBUG(llvm::dbgs() << "DEBUG LINE 103 InstW NULL\n");
                if (nullptr == *instnodes.find(childW))
                    DEBUG(llvm::dbgs() << "DEBUG LINE 104 InstW NULL\n");

                PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW), PARAMETER);
            }
        } // end for tree
    }   // end for list
}

int pdg::ProgramDependencyGraph::connectCallerAndCallee(InstructionWrapper *InstW,
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
            if ((*nodeIt)->getInstruction() == (*RI)) {
                if (nullptr != dyn_cast<ReturnInst>((*nodeIt)->getInstruction())->getReturnValue())
                    PDG->addDependency(*instnodes.find(*nodeIt), InstW, DATA_GENERAL);
                    // TODO: indirect call, function name??
                else
                    errs() << "void ReturnInst: " << *(*nodeIt)->getInstruction();
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
            DEBUG(dbgs() << "DEBUG LINE 168 InstW NULL\n");
        if (nullptr == *instnodes.find(actual_outW))
            DEBUG(dbgs() << "DEBUG LINE 169 InstW NULL\n");

        if (InstW == *instnodes.find(actual_inW) || InstW == *instnodes.find(actual_outW)) {
            DEBUG(dbgs() << "DEBUG LINE 182 InstW duplicate");
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

        // 3. ACTUAL_OUT --> LoadInsts in #Caller# function
        for (tree<InstructionWrapper *>::iterator
                     actual_out_TI = (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin(),
                     actual_out_TE = (*actual_argI)->getTree(ACTUAL_OUT_TREE).end();
             actual_out_TI != actual_out_TE; ++actual_out_TI) {

            // must handle nullptr case first
            if (nullptr == (*actual_out_TI)->getFieldType()) {
                DEBUG(dbgs() << "DEBUG ACTUAL_OUT->LoadInst: actual_out_TI->getFieldType() "
                        "is empty!\n");
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

void pdg::ProgramDependencyGraph::connectFunctionAndFormalTrees(llvm::Function *callee) {

    for (list<ArgumentWrapper *>::iterator
                 argI = funcMap[callee]->getArgWList().begin(),
                 argE = funcMap[callee]->getArgWList().end();
         argI != argE; ++argI) {

        InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
        InstructionWrapper *formal_outW = *(*argI)->getTree(FORMAL_OUT_TREE).begin();

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
            if ((*formal_in_TI)->getFieldType() == nullptr) {
                DEBUG(dbgs() << "connectFunctionAndFormalTrees: "
                        "formal_in_TI->getFieldType() == nullptr !\n");
                break;
            }

            if ((*formal_in_TI)->getFieldType() != nullptr) {
                // link specific field with GEP if there is any
                auto GEPList = (*argI)->getGEPList();
                for (auto GEPInst : GEPList) {
                    int operand_num = GEPInst->getInstruction()->getNumOperands();
                    llvm::Value *last_idx = GEPInst->getInstruction()->getOperand(operand_num - 1);
                    if (llvm::ConstantInt* constInt = dyn_cast<ConstantInt>(last_idx)) {
                        // make sure type is matched
                        llvm::Type *parent_type = (*formal_in_TI)->getParentType();
                        if (!isa<GetElementPtrInst>(GEPInst->getInstruction()) || parent_type == nullptr) continue;
                        auto GEP = dyn_cast<GetElementPtrInst>(GEPInst->getInstruction());
                        llvm::Type *GEPResTy = GEP->getResultElementType();
                        llvm::Type *GEPSrcTy = GEP->getSourceElementType();
                        //if (GEPSrcTy->isStructTy()) {
                            //errs() << "Source Type" << GEPSrcTy->getStructName() << "\n";
                        //}
                        if (parent_type != nullptr) {
                            errs() << "Source Type" << GEPSrcTy->getTypeID() << "\n";
                            errs() << parent_type->getTypeID() << "\n\n";
                        }

                        llvm::Type *TreeNodeTy = (*formal_in_TI)->getFieldType();
                        // get access field id from GEP
                        int field_idx = constInt->getSExtValue();
                        // plus one. Since for struct, the 0 index is used by the parent struct type
                        //if (field_idx+1 == (*formal_in_TI)->getFieldId() && GEPResTy == TreeNodeTy && GEPSrcTy == (*formal_in_TI)->getParentType()) {
                        // parent_type must be a pointer. Since only sub fields can have parent that is not nullptr
                        if (parent_type->isPointerTy()) {
                            parent_type = parent_type->getPointerElementType();
                        }
                        bool match = GEPSrcTy == parent_type;
                        if (field_idx+1 == (*formal_in_TI)->getFieldId() && GEPResTy == TreeNodeTy && match) {
                            PDG->addDependency(*instnodes.find(*formal_in_TI), GEPInst, STRUCT_FIELDS);
                        }
                    }
                }

                // connect formal-in-tree type nodes with storeinst in call_func
                for (auto userIter = (*argI)->getArg()->user_begin();
                     userIter != (*argI)->getArg()->user_end(); ++userIter)  {
                    if (llvm::Instruction *tmpInst = dyn_cast<Instruction>(*userIter)) {
                        PDG->addDependency(*instnodes.find(*formal_in_TI),
                                           instMap[tmpInst], DATA_GENERAL);
                    }
                }
            }

            // 2. Callee's LoadInsts --> FORMAL_OUT in Callee function
            // must handle nullptr case first
            if ((*formal_out_TI)->getFieldType() == nullptr) {
                DEBUG(dbgs() << "connectFunctionAndFormalTrees: LoadInst->FORMAL_OUT: "
                        "formal_out_TI->getFieldType() == nullptr!\n");
                break;
            }

            //      errs() << "DEBUG 214\n";
            if ((*formal_out_TI)->getFieldType() == nullptr) {

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
        } // end for (tree formal_in_TI...)
    } // end for arg iteration...
}

bool pdg::ProgramDependencyGraph::runOnModule(Module &M) {

    //Global_AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();

    DEBUG(dbgs() << "ProgramDependencyGraph::runOnModule" << '\n');

    constructFuncMap(M, funcMap);

    for (auto &func : funcMap) {
        DEBUG(dbgs() << func.first->getName() << "\n");
    }
    // Get the Module's list of global variable and function identifiers

    DEBUG(dbgs() << "======Global List: ======\n");

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
        DEBUG(dbgs() << "Module Size: " << M.size() << "\n");
        Function *F = dyn_cast<Function>(FF);

        if ((*F).isDeclaration()) {
            DEBUG(dbgs() << (*F).getName() << " is defined outside!"
                         << "\n");
            continue;
        }

        funcs++; // label this author-defined function

        DEBUG(dbgs() << "PDG " << 1.0 * funcs / M.getFunctionList().size() * 100
                     << "% completed\n");


        //constructInstMap(*F);

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

            if (isa<CallInst>(pInstruction)) {
                funcMap[&*F]->getCallInstList().push_back(dyn_cast<CallInst>(pInstruction));
#if 0
                if (DbgDeclareInst *ddi = dyn_cast<DbgDeclareInst>(pInstruction)) {
                    DEBUG(dbgs() << "Find a dbg declare Inst!!!!" << "\n");
                    DILocalVariable *div = ddi->getVariable();
                    DEBUG(dbgs() << div->getRawName()->getString().str() << "\n");
                    // fetch metadata associate with the dbg inst
                    MDNode *mdnode = dyn_cast<MDNode>(div->getRawType());
                    DICompositeType *dct = dyn_cast<DICompositeType>(mdnode);

                    std::string struct_name = dct->getName().str();
                    // ensure existing struct name
                    for (auto node : dct->getElements()) {
                        // retrive the name in the struct
                        DIDerivedType *didt = dyn_cast<DIDerivedType>(node);
                        std::string var_name = didt->getName().str();
                        if (struct_fields_map.find(struct_name) == struct_fields_map.end()) {
                            struct_fields_map[struct_name] = std::vector<std::string>();
                            struct_fields_map[struct_name].push_back(var_name);
                        } else {
                            struct_fields_map[struct_name].push_back(var_name);
                        }
                    }
                }
#endif
            }

#if 0
            if (isa<AllocaInst>(pInstruction)) {
                // find struct allocation
                AllocaInst *alloinst = dyn_cast<AllocaInst>(pInstruction);
                if (alloinst->getAllocatedType()->isStructTy() || alloinst->getAllocatedType()->isPointerTy()) {

                    if (alloinst->getAllocatedType()->isPointerTy()) {
                        llvm::PointerType *pt = dyn_cast<PointerType>(alloinst->getAllocatedType());
                        if(!pt->getElementType()->isStructTy()) {
                            continue;
                        }
                    }
                    constructStructMap(M, pInstruction, alloca_struct_map);
                }
            }
#endif
        }
        // print PtrSet only
        ControlDependencyGraph &cdgGraph = getAnalysis<ControlDependencyGraph>(*F);
        DataDependencyGraph &ddgGraph = getAnalysis<DataDependencyGraph>(*F);

        // set Entries for Function, set up links between dummy entry nodes and their func*
        for (std::set<InstructionWrapper *>::iterator nodeIt = funcInstWList[&*F].begin();
             nodeIt != funcInstWList[&*F].end(); nodeIt++) {

            InstructionWrapper *InstW = *nodeIt;
            if (InstW->getType() == ENTRY) {
                DEBUG(dbgs() << "Find Entry" << "\n");
                std::map<const llvm::Function *, FunctionWrapper *>::const_iterator FI =
                        funcMap.find(InstW->getFunction());

                if (FI != funcMap.end()) {
                    //   errs() << "find successful!" << "\n";
                    funcMap[InstW->getFunction()]->setEntry(InstW);
                }
            }
        } // end for set Entries...

        clock_t begin2 = clock();
        // process call nodes, one call node will only be touched
        // once(!InstW->getAccess)
        // the iteration should be done for the InstructionWrapperList, not original F
        for (std::set<InstructionWrapper *>::iterator nodeIt =
                funcInstWList[&*F].begin();
             nodeIt != funcInstWList[&*F].end(); ++nodeIt) {

            InstructionWrapper *InstW = *nodeIt;
            llvm::Instruction *pInstruction = InstW->getInstruction();

            if (pInstruction != nullptr && InstW->getType() == INST &&
                isa<CallInst>(pInstruction) && !InstW->getAccess()) {
                InstructionWrapper *CallInstW = InstW;
                CallInst *CI = dyn_cast<CallInst>(pInstruction);
                Function *callee = CI->getCalledFunction();

                if (callee == nullptr) {
                    DEBUG(dbgs() << "call_func = null: " << *CI << "\n");

                    Type *t = CI->getCalledValue()->getType();
                    DEBUG(dbgs() << "indirect call, called Type t = " << *t << "\n");

                    FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
                    DEBUG(dbgs() << "after cast<FunctionType>, ft = " << *funcTy << "\n");
                    // here processing the indirect function. For now, this is not connected for simplicity(debug purpose).
                    //connectAllPossibleFunctions(InstW, funcTy);
                    continue;
                }

                // TODO: isIntrinsic or not? Consider intrinsics as common
                // instructions for now, continue directly
                // determine if the function start with llvm::
                //if (callee->isIntrinsic() || callee->isDeclaration()) {
                if (callee->isIntrinsic()) {
                    //errs() << "Intrinsic: " << callee->getName() << "\n";
                    // if it is a var_annotation, save the sensitive value by the way
                    if (callee->getIntrinsicID() == Intrinsic::var_annotation) {
                        Value *v = CI->getArgOperand(0);
                        DEBUG(dbgs() << "Intrinsic var_annotation: " << *v << "\n");

                        if (isa<BitCastInst>(v)) {
                            Instruction *tempI = dyn_cast<Instruction>(v);
                            DEBUG(dbgs() << "******** BitInst opcode: " << *tempI << "BitCast \n");

                            for (llvm::Use &U : tempI->operands()) {
                                Value *v_for_cast = U.get();
                            }
                        }
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

                if (!callee->isDeclaration()) {
                    if (!callee->arg_empty()) {
                        if (funcMap[callee]->hasTrees() != true) {
                            //	  errs() << "DEBUG 456 New call for tree construction: "
                            //<< *InstW->getInstruction() << "\n";  build formal trees in memory
                            buildFormalParameterTrees(callee);
                            //initializeVisitedMap(callee, ddgGraph);
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

                    // link field inst with argument inst
                    std::list<ArgumentWrapper*> argList = funcMap[callee]->getArgWList();
                    for (ArgumentWrapper *argW : argList) {
                        tree<InstructionWrapper *>::iterator TI = argW->getTree(FORMAL_IN_TREE).begin();
                        tree<InstructionWrapper *>::iterator TE = argW->getTree(FORMAL_IN_TREE).end();
                        for(; TI != TE; ++TI) {
                            if (PDG->depends(InstW, *TI)) {
                                PDG->addDependency(InstW, *TI, STRUCT_FIELDS);
                            }
                        }
                    }
                }

            } // end callnode


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

                //if (InstW->getType() == INST || InstW->getType() == STRUCT_FIELD) {
                if (ddgGraph.DDG->depends(InstW, InstW2)) {
                    PDG->addDependency(InstW, InstW2, ddgGraph.DDG->getDataType(InstW,InstW2));
                }

                if (nullptr != InstW2->getInstruction()) {
                    if (cdgGraph.CDG->depends(InstW, InstW2)) {
                        PDG->addDependency(InstW, InstW2, CONTROL);
                    }
                }
                //} // end if(InstW->getType()== INST)


                if (InstW->getType() == ENTRY) {
                    if (nullptr != InstW2->getInstruction() &&
                        cdgGraph.CDG->depends(InstW, InstW2)) {
                        PDG->addDependency(InstW, InstW2, CONTROL);
                    }
                }

            } // end second iteration for PDG->addDependency...
        }   // end the iteration for finding CallInst

    } // end for(Module...

    DEBUG(dbgs() << "\n\n PDG construction completed! ^_^\n\n");
    DEBUG(dbgs() << "funcs = " << funcs << "\n");
    DEBUG(dbgs() << "+++++++++++++++++++++++++++++++++++++++++++++\n");

    printArgUseInfo(M);

    // clean up
    funcMap.clear();
    callMap.clear();
    instnodes.clear();
    globalList.clear();
    instMap.clear();
    funcInstWList.clear();
    return false;
}

void pdg::ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<ControlDependencyGraph>();
    AU.addRequired<DataDependencyGraph>();
    AU.setPreservesAll();
}

void pdg::ProgramDependencyGraph::print(llvm::raw_ostream &OS, const llvm::Module *) const {
    PDG->print(OS, (getPassName().data()));
}

//pdg::ProgramDependencyGraph *CreateProgramDependencyGraphPass() {
//    return new pdg::ProgramDependencyGraph();
//}

static RegisterPass<pdg::ProgramDependencyGraph>
        PDG("pdg", "Program Dependency Graph Construction", false, true);
