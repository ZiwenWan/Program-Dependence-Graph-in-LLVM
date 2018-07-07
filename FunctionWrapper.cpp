#include "FunctionWrapper.h"

void pdg::ArgumentWrapper::copyTree(const tree<InstructionWrapper *> &srcTree, TreeType treeTy) {
        if (srcTree.empty()) {
                errs() << *arg->getParent() << " arg : " << *arg << " srcTree is empty!\n";
                exit(1);
        }

        InstWrapperType instWTy;//formal_out actual_in/out
        // choose new argument. CallInst should get arg from CallSite instead of formal function.
        Argument *newArg = nullptr;
        switch (treeTy) {
                case FORMAL_IN_TREE:
                        errs() << "FORMAL_IN_TREE can't be copied\n";
                break;
                case FORMAL_OUT_TREE:
                        formalOutTree = srcTree;
                newArg = (*srcTree.begin())->getArgument();
                instWTy = FORMAL_OUT;
                break;
                case ACTUAL_IN_TREE:
                        actualInTree = srcTree;
                newArg = this->getArg();
                instWTy = ACTUAL_IN;
                break;
                case ACTUAL_OUT_TREE:
                        actualOutTree = srcTree;
                newArg = this->getArg();
                instWTy = ACTUAL_OUT;
                break;
        }

//    formalOutTree = formalInTree;
        tree<InstructionWrapper *>::iterator SI = srcTree.begin(), SE = srcTree.end();
        tree<InstructionWrapper *>::iterator TI = this->getTree(treeTy).begin(), TE = this->getTree(treeTy).end();

//    InstructionWrapper(Function *Func, Argument *arg, Type* field_type, int field_id, InstWrapperType type) ;
        for (; SI != SE && TI != TE; ++SI, ++TI) {
                InstructionWrapper *typeFieldW;
                if (SI == srcTree.begin()) {
                        typeFieldW = new InstructionWrapper((*SI)->getFunction(), newArg,
                                                            (*SI)->getFieldType(), nullptr, id++, instWTy);
                } else {
                        typeFieldW = new InstructionWrapper((*SI)->getFunction(), newArg,
                                                            (*SI)->getFieldType(), nullptr, id++, PARAMETER_FIELD);
                }
                *TI = typeFieldW;
                instnodes.insert(typeFieldW);
        }
}//end copyTree


bool pdg::FunctionWrapper::hasFuncOrFilePrt() {
        // check whether a function has a function_ptr or FILE parameter
        for (Function::arg_iterator argi = Func->arg_begin(),
                     arge = Func->arg_end();
             argi != arge; ++argi) {
                // params.push_back(argi->getType());
                // function ptr, put func into insensitive_set, not sensitive
                if (argi->getType()->isPointerTy()) {

                        if (argi->getType()->getContainedType(0)->isFunctionTy()) {
                                std::string Str;
                                raw_string_ostream OS(Str);
                                OS << *argi->getType();
                                errs() << "DEBUG LINE 700 FunctionPointerType : " << OS.str() << "\n";
                                return true;
                        }

                        if (argi->getType()->getContainedType(0)->isStructTy()) {
                                std::string Str;
                                raw_string_ostream OS(Str);
                                OS << *argi->getType();

                                // FILE*, bypass, no need to buildTypeTree
                                if ("%struct._IO_FILE*" == OS.str() ||
                                    "%struct._IO_marker*" == OS.str()) {

                                        errs() << "DEBUG LINE 711 struct._IO_marker*/struct._IO_FILE* \n";
                                        return true;
                                        // continue;
                                }
                        }
                }
        } // end for Function::arg_iterator argi...

        return false;
}

llvm::Instruction* pdg::ArgumentWrapper::findAllocaInst(Argument *arg) {
        llvm::Instruction *ALLOCA_DEPENDEN_ON_ARG = nullptr;
        for (auto UB = arg->user_begin(); UB != arg->user_end(); ++ UB) {
                if(Instruction *userInst = dyn_cast<Instruction>(*UB)) {
                        if (isa<StoreInst>(userInst)) {
                                for (Instruction::const_op_iterator cuit = userInst->op_begin();
                                     cuit != userInst->op_end(); ++cuit) {
                                        if (isa<llvm::AllocaInst>(*cuit)) {
                                                auto tmp = dyn_cast<Instruction>(*cuit);
                                                ALLOCA_DEPENDEN_ON_ARG = tmp;
                                        }
                                }
                        }
                }
        }
        if (ALLOCA_DEPENDEN_ON_ARG == nullptr) {
                errs() << "Empty alloca inst" << "\n";
        }
        return ALLOCA_DEPENDEN_ON_ARG;
}

void pdg::ArgumentWrapper::getDependentGEP(llvm::Instruction *allocaInst) {
        std::queue<llvm::Instruction *> dependency_queue;

        dependency_queue.push(allocaInst);
        while (!dependency_queue.empty()) {
                llvm::Instruction *inst = dependency_queue.front();
                dependency_queue.pop();
                for (auto UB = inst->user_begin(); UB != inst->user_end(); ++UB) {
                        if (Instruction *tmpInst = dyn_cast<Instruction>(*UB)) {
                                dependency_queue.push(tmpInst);
                                // if find a GEP inst, add to return res, this GEP must
                                if (isa<GetElementPtrInst>(tmpInst)) {
                                        if (instMap[tmpInst] != nullptr) {
                                                this->GEPList.insert(instMap[tmpInst]);
                                        }
                                }
                        }
                }
        }
}
