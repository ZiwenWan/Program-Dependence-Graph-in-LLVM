#include "FunctionWrapper.h"

void pdg::ArgumentWrapper::copyTree(const tree<InstructionWrapper *> &srcTree, TreeType treeTy) {
        if (srcTree.empty()) {
                errs() << *arg->getParent() << " arg : " << *arg << " srcTree is empty!\n";
                exit(1);
        }

        InstWrapperType instWTy;//formal_out actual_in/out

        switch (treeTy) {
                case FORMAL_IN_TREE:
                        errs() << "FORMAL_IN_TREE can't be copied\n";
                break;
                case FORMAL_OUT_TREE:
                        formalOutTree = srcTree;
                instWTy = FORMAL_OUT;
                break;
                case ACTUAL_IN_TREE:
                        actualInTree = srcTree;
                instWTy = ACTUAL_IN;
                break;
                case ACTUAL_OUT_TREE:
                        actualOutTree = srcTree;
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
                        typeFieldW = new InstructionWrapper((*SI)->getFunction(), (*SI)->getArgument(),
                                                            (*SI)->getFieldType(), id++, instWTy);
                } else {
                        typeFieldW = new InstructionWrapper((*SI)->getFunction(), (*SI)->getArgument(),
                                                            (*SI)->getFieldType(), id++, PARAMETER_FIELD);
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
