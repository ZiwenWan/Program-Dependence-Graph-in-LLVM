/** ---*- C++ -*--- ConnectFunctions.cpp --------------------------
 * This cpp file implements some functions for connecting callers and callees, using parameter trees.
 *
 */
#include "ConnectFunctions.h"


using namespace std;
using namespace pdg;

//static std::set<Type*> recursive_types;
//static std::set<Type*> unseparated_types;




//void pdg::addDependencies(pdg::ProgramDependencyGraph *pdg, InstructionWrapper *instW1, InstructionWrapper *instW2,
//                          InstWrapperType instWTy) {
//    pdg->PDG->addDependency(instW1, instW2, InstWTy);
//}

void pdg::insertArgToTree(TypeWrapper *tyW, ArgumentWrapper *pArgW, TreeType treeTy, tree<InstructionWrapper*>::iterator insert_loc) {
    Argument *arg = pArgW->getArg();
    TypeWrapper *tempTyW = new TypeWrapper(tyW->getType()->getPointerElementType(), id);
    llvm::Type *parent_type = nullptr;
    InstructionWrapper *typeFieldW = new InstructionWrapper(arg->getParent(), arg, tempTyW->getType(), parent_type, id++,PARAMETER_FIELD);
    instnodes.insert(typeFieldW);
    pArgW->getTree(treeTy).append_child(insert_loc,typeFieldW);
}

#if 0
int pdg::buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, DataDependencyGraph &ddg){
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
                for(unsigned int i = 0; i < tyW->getType()->getContainedType(0)->getNumContainedTypes(); i++){
                    TypeWrapper *tempTyW = new TypeWrapper(tyW->getType()->getContainedType(0)->getContainedType(i),id);
                    // build a new instWrapper for each single argument and then insert the inst to instnodes.
                    InstructionWrapper *typeFieldW = new InstructionWrapper(arg->getParent(), arg, tempTyW->getType(), id++, PARAMETER_FIELD);
                    instnodes.insert(typeFieldW);
                    //funcInstWList[arg->getParent()].insert(typeFieldW);
                    // start inserting formal tree instructions
                    pArgW->getTree(treeTy).append_child(insert_loc, typeFieldW);

                    // create a field inst for this specific argument. And insert to instnodes to be processed.
                    llvm::Instruction *allocaInst = findAllocaInst(arg);
                    InstructionWrapper *fieldInstW = nullptr;
                    if (allocaInst != nullptr) {
                        fieldInstW = new InstructionWrapper(allocaInst, arg->getParent(), i+10, STRUCT_FIELD, tyW->getType());
                    } else {
                        fieldInstW = new InstructionWrapper(arg->getParent(), i, STRUCT_FIELD, tyW->getType());
                    }
                    instnodes.insert(fieldInstW);
                    funcInstWList[arg->getParent()].insert(fieldInstW);
                    ddg.DDG->addDependency(typeFieldW, fieldInstW, STRUCT_FIELDS);
                    //ddg.DDG->addDependency(fieldInstW, typeFieldW, STRUCT_FIELDS);

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

                    buildFormalTypeTree(arg, tempTyW, treeTy, ddg);
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

//build formal in/out tree for each argument in function body(funcMap)
void pdg::buildFormalTree(Argument *arg, TreeType treeTy, DataDependencyGraph &ddg){
    InstructionWrapper *treeTyW = nullptr;
    treeTyW = new InstructionWrapper(arg->getParent(), arg, arg->getType(), id, FORMAL_IN);

    if(treeTyW != nullptr) {
        instnodes.insert(treeTyW);
    }
    else{
        DEBUG(dbgs() << "treeTyW is a null pointer!" <<"\n");
        exit(1);
    }

    //find the right arg, and set tree root
    std::list<ArgumentWrapper *>::iterator argWLoc;
    //find root node for formal trees(funcMap)
    for(auto argWI = funcMap[arg->getParent()]->getArgWList().begin(),
                argWE = funcMap[arg->getParent()]->getArgWList().end(); argWI != argWE; ++argWI){
        if((*argWI)->getArg() == arg)
            argWLoc = argWI;
    }

    auto treeRoot = (*argWLoc)->getTree(treeTy).set_head(treeTyW);

    //  errs() << "treeRoot = " << *(*treeRoot)->getFieldType() << "\n";

    TypeWrapper *tyW = new TypeWrapper(arg->getType(),id++);

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
        buildFormalTypeTree(arg, tyW, treeTy, ddg);
    }
    id = 0;
}

//build formal trees for each function

void pdg::buildFormalParameterTrees(Function* callee, DataDependencyGraph &ddg){
    // sequentially set up parameter trees for the each argument
    auto argI = funcMap[callee]->getArgWList().begin();
    auto argE = funcMap[callee]->getArgWList().end();

    DEBUG(dbgs() << "Function: " << callee->getName() << " " << *callee->getFunctionType() << "\n");

    for(; argI != argE; ++argI){
        buildFormalTree((*argI)->getArg(), FORMAL_IN_TREE, ddg);
        DEBUG(dbgs() << " F formalInTree size = " << (*argI)->getTree(FORMAL_IN_TREE).size() << "&&\n");
        //we use this copy way just for saving time for the tree construction
        (*argI)->copyTree((*argI)->getTree(FORMAL_IN_TREE), FORMAL_OUT_TREE);
    }

    funcMap[callee]->setTreeFlag(true);
}
#endif

//build actual trees for each call instruction
void pdg::buildActualParameterTrees(CallInst *CI){
    Function *callee = CI->getCalledFunction();

    auto argI = callMap[CI]->getArgWList().begin();
    auto argE = callMap[CI]->getArgWList().end();

    auto argFI = funcMap[callee]->getArgWList().begin();
    auto argFE = funcMap[callee]->getArgWList().end();

    DEBUG(dbgs() << "Building actual parameter tree:" << callee->getName() << "\n");
    //  errs() << "argFI FORMAL_IN_TREE size " << (*argFI)->getTree(FORMAL_IN_TREE).size() << "\n";

    //copy FormalInTree in callee to ActualIn/OutTree in callMap
    for(; argI != argE && argFI != argFE; ++argI,++argFI){
        (*argI)->copyTree((*argFI)->getTree(FORMAL_IN_TREE), ACTUAL_IN_TREE);
        (*argI)->copyTree((*argFI)->getTree(FORMAL_IN_TREE), ACTUAL_OUT_TREE);
        //errs() << "argIcopyTree size " << (*argI)->getTree(ACTUAL_IN_TREE).size() << " " << (*argI)->getTree(ACTUAL_OUT_TREE).size();
    }
}
