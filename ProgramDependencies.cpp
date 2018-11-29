#include "ProgramDependencies.h"
#include "DSAGenerator.h"

#define SUCCEED 0
#define NULLPTR 1
#define RECURSIVE_TYPE 2

using namespace llvm;
using namespace std;

char pdg::ProgramDependencyGraph::ID = 0;

static std::set<Type*> recursive_types;

tree<pdg::InstructionWrapper*>::iterator pdg::ProgramDependencyGraph::getInstInsertLoc(pdg::ArgumentWrapper *argW, InstructionWrapper *tyW, TreeType treeTy) {
  tree<pdg::InstructionWrapper *>::iterator insert_loc = argW->getTree(treeTy).begin();
  while ((*insert_loc) != tyW && insert_loc != argW->getTree(treeTy).end()) {
    insert_loc++;
  }
  return insert_loc;
}

bool pdg::ProgramDependencyGraph::isBasicTypeOrPtr(llvm::Type *ty)
{
    if (ty == nullptr)
    {
        return false;
    }

    if (ty->isPointerTy())
    {
        ty = dyn_cast<PointerType>(ty)->getElementType();
    }

    return ty->isSingleValueType();
}

pdg::ArgumentWrapper* pdg::ProgramDependencyGraph::getArgWByIdx(pdg::FunctionWrapper* funcW, int argWIdx)
{
    return (funcW->getArgWList())[argWIdx];
}

pdg::ArgumentWrapper* getArgWrapperFromFunction(pdg::FunctionWrapper *funcW, Argument *arg)
{
    for (std::vector<pdg::ArgumentWrapper *>::iterator argWI = funcW->getArgWList().begin(),
                                                     argWE = funcW->getArgWList().end();
         argWI != argWE; ++argWI)
    {
        if ((*argWI)->getArg() == arg)
            return *argWI;
    }
    return nullptr;
}

void pdg::ProgramDependencyGraph::insertArgToTree(TypeWrapper *tyW, ArgumentWrapper *pArgW, TreeType treeTy,
                                                  tree<InstructionWrapper *>::iterator insertLoc) {
    Argument *arg = pArgW->getArg();
    TypeWrapper *tempTyW = new TypeWrapper(tyW->getType()->getPointerElementType(), id);
    llvm::Type *parent_type = nullptr;
    InstructionWrapper *typeFieldW = new InstructionWrapper(arg->getParent(), arg, tempTyW->getType(), parent_type, id++,PARAMETER_FIELD);
    instnodes.insert(typeFieldW);
    pArgW->getTree(treeTy).append_child(insertLoc,typeFieldW);
}

int pdg::ProgramDependencyGraph::buildFormalTypeTree(Argument *arg, InstructionWrapper *tyW, TreeType treeTy, int field_pos ) {
    if(arg == nullptr) {
        DEBUG(dbgs() << "In buildTypeTree, incoming arg is a nullptr\n");
        return NULLPTR;
    }

    if (!tyW->getFieldType()->isPointerTy()) {
        if(arg != nullptr)
            DEBUG(dbgs() << tyW->getFieldType() << " is a Non-pointer type. arg = " << *arg << "\n");
        else
            DEBUG(dbgs() << "arg is nullptr!\n");
        return SUCCEED;
    }

    //kinds of complex processing happen when pointers come(recursion, FILE*, )...
    ArgumentWrapper *pArgW = getArgWrapperFromFunction(funcMap[arg->getParent()], arg);

    if(pArgW == nullptr){
        DEBUG(dbgs() << "getArgWrapper returns a NULL pointer!" << "\n");
        return NULLPTR;
    }

    // setup worklist to avoid recusion processing
    std::queue<InstructionWrapper *> worklist;
    worklist.push(tyW);
    // std::set<Type *> recursive_types;
    int depth = 0;
    tree<InstructionWrapper *>::iterator insert_loc;
    while (!worklist.empty()) {
      if (depth >= 1) {
          errs() << "Tree stop building at level " << depth << "\n";
          return SUCCEED;
      }
      depth += 1;
      int queue_size = worklist.size();
      while (queue_size > 0) {
        InstructionWrapper *curTyNode = worklist.front();
        worklist.pop();
        queue_size -= 1;
        insert_loc = getInstInsertLoc(pArgW, curTyNode, treeTy);
        // this function works on pointer type data.
        if (!curTyNode->getFieldType()->isPointerTy()) {
          continue;
        }
        // need to process the singlevalue type pointer differently
        if (PointerType *pt = dyn_cast<PointerType>(curTyNode->getFieldType())) {
          llvm::Type *element_type = pt->getElementType();
          if (element_type->isSingleValueType()) {
            // insert pointer type node
            // insertArgToTree(curTyNode, pArgW, treeTy, insert_loc);
            // insert pointed buffer node
            errs() << "Find single type node\n";
            llvm::Type *pointedNodeTy = pt->getElementType();
            // by default, the pointed buffer has 0 offset compared to parent
            InstructionWrapper *pointedTypeFieldW = new InstructionWrapper(
                arg->getParent(), arg, pointedNodeTy, curTyNode->getFieldType(),
                0, PARAMETER_FIELD);
            instnodes.insert(pointedTypeFieldW);
            funcInstWList[arg->getParent()].insert(pointedTypeFieldW);
            pArgW->getTree(treeTy).append_child(insert_loc, pointedTypeFieldW);
            return SUCCEED;
          }

          if (!element_type->isStructTy()) {
            return SUCCEED;
          }
        }

        if (recursive_types.find(curTyNode->getFieldType()) !=
            recursive_types.end()) {
          DEBUG(dbgs() << curTyNode->getFieldType()
                       << " is a recursive type found in historic record!\n ");
          errs() << curTyNode->getFieldType()
                 << " is a recursive type found in historic record!\n ";
          return RECURSIVE_TYPE;
        }

        // insert_loc = getInstInsertLoc(pArgW, curTyNode, treeTy);
        // if ty is a pointer, its containedType [ty->getContainedType(0)] means
        // the type ty points to
        llvm::Type *parent_type = nullptr;
        // for(unsigned int i = 0; i <
        // curTyNode->getType()->getContainedType(0)->getNumContainedTypes();
        // i++) {
        for (unsigned int i = 0; i < dyn_cast<PointerType>(curTyNode->getFieldType())->getElementType()->getNumContainedTypes(); i++) {
          // recursion, e.g. linked list, do backtracking along the path until
          // reaching the root, if we can find an type that appears before, use
          // 1-limit to break the tree construction when insert_loc is not the
          // root, it means we need to do backtracking the check recursion
          if (pArgW->getTree(treeTy).depth(insert_loc) != 0) {
            bool recursion_flag = false;
            tree<InstructionWrapper *>::iterator backTreeIt = pArgW->getTree(treeTy).parent(insert_loc);
            while (pArgW->getTree(treeTy).depth(backTreeIt) != 0) {
              if ((*insert_loc)->getFieldType() == (*backTreeIt)->getFieldType()) {
                errs() << "Find recursion type!" << "\n";
                recursion_flag = true;
                recursive_types.insert((*insert_loc)->getFieldType());
                break;
              }
              backTreeIt = pArgW->getTree(treeTy).parent(backTreeIt);
            }

            if (recursion_flag) {
              // process next contained type, because this type brings in a
              // recursion
              continue;
            }
          }

          // here extracting the contained filed, and insert to tree. This
          // happens after we make sure the current node is not recursive for
          // example: int* -> int, struct -> stcut.field
          int cur_field_offset = i;
          parent_type = curTyNode->getFieldType();
          llvm::Type *childType = curTyNode->getFieldType()->getContainedType(0)->getContainedType(i);
          // if (childType->isIntegerTy()) {
          //   if (childType->getIntegerBitWidth() <= 8) {
          //     errs() << "Find child has less than 8 bits width\n";
          //     continue;
          //   }
          // }
          InstructionWrapper *typeFieldW = new InstructionWrapper( arg->getParent(), arg, childType, parent_type, cur_field_offset, PARAMETER_FIELD);
          instnodes.insert(typeFieldW);
          funcInstWList[arg->getParent()].insert(typeFieldW);
          // start inserting formal tree instructions
          pArgW->getTree(treeTy).append_child(insert_loc, typeFieldW);
          // function ptr, FILE*, complex composite struct...

          if (childType->isPointerTy()) {
            Type *childEleTy =
                dyn_cast<PointerType>(childType)->getElementType();
            // if field is a function Ptr
            if (childType->isFunctionTy() || childEleTy->isFunctionTy()) {
              string Str;
              raw_string_ostream OS(Str);
              OS << childType;
              continue;
            }

            if (childType->isStructTy() || childEleTy->isStructTy()) {
              string Str;
              raw_string_ostream OS(Str);
              OS << childType;
              // FILE*, bypass, no need to buildTypeTree
              if ("%struct._IO_FILE*" == OS.str() ||
                  "%struct._IO_marker*" == OS.str()) {
                continue;
              }
            }
          }
          worklist.push(typeFieldW);
        }
      }
    }

    return SUCCEED;
}

void pdg::ProgramDependencyGraph::buildFormalTree(Argument *arg,
                                                  TreeType treeTy,
                                                  int field_pos) {
  InstructionWrapper *treeTyW = nullptr;
  llvm::Type *parent_type = nullptr;
  treeTyW = new InstructionWrapper(arg->getParent(), arg, arg->getType(),
                                   parent_type, field_pos, FORMAL_IN);

  if (treeTyW != nullptr) {
    instnodes.insert(treeTyW);
  } else {
    DEBUG(dbgs() << "treeTyW is a null pointer!"
                 << "\n");
    exit(1);
  }

  // find the right arg, and set tree root
  std::vector<pdg::ArgumentWrapper *>::iterator argWLoc;
  // find root node for formal trees(funcMap)
  for (auto argWI = funcMap[arg->getParent()]->getArgWList().begin();
       argWI != funcMap[arg->getParent()]->getArgWList().end(); ++argWI) {
    if ((*argWI)->getArg() == arg) {
      argWLoc = argWI;
    }
  }

  auto treeRoot = (*argWLoc)->getTree(treeTy).set_head(treeTyW);
  // TypeWrapper *tyW = new TypeWrapper(arg->getType(),field_pos++);
  string Str;
  raw_string_ostream OS(Str);
  // OS << *tyW->getType();
  // FILE*, bypass, no need to buildTypeTree
  if ("%struct._IO_FILE*" == OS.str() || "%struct._IO_marker*" == OS.str()) {
    errs() << "OS.str() = " << OS.str()
           << " FILE* appears, stop buildTypeTree\n";
  } else if (treeTyW->getFieldType()->isPointerTy() &&
             treeTyW->getFieldType()->getContainedType(0)->isFunctionTy()) {
    errs() << *arg->getParent()->getFunctionType()
           << " DEBUG 312: in buildFormalTree: function pointer arg = "
           << *treeTyW->getFieldType() << "\n";
  } else {
    // errs() << "start building formal type tree!" << "\n";
    buildFormalTypeTree(arg, treeTyW, treeTy, field_pos);
  }
  id = 0;
}

void pdg::ProgramDependencyGraph::buildFormalParameterTrees(
    llvm::Function *callee) {
  auto argI = funcMap[callee]->getArgWList().begin();
  auto argE = funcMap[callee]->getArgWList().end();

  DEBUG(dbgs() << "Function: " << callee->getName() << " "
               << *callee->getFunctionType() << "\n");
  // errs() << "Function: " << callee->getName() << " " <<
  // *callee->getFunctionType() << "\n";

  int field_pos = 0;
  for (; argI != argE; ++argI) {
    buildFormalTree((*argI)->getArg(), FORMAL_IN_TREE, field_pos);
    DEBUG(dbgs() << " F formalInTree size = "
                 << (*argI)->getTree(FORMAL_IN_TREE).size() << "&&\n");
    // we use this copy way just for saving time for the tree construction
    (*argI)->copyTree((*argI)->getTree(FORMAL_IN_TREE), FORMAL_OUT_TREE);
  }

  funcMap[callee]->setTreeFlag(true);
}

void pdg::ProgramDependencyGraph::buildActualParameterTrees(CallInst *CI) {
  Function *called_func;
  // processing indirect call. Pick the first candidate function and copy its'
  // formal parameter tree.
  if (CI->getCalledFunction() == nullptr) {
    std::vector<llvm::Function *> indirect_call_candidate =
        collectIndirectCallCandidates(CI->getFunctionType());
    if (indirect_call_candidate.size() == 0) {
      errs() << "Parameter num 0, no need to build actual parameter tree"
             << "\n";
      return;
    }
    // get the first possible candidate
    called_func = indirect_call_candidate[0];
  } else {
    called_func = CI->getCalledFunction();
  }

  auto argI = callMap[CI]->getArgWList().begin();
  auto argE = callMap[CI]->getArgWList().end();

  auto argFI = funcMap[called_func]->getArgWList().begin();
  auto argFE = funcMap[called_func]->getArgWList().end();

  DEBUG(dbgs() << "Building actual parameter tree:" << called_func->getName()
               << "\n");
  //  errs() << "argFI FORMAL_IN_TREE size " <<
  //  (*argFI)->getTree(FORMAL_IN_TREE).size() << "\n";

  // copy FormalInTree in callee to ActualIn/OutTree in callMap
  for (; argI != argE && argFI != argFE; ++argI, ++argFI) {
    (*argI)->copyTree((*argFI)->getTree(FORMAL_IN_TREE), ACTUAL_IN_TREE);
    (*argI)->copyTree((*argFI)->getTree(FORMAL_IN_TREE), ACTUAL_OUT_TREE);
    // errs() << "argIcopyTree size " << (*argI)->getTree(ACTUAL_IN_TREE).size()
    // << " " << (*argI)->getTree(ACTUAL_OUT_TREE).size();
  }
}

void pdg::ProgramDependencyGraph::drawFormalParameterTree(Function *func,
                                                          TreeType treeTy) {
  for (std::vector<ArgumentWrapper *>::iterator
           argI = funcMap[func]->getArgWList().begin(),
           argE = funcMap[func]->getArgWList().end();
       argI != argE; ++argI) {
    for (tree<InstructionWrapper *>::iterator
             TI = (*argI)->getTree(treeTy).begin(),
             TE = (*argI)->getTree(treeTy).end();
         TI != TE; ++TI) {
      for (unsigned i = 0; i < TI.number_of_children(); i++) {
        InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);
        DEBUG(dbgs() << "TreeType: ");
        DEBUG(dbgs() << treeTy);
        DEBUG(dbgs() << "\n");
        if (nullptr == *instnodes.find(*TI))
          DEBUG(dbgs() << "DEBUG LINE 84 InstW NULL\n");
        if (nullptr == *instnodes.find(childW))
          DEBUG(dbgs() << "DEBUG LINE 85 InstW NULL\n");
        PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW),
                           PARAMETER);
      }
    } // end for tree
  }   // end for list
}

void pdg::ProgramDependencyGraph::drawActualParameterTree(CallInst *CI,
                                                          TreeType treeTy) {
  int ARG_POS = 0;
  for (std::vector<ArgumentWrapper *>::iterator
           argI = callMap[CI]->getArgWList().begin(),
           argE = callMap[CI]->getArgWList().end();
       argI != argE; ++argI) {

    llvm::Value *tmp_val =
        CI->getOperand(ARG_POS); // get the corresponding argument
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
      for (unsigned i = 0; i < TI.number_of_children(); i++) {
        InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

        DEBUG(llvm::dbgs() << "TreeType:");
        DEBUG(llvm::dbgs() << treeTy);
        DEBUG(llvm::dbgs() << "\n");

        if (nullptr == *instnodes.find(*TI))
          DEBUG(llvm::dbgs() << "DEBUG LINE 103 InstW NULL\n");
        if (nullptr == *instnodes.find(childW))
          DEBUG(llvm::dbgs() << "DEBUG LINE 104 InstW NULL\n");

        PDG->addDependency(*instnodes.find(*TI), *instnodes.find(childW),
                           PARAMETER);
      }
    } // end for tree
  }   // end for list
}

int pdg::ProgramDependencyGraph::connectCallerAndCallee(
    InstructionWrapper *InstW, llvm::Function *callee) {
  if (InstW == nullptr || callee == nullptr) {
    return 1;
  }

  // callInst in caller --> Entry Node in callee
  PDG->addDependency(InstW, funcMap[callee]->getEntry(), CONTROL);

  Function *caller = InstW->getInstruction()->getFunction();
  // ReturnInst in callee --> CallInst in caller
  for (std::list<Instruction *>::iterator
           RI = funcMap[callee]->getReturnInstList().begin(),
           RE = funcMap[callee]->getReturnInstList().end();
       RI != RE; ++RI) {

    for (std::set<InstructionWrapper *>::iterator nodeIt =
             funcInstWList[caller].begin();
         nodeIt != funcInstWList[caller].end(); ++nodeIt) {
      if ((*nodeIt)->getInstruction() == (*RI)) {
        if (nullptr !=
            dyn_cast<ReturnInst>((*nodeIt)->getInstruction())->getReturnValue())
          PDG->addDependency(*instnodes.find(*nodeIt), InstW, DATA_GENERAL);
        else
          errs() << "void ReturnInst: " << *(*nodeIt)->getInstruction();
      }
    }
  }

  // TODO: consider all kinds of connecting cases
  // connect caller InstW with ACTUAL IN/OUT parameter trees
  CallInst *CI = dyn_cast<CallInst>(InstW->getInstruction());

  for (std::vector<ArgumentWrapper *>::iterator
           argI = callMap[CI]->getArgWList().begin(),
           argE = callMap[CI]->getArgWList().end();
       argI != argE; ++argI) {

    InstructionWrapper *actual_inW = *(*argI)->getTree(ACTUAL_IN_TREE).begin();
    InstructionWrapper *actual_outW =
        *(*argI)->getTree(ACTUAL_OUT_TREE).begin();

    if (nullptr == *instnodes.find(actual_inW))
      DEBUG(dbgs() << "DEBUG LINE 168 InstW NULL\n");
    if (nullptr == *instnodes.find(actual_outW))
      DEBUG(dbgs() << "DEBUG LINE 169 InstW NULL\n");

    if (InstW == *instnodes.find(actual_inW) ||
        InstW == *instnodes.find(actual_outW)) {
      DEBUG(dbgs() << "DEBUG LINE 182 InstW duplicate");
      continue;
    }

    PDG->addDependency(InstW, *instnodes.find(actual_inW), PARAMETER);
    PDG->addDependency(InstW, *instnodes.find(actual_outW), PARAMETER);
  }

  // old way, process four trees at the same time, remove soon
  std::vector<ArgumentWrapper *>::iterator formal_argI;
  formal_argI = funcMap[callee]->getArgWList().begin();

  std::vector<ArgumentWrapper *>::iterator formal_argE;
  formal_argE = funcMap[callee]->getArgWList().end();

  std::vector<ArgumentWrapper *>::iterator actual_argI;
  actual_argI = callMap[CI]->getArgWList().begin();

  std::vector<ArgumentWrapper *>::iterator actual_argE;
  actual_argE = callMap[CI]->getArgWList().end();

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

      if (actual_in_TI == nullptr || formal_in_TI == nullptr) {
        return 1;
      }

      InstructionWrapper *actual_in_instW = *instnodes.find(*actual_in_TI);
      InstructionWrapper *formal_in_instW = *instnodes.find(*formal_in_TI);
      InstructionWrapper *actual_out_instW = *instnodes.find(*actual_out_TI);
      InstructionWrapper *formal_out_instW = *instnodes.find(*formal_out_TI);

      if (actual_in_instW == nullptr || formal_in_instW == nullptr) {
        // no need to build later process
        return 1;
      }

      // connect trees: antual-in --> formal-in, formal-out --> actual-out
      PDG->addDependency(actual_in_instW, formal_in_instW, PARAMETER);

      PDG->addDependency(actual_out_instW, formal_out_instW, PARAMETER);

    } // end for(tree...) intra-connection between actual/formal

    // 3. ACTUAL_OUT --> LoadInsts in #Caller# function
    for (tree<InstructionWrapper *>::iterator
             actual_out_TI = (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin(),
             actual_out_TE = (*actual_argI)->getTree(ACTUAL_OUT_TREE).end();
         actual_out_TI != actual_out_TE; ++actual_out_TI) {

      // must handle nullptr case first
      if (nullptr == (*actual_out_TI)->getFieldType()) {
        DEBUG(dbgs()
              << "DEBUG ACTUAL_OUT->LoadInst: actual_out_TI->getFieldType() "
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
            std::set<InstructionWrapper *>::iterator nodeItB =
                funcInstWList[callee].begin();
            std::set<InstructionWrapper *>::iterator nodeItE =
                funcInstWList[callee].begin();
            for (; nodeItB != nodeItE; ++nodeItB) {
              if ((*nodeItB)->getInstruction() == dyn_cast<Instruction>(*LI))
                PDG->addDependency(*instnodes.find(*actual_out_TI),
                                   *instnodes.find(*nodeItB), DATA_GENERAL);
            }
          }
        } // end for(; LI != LE; ++LI)
      }   // end if(nullptr != (*TI3)->...)
    }     // end for(tree actual_out_TI = getTree FORMAL_OUT_TREE)

  } // end for argI iteration

  return 0;
}

std::vector<std::pair<pdg::InstructionWrapper *, pdg::InstructionWrapper *>>
pdg::ProgramDependencyGraph::getParameterTreeNodeWithCorrespondGEP(
    ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI) {
  // find all GEP elements, including GEP in nested call through function call.
  // Avoid repeat function
  std::set<InstructionWrapper *> RelevantGEPList = getAllRelevantGEP(argW->getArg());

  std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>> treeNodeGepPairs;
  for (auto GEPInstW : RelevantGEPList) {
    int operand_num = GEPInstW->getInstruction()->getNumOperands();
    llvm::Value *last_idx =
        GEPInstW->getInstruction()->getOperand(operand_num - 1);
    // cast the last_idx to int type
    if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx)) {
      // make sure type is matched
      llvm::Type *parent_type = (*formal_in_TI)->getParentType();
      if (!isa<GetElementPtrInst>(GEPInstW->getInstruction()) ||
          parent_type == nullptr)
        continue;
      auto GEP = dyn_cast<GetElementPtrInst>(GEPInstW->getInstruction());
      llvm::Type *GEPResTy = GEP->getResultElementType();
      llvm::Type *GEPSrcTy = GEP->getSourceElementType();

      if (parent_type != nullptr) {
        DEBUG(dbgs() << "Source Type" << GEPSrcTy->getTypeID() << "\n");
        DEBUG(dbgs() << parent_type->getTypeID() << "\n\n");
      }

      llvm::Type *TreeNodeTy = (*formal_in_TI)->getFieldType();
      // get access field idx from GEP
      int field_idx = constInt->getSExtValue();
      // plus one. Since for struct, the 0 index is used by the parent struct
      // type parent_type must be a pointer. Since only sub fields can have
      // parent that is not nullptr
      if (parent_type->isPointerTy()) {
        parent_type = parent_type->getPointerElementType();
      }

      // check the src type in GEP inst is equal to parent_type (GET FROM)
      // check if the offset is equal
      int field_offset = (*formal_in_TI)->getFieldId();
      // if (field_idx+1 == relative_idx && GEPResTy == TreeNodeTy && match) {
      bool srcTypeMatch = GEPSrcTy == parent_type;
      bool resTypeMatch = GEPResTy == TreeNodeTy;
      bool offsetMatch = field_idx == field_offset;
      if (offsetMatch && resTypeMatch && srcTypeMatch) {
        // also add this GEP to the list of GEP that directly access parameter.
        treeNodeGepPairs.push_back(std::make_pair(*formal_in_TI, GEPInstW));
        // errs() << argW->getArg()->getParent()->getName() <<
        // tree<InstructionWrapper*>::depth(formal_in_TI) << "\n"; errs() <<
        // "Pushing: " << *GEP << "\n"; errs() << "Size: " <<
        // treeNodeGepPairs.size() << "\n";
        return treeNodeGepPairs;
      }
    }
  }

  return treeNodeGepPairs;
}

// determine whether the GEP instruction is used by a load/store instruction
// later.
int pdg::ProgramDependencyGraph::getGEPOpType(InstructionWrapper *GEPInstW) {
  // by default, set the GEP as not access
  int gepAccessType = NOACCESS;
  std::queue<InstructionWrapper *> instWQueue;
  std::set<InstructionWrapper *> seenInstW;
  instWQueue.push(GEPInstW);
  seenInstW.insert(GEPInstW);

  // actually, here only need to search one depth
  // find DDG relevant insts
  // also search through parameter tree node.
  // Some field may be read into a pointer first and then passed to a function
  if (GEPInstW->getInstruction() == nullptr) {
    return 0;
  }

  while (!instWQueue.empty()) {
    InstructionWrapper *instW = instWQueue.front();
    instWQueue.pop();
    llvm::Instruction *tmpInst = instW->getInstruction();
    // if (tmpInst != nullptr) {
    //     errs() << *instW->getInstruction() << "\n";
    // }
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList =
        dataDNode->getDependencyList();

    for (auto dependencyPair : dataDList) {
      InstructionWrapper *DNodeW2 =
          const_cast<InstructionWrapper *>(dependencyPair.first->getData());
      // avoid repreated instruction node.
      if (seenInstW.find(DNodeW2) != seenInstW.end()) {
        continue;
      }

      instWQueue.push(DNodeW2);
      seenInstW.insert(DNodeW2);

      if (DNodeW2->getInstruction() == nullptr) {
        DEBUG(dbgs() << "Type Node"
                     << "\n");
        continue;
      }

      if (isa<llvm::LoadInst>(DNodeW2->getInstruction())) {
        gepAccessType = pdg::AccessType::READ_FIELD;
      }

      if (isa<llvm::StoreInst>(DNodeW2->getInstruction())) {
        gepAccessType = pdg::AccessType::WRITE_FIELD;
        return gepAccessType;
      }
    }
  }

  return gepAccessType;
}

void pdg::ProgramDependencyGraph::linkTypeNodeWithGEPInst(
    ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator formal_in_TI) {

  std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>>
      treeNodeWithCorrespondGEP =
          getParameterTreeNodeWithCorrespondGEP(argW, formal_in_TI);

  for (auto node_gep_pair : treeNodeWithCorrespondGEP) {
    InstructionWrapper *typeNodeW = node_gep_pair.first;
    InstructionWrapper *gepNodeW = node_gep_pair.second;
    PDG->addDependency(typeNodeW, gepNodeW, STRUCT_FIELDS);
    typeNodeW->setVisited(true);

    // collecting R/W information
    // 1. test if is a GEP instruction
    if (!isa<llvm::GetElementPtrInst>(gepNodeW->getInstruction())) {
      continue;
    }

    // int gepAccessType = getGEPOpType(gepNodeW);
    // int old_type = typeNodeW->getAccessType();

    // if (gepAccessType < old_type) {
    //     // if access info not changed, continue processing
    //     continue;
    // }

    // typeNodeW->setAccessType(gepAccessType);
    // need to update all parent node's information
    // auto parentIter = tree<InstructionWrapper *>::parent(formal_in_TI);
    // while (parentIter != nullptr) {
    //   InstructionWrapper *parentInstW = (*parentIter);
    //   parentInstW->setAccessType(gepAccessType);
    //   parentIter = tree<InstructionWrapper *>::parent(parentIter);
    // }
  }
}

void pdg::ProgramDependencyGraph::connectFunctionAndFormalTrees(
    llvm::Function *callee) {
  if (callee == nullptr) {
    return;
  }

  for (vector<ArgumentWrapper *>::iterator
           argI = funcMap[callee]->getArgWList().begin(),
           argE = funcMap[callee]->getArgWList().end();
       argI != argE; ++argI) {
    InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
    InstructionWrapper *formal_outW =
        *(*argI)->getTree(FORMAL_OUT_TREE).begin();

    // connect Function's EntryNode with formal in/out tree roots
    PDG->addDependency(funcMap[callee]->getEntry(), formal_inW, PARAMETER);
    PDG->addDependency(funcMap[callee]->getEntry(), formal_outW, PARAMETER);

    // get R/W information for the argument
    // if (isBasicTypeOrPtr((*argI)->getArg()->getType())) {
    //     int _access_type = getArgOpType((*argI)->getArg());
    //     formal_inW->setAccessType(_access_type);
    // }

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

      // link specific field with GEP if there is any
      linkTypeNodeWithGEPInst(*argI, formal_in_TI);
      // connect formal-in-tree type nodes with storeinst in call_func
      if ((*argI)->getTree(FORMAL_IN_TREE).depth(formal_in_TI) == 0) {
        for (auto userIter = (*argI)->getArg()->user_begin();
             userIter != (*argI)->getArg()->user_end(); ++userIter) {
          if (llvm::Instruction *tmpInst = dyn_cast<Instruction>(*userIter)) {
            PDG->addDependency(*instnodes.find(*formal_in_TI), instMap[tmpInst],
                               DATA_GENERAL);
            (*formal_in_TI)->setVisited(true);
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

      if ((*formal_out_TI)->getFieldType() != nullptr) {
        std::list<LoadInst *>::iterator LI =
            funcMap[callee]->getLoadInstList().begin();
        std::list<LoadInst *>::iterator LE =
            funcMap[callee]->getLoadInstList().end();

        for (; LI != LE; ++LI) {
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
    }     // end for (tree formal_in_TI...)
  }       // end for arg iteration...
}

bool pdg::ProgramDependencyGraph::isFuncTypeMatch(
    llvm::FunctionType *funcTy, llvm::FunctionType *indirectFuncCallTy) {
  if (funcTy->getNumParams() != indirectFuncCallTy->getNumParams()) {
    return false;
  }

  if (funcTy->getReturnType() != indirectFuncCallTy->getReturnType()) {
    return false;
  }

  unsigned param_len = funcTy->getNumParams();
  for (unsigned i = 0; i < param_len; ++i) {
    if (funcTy->getParamType(i) != indirectFuncCallTy->getParamType(i)) {
      return false;
    }
  }

  return true;
}

std::vector<llvm::Function *>
pdg::ProgramDependencyGraph::collectIndirectCallCandidates(
    FunctionType *funcType) {
  std::vector<llvm::Function *> indirectCallList;
  std::map<const llvm::Function *, FunctionWrapper *>::iterator FI =
      funcMap.begin();
  std::map<const llvm::Function *, FunctionWrapper *>::iterator FE =
      funcMap.end();
  for (; FI != FE; ++FI) {
    llvm::Function *curFunc = const_cast<llvm::Function *>((*FI).first);
    // get Function type
    llvm::FunctionType *curFuncType = curFunc->getFunctionType();

    if (curFunc->getFunctionType() == funcType &&
        curFunc->getName() != "main") {
      DEBUG(dbgs() << curFunc->getName() << " function pointer! \n");
    }

    if (curFunc->getName() == "main") {
      continue;
    }

    // compare the indirect call function type with each function
    if (isFuncTypeMatch(curFuncType, funcType)) {
      //    errs() << "Current Function Name: " << curFunc->getName() << "\n";
      //    errs() << "Current Func Type: " << *curFuncType << "\n";
      //    errs() << "CmpFuncType: " << *funcType << "\n";
      indirectCallList.push_back(curFunc);
    }
  }

  return indirectCallList;
}

void pdg::ProgramDependencyGraph::connectAllPossibleFunctions(
    CallInst *CI, std::vector<llvm::Function *> indirect_call_candidates) {
  InstructionWrapper *CInstW = instMap[CI];
  for (llvm::Function *func : indirect_call_candidates) {
    connectCallerAndCallee(CInstW, func);
  }
}

void pdg::ProgramDependencyGraph::collectGlobalInstList() {
  // Get the Module's list of global variable and function identifiers
  DEBUG(dbgs() << "======Global List: ======\n");
  for (llvm::Module::global_iterator globalIt = module->global_begin();
       globalIt != module->global_end(); ++globalIt) {
    InstructionWrapper *globalW =
        new InstructionWrapper(nullptr, nullptr, &(*globalIt), GLOBAL_VALUE);
    instnodes.insert(globalW);
    globalList.insert(globalW);

    // find all global pointer values and insert them into a list
    if (globalW->getValue()->getType()->getContainedType(0)->isPointerTy()) {
      gp_list.push_back(globalW);
    }
  }
}

void pdg::ProgramDependencyGraph::categorizeInstInFunc(llvm::Function *func) {
  // find all Load/Store instructions for each F, insert to F's storeInstList
  // and loadInstList
  for (inst_iterator I = inst_begin(func), IE = inst_end(func); I != IE; ++I) {
    Instruction *pInstruction = dyn_cast<Instruction>(&*I);

    if (isa<StoreInst>(pInstruction)) {
      StoreInst *SI = dyn_cast<StoreInst>(pInstruction);
      funcMap[func]->getStoreInstList().push_back(SI);
      funcMap[func]->getPtrSet().insert(SI->getPointerOperand());
      if (SI->getValueOperand()->getType()->isPointerTy()) {
        funcMap[func]->getPtrSet().insert(SI->getValueOperand());
      }
    }
    if (isa<LoadInst>(pInstruction)) {
      LoadInst *LI = dyn_cast<LoadInst>(pInstruction);
      funcMap[func]->getLoadInstList().push_back(LI);
      funcMap[func]->getPtrSet().insert(LI->getPointerOperand());
    }

    if (isa<ReturnInst>(pInstruction))
      funcMap[func]->getReturnInstList().push_back(pInstruction);

    if (isa<CallInst>(pInstruction)) {
      funcMap[func]->getCallInstList().push_back(
          dyn_cast<CallInst>(pInstruction));
    }
  }
}

bool pdg::ProgramDependencyGraph::processingCallInst(
    InstructionWrapper *instW) {
  llvm::Instruction *pInstruction = instW->getInstruction();
  if (pInstruction != nullptr && instW->getType() == INST &&
      isa<CallInst>(pInstruction) && !instW->getVisited()) {
    // InstructionWrapper *CallInstW = instW;
    CallInst *CI = dyn_cast<CallInst>(pInstruction);
    Function *callee = CI->getCalledFunction();

    if (callee == nullptr) {
      // indirect function call
      // get func type for indirect call inst
      Type *t = CI->getCalledValue()->getType();
      FunctionType *funcTy =
          cast<FunctionType>(cast<PointerType>(t)->getElementType());
      // collect all possible function with same function signature
      std::vector<llvm::Function *> indirect_call_candidates =
          collectIndirectCallCandidates(funcTy);
      if (indirect_call_candidates.size() == 0) {
        // errs() << "Indirect Call Parent function: " <<
        // pInstruction->getParent()->getName() << "\n"; errs() << *pInstruction
        // << "\n";
        errs() << "cannot find possible indirect call candidates.."
               << "\n";
        return false;
      }
      CallWrapper *callW = new CallWrapper(CI, indirect_call_candidates);
      callMap[CI] = callW;
      errs() << "indirect call, called Type t = " << *t << "\n";
      // build formal tree for all candidiates.
      for (llvm::Function *func : indirect_call_candidates) {
        if (func->isDeclaration()) {
          continue;
        }

        if (func->arg_empty()) {
          continue;
        }

        if (funcMap[func]->hasTrees()) {
          continue;
        }

        buildFormalParameterTrees(func);
        drawFormalParameterTree(func, FORMAL_IN_TREE);
        drawFormalParameterTree(func, FORMAL_OUT_TREE);
        connectFunctionAndFormalTrees(func);
        funcMap[func]->setTreeFlag(true);
      }
      buildActualParameterTrees(CI);
      drawActualParameterTree(CI, ACTUAL_IN_TREE);
      drawActualParameterTree(CI, ACTUAL_OUT_TREE);
      // connect actual tree with all possible candidaites.
      connectAllPossibleFunctions(CI, indirect_call_candidates);

      DEBUG(dbgs() << "call_func = null: " << *CI << "\n");
      DEBUG(dbgs() << "indirect call, called Type t = " << *t << "\n");
      DEBUG(dbgs() << "after cast<FunctionType>, ft = " << *funcTy << "\n");
      return true;
    }

    // handle intrinsic functions
    if (callee->isIntrinsic()) {
      if (callee->getIntrinsicID() == Intrinsic::var_annotation) {
        Value *v = CI->getArgOperand(0);
        DEBUG(dbgs() << "Intrinsic var_annotation: " << *v << "\n");
        if (isa<BitCastInst>(v)) {
          Instruction *tempI = dyn_cast<Instruction>(v);
          DEBUG(dbgs() << "******** BitInst opcode: " << *tempI
                       << "BitCast \n");

          for (llvm::Use &U : tempI->operands()) {
            Value *v_for_cast = U.get();
          }
        }
      }
      return false;
    }

    // special cases done, common function
    CallWrapper *callW = new CallWrapper(CI);
    callMap[CI] = callW;

    if (!callee->isDeclaration()) {
      if (!callee->arg_empty()) {
        if (funcMap[callee]->hasTrees() != true) {
          DEBUG(dbgs() << "Buiding parameter tree"
                       << "\n");
          buildFormalParameterTrees(callee);
          drawFormalParameterTree(callee, FORMAL_IN_TREE);
          drawFormalParameterTree(callee, FORMAL_OUT_TREE);
          connectFunctionAndFormalTrees(callee);
          funcMap[callee]->setTreeFlag(true);
        }
        buildActualParameterTrees(CI);
        drawActualParameterTree(CI, ACTUAL_IN_TREE);
        drawActualParameterTree(CI, ACTUAL_OUT_TREE);
      } // end if !callee

      if (0 == connectCallerAndCallee(instW, callee)) {
        instW->setVisited(true);
      }

      // link typenode inst with argument inst
      std::vector<ArgumentWrapper *> argList = funcMap[callee]->getArgWList();
      for (ArgumentWrapper *argW : argList) {
        tree<InstructionWrapper *>::iterator TI =
            argW->getTree(FORMAL_IN_TREE).begin();
        tree<InstructionWrapper *>::iterator TE =
            argW->getTree(FORMAL_IN_TREE).end();
        for (; TI != TE; ++TI) {
          if (PDG->depends(instW, *TI)) {
            PDG->addDependency(instW, *TI, STRUCT_FIELDS);
          }
        }
      }
    }
  }
  return true;
}

bool pdg::ProgramDependencyGraph::addNodeDependencies(
    InstructionWrapper *instW1) {
  // processing Global instruction
  if (instW1->getInstruction() != nullptr) {
    if (llvm::LoadInst *LDInst =
            dyn_cast<llvm::LoadInst>(instW1->getInstruction())) {
      for (auto GlobalInstW : globalList) {
        // iterate users of the global value
        for (User *U : GlobalInstW->getValue()->users()) {
          if (Instruction *userInst = dyn_cast<Instruction>(U)) {
            InstructionWrapper *userInstW = instMap[userInst];
            PDG->addDependency(GlobalInstW, userInstW, GLOBAL_VALUE);
          }
        }
      }
    }
  }

  // processing ddg nodes
  pdg::DependencyNode<InstructionWrapper> *dataDNode =
      ddg->DDG->getNodeByData(instW1);
  pdg::DependencyNode<InstructionWrapper>::DependencyLinkList dataDList =
      dataDNode->getDependencyList();
  for (auto dependencyPair : dataDList) {
    InstructionWrapper *DNodeW2 =
        const_cast<InstructionWrapper *>(dependencyPair.first->getData());
    PDG->addDependency(instW1, DNodeW2, dependencyPair.second);
  }

  // Control Dep entry
  if (instW1->getType() == ENTRY) {
    llvm::Function *parentFunc = instW1->getFunction();
    for (InstructionWrapper *instW2 : funcInstWList[parentFunc]) {
      PDG->addDependency(instW1, instW2, CONTROL);
    }
  }

  return true;
}

bool pdg::ProgramDependencyGraph::runOnModule(Module &M) {

  std::set<std::string> funcList = {"consume_skb"};
  //   std::set<std::string> funcList = {
  //       "rtnl_link_unregister",
  //       "ether_setup",
  //       "free_netdev",
  //       "eth_mac_addr",
  //       "eth_validate_addr",
  //       "__alloc_percpu",
  //       "free_percpu",
  //       "consume_skb",
  //       "netif_carrier_on",
  //       "netif_carrier_off",
  //       "get_random_bytes",
  //       "rtnl_lock",
  //       "__rtnl_link_register",
  //       "_cond_resched",
  //       "__rtnl_link_unregister",
  //       "rtnl_unlock",
  //       "alloc_netdev_mqs",
  //       "register_netdevice"
  //   };

  DEBUG(dbgs() << "ProgramDependencyGraph::runOnModule" << '\n');
  module = &M;

  constructFuncMap(M);
  collectGlobalInstList();
  int funcs = 0;
  // process a module function by function
  for (Module::iterator FF = M.begin(), E = M.end(); FF != E; ++FF) {
    DEBUG(dbgs() << "Module Size: " << M.size() << "\n");
    Function *F = dyn_cast<Function>(FF);

    // #ifdef TEST_IDL
    if (funcList.find(F->getName()) == funcList.end()) {
      continue;
    }
    //#endif
    if (F->isDeclaration()) {
      DEBUG(dbgs() << (*F).getName() << " is defined outside!"
                   << "\n");
      continue;
    }

    funcs++; // label this author-defined function

    DEBUG(dbgs() << "PDG " << 1.0 * funcs / M.getFunctionList().size() * 100
                 << "% completed\n");

    categorizeInstInFunc(F);

    cdg = &getAnalysis<ControlDependencyGraph>(*F);
    ddg = &getAnalysis<DataDependencyGraph>(*F);

    errs() << "Finsh getting cdg and ddg\n";
    // for each instruction, copy all ddg and cdg dependencies
    for (std::set<InstructionWrapper *>::iterator nodeIt =
             funcInstWList[F].begin();
         nodeIt != funcInstWList[F].end(); ++nodeIt) {
      InstructionWrapper *instW1 = *nodeIt;
      if (!addNodeDependencies(instW1)) {
        continue;
      }
    }

    if (!funcMap[F]->hasTrees()) {
      buildFormalParameterTrees(F);
      drawFormalParameterTree(F, FORMAL_IN_TREE);
      drawFormalParameterTree(F, FORMAL_OUT_TREE);
      connectFunctionAndFormalTrees(F);
      funcMap[F]->setTreeFlag(true);
    }
  } // end for(Module...

  for (Module::iterator FF = M.begin(); FF != M.end(); ++FF) {
    llvm::Function *F = dyn_cast<llvm::Function>(FF);
    if (F->isDeclaration()) {
      continue;
    }
    for (llvm::CallInst *callInst : funcMap[F]->getCallInstList()) {
      if (!processingCallInst(instMap[callInst])) {
        continue;
      }
    }
  }

  DEBUG(dbgs() << "\n\n PDG construction completed! ^_^\n\n");
  DEBUG(dbgs() << "funcs = " << funcs << "\n");
  DEBUG(dbgs() << "+++++++++++++++++++++++++++++++++++++++++++++\n");

  for (Module::iterator FF = M.begin(); FF != M.end(); ++FF) {
    llvm::Function *F = dyn_cast<llvm::Function>(FF);

    if (funcList.find(F->getName()) == funcList.end()) {
      continue;
    }

    if (F->isDeclaration()) {
      continue;
    }
    // errs() << "Getting intra function for func: " << F->getName() << "\n";
    // after building all dependencies, also collect read/write information in
    // function
    getIntraFuncReadWriteInfo(F);
  }

  // after we get intra function dependencies, we can link use inter function
  // use info
  for (Module::iterator FF = M.begin(); FF != M.end(); ++FF) {
    llvm::Function *F = dyn_cast<llvm::Function>(FF);

    if (funcList.find(F->getName()) == funcList.end()) {
      continue;
    }

    if (F->isDeclaration()) {
      continue;
    }

    getInterFuncReadWriteInfo(F);
  }

  errs() << "Start printing arg use information\n";
  printParameterTreeForFunc(M, funcList);

  cleanupGlobalVars();
  return false;
}

// functions for computing R/W info for each typenode.
int pdg::ProgramDependencyGraph::getArgType(llvm::Argument *arg) {
  llvm::Type *type = arg->getType();

  if (!isa<PointerType>(type)) {
    if (type->isSingleValueType()) {
      return SINGLE_VALUE_TYPE;
    }

    if (type->isAggregateType()) {
      return AGGREGATE_VALUE_TYPE;
    }
  }

  if (PointerType *pt = dyn_cast<PointerType>(type)) {
    type = pt->getElementType();
    if (type->isSingleValueType()) {
      return SINGLE_VALUE_PTR_TYPE;
    }

    if (type->isAggregateType()) {
      return AGGREGATE_VALUE_PTR_TYPE;
    }
  }

  return UNKNOWN_TYPE;
}

unsigned
pdg::ProgramDependencyGraph::getStructElementNum(llvm::Module &M,
                                                 llvm::Type *curNodeTy) {
  if (curNodeTy == nullptr) {
    return -1;
  }
  DataLayout DL = M.getDataLayout();
  // the parent must be struct pointer type or struct type.
  if (PointerType *pt = dyn_cast<PointerType>(curNodeTy)) {

    if (curNodeTy->isPointerTy()) {
      if (!pt->getElementType()->isStructTy()) {
        return -1;
      }
    } else {
      if (!curNodeTy->isStructTy()) {
        return -1;
      }
    }

    StructType *st = nullptr;

    if (curNodeTy->isPointerTy()) {
      // cast the pointer type to struct type.(we know it's a struct pointer at
      // this point)
      st = dyn_cast<StructType>(pt->getElementType());
    } else {
      st = dyn_cast<StructType>(curNodeTy);
    }
    return st->getNumElements();
  }
  return -1;
}

const StructLayout *
pdg::ProgramDependencyGraph::getStructLayout(llvm::Module &M,
                                             InstructionWrapper *curTyNode) {
  if (curTyNode == nullptr) {
    return nullptr;
  }

  DataLayout DL = M.getDataLayout();
  llvm::Type *curNodeTy = curTyNode->getFieldType();

  // the parent must be struct pointer type or struct type.
  PointerType *pt = dyn_cast<PointerType>(curNodeTy);
  if (curNodeTy->isPointerTy()) {
    if (!pt->getElementType()->isStructTy()) {
      return nullptr;
    }
  } else {
    if (!curNodeTy->isStructTy()) {
      return nullptr;
    }
  }

  StructType *st = nullptr;

  if (curNodeTy->isPointerTy()) {
    // cast the pointer type to struct type.(we know it's a struct pointer at
    // this point)
    st = dyn_cast<StructType>(pt->getElementType());
  } else {
    st = dyn_cast<StructType>(curNodeTy);
  }

  const StructLayout *StL = DL.getStructLayout(st);
  return StL;
}

void pdg::ProgramDependencyGraph::printParameterTreeForFunc(
    llvm::Module &M, std::set<std::string> funcList) {
  // typedef std::map<unsigned, std::pair<std::string, DIType *>> offsetNames;
  std::vector<std::string> accessTypeName = {"No Access", "Read", "Write"};

  std::map<Function *, std::map<unsigned, offsetNames>> funcArgOffsetNames = getAnalysis<DSAGenerator>().getFuncArgOffsetNames();

  for (llvm::Function &func : M) {
    // #ifdef TEST_IDL
    if (funcList.find(func.getName()) == funcList.end()) {
      continue;
    }
    // #endif
    if (func.isDeclaration()) {
      continue;
    }
    // get offset - name pair
    std::map<unsigned, offsetNames> argsOffsetNames = funcArgOffsetNames[&func];
    // traverse parameter tree, print access information and name for each field
    FunctionWrapper *funcW = funcMap[&func];

    errs() << "\n [For function: " << func.getName() << "] \n";
    for (auto argW : funcW->getArgWList()) {
      errs() << "Arg use information for arg no: " << argW->getArg()->getArgNo() << "\n";
      errs() << "Size of argW: " << argW->getTree(FORMAL_IN_TREE).size() << "\n";
      offsetNames argOffsetNames = argsOffsetNames[argW->getArg()->getArgNo()];

      auto treeI = argW->getTree(FORMAL_IN_TREE).begin();
      auto treeIE = argW->getTree(FORMAL_IN_TREE).end();
      int visit_order = 0;
      int shift = 0;
      for (; treeI != treeIE; ++treeI) {
        InstructionWrapper *curTyNode = *treeI;
        llvm::Type *parentTy = curTyNode->getParentType();
        llvm::Type *curType = curTyNode->getFieldType();

        errs() << "visit order: " << visit_order + shift<< "\n";
        errs() << "Num of child: " << tree<InstructionWrapper *>::number_of_children(treeI) << "\n";
        errs() << "Field type: " << curType->getTypeID() << "\n";
        if (parentTy == nullptr) {
          errs() << "** Root type node **" << "\n";
          errs() << "Field name: " << argOffsetNames[visit_order + shift].first << "\n";
          errs() << "Access Type: " << accessTypeName[curTyNode->getAccessType()] << "\n";
          errs() << ".............................................\n";
          visit_order += 1;
          continue;
        }


        if (argOffsetNames.find(visit_order) == argOffsetNames.end()) {
          visit_order += 1;
          errs() << "Access Type: " << accessTypeName[curTyNode->getAccessType()] << "\n";
          continue;
        }

        // handle bit fields packing
        if (curType->isIntegerTy() && argOffsetNames[visit_order + shift].second->isBitField()) {
          int bitFieldSum = 0;
          unsigned integerBitWidth = curType->getIntegerBitWidth();
          errs() << "sub field name: packed bit field.\n";
          errs() << "Access Type: " << accessTypeName[curTyNode->getAccessType()] << "\n";
          errs() << "..................................................\n";
          while (integerBitWidth > 0) {
            if (!argOffsetNames[visit_order + shift].second->isBitField()) {
              break;
            }
            integerBitWidth -= argOffsetNames[visit_order + shift].second->getSizeInBits();
            shift += 1;
          }
          continue;
        }

        errs() << "sub field name: " << argOffsetNames[visit_order + shift].first << "\n";
        errs() << "Access Type: " << accessTypeName[curTyNode->getAccessType()] << "\n";
        errs() << "Field ID: " << curTyNode->getFieldId() << "\n";
        errs() << "..................................................\n";
        visit_order += 1;
      }
      errs() << (argOffsetNames[0].first) << "\n";
      errs() << (argOffsetNames[1].first) << "\n";
      errs() << "..................................................\n";
    }
  }
}

std::set<pdg::InstructionWrapper *>
pdg::ProgramDependencyGraph::getAllRelevantGEP(llvm::Argument *arg) {
  std::vector<Instruction *> initialStoreInsts = getArgStoreInsts(arg);
  std::set<InstructionWrapper *> relevantGEPs;
  std::queue<InstructionWrapper *> instWQ;

  for (Instruction *storeInst : initialStoreInsts) {
    DependencyNode<InstructionWrapper> *dataDNode =
        PDG->getNodeByData(instMap[storeInst]);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList =
        dataDNode->getDependencyList();
    for (auto depPair : dataDList) {
      int depType = depPair.second;
      if (depType == DATA_ALIAS) {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        instWQ.push(depInstW);
        if (isa<GetElementPtrInst>(depInstW->getInstruction())) {
          relevantGEPs.insert(depInstW);
        }
      }
    }
  }

  while (!instWQ.empty()) {
    InstructionWrapper *instW = instWQ.front();
    instWQ.pop();

    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    for (auto depPair : dataDList) {
      int depType = depPair.second;
      if (depType == DATA_DEF_USE) {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        if (depInstW->getInstruction() != nullptr && isa<GetElementPtrInst>(depInstW->getInstruction())) {
          relevantGEPs.insert(depInstW);
          instWQ.push(depInstW);
        }
      }
    }
  }

  return relevantGEPs;
}

void pdg::ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<ControlDependencyGraph>();
  AU.addRequired<DataDependencyGraph>();
  AU.addRequired<DSAGenerator>();
  // AU.addRequired<llvm::CallGraphWrapperPass>();
  AU.setPreservesAll();
}

void pdg::ProgramDependencyGraph::print(llvm::raw_ostream &OS,
                                        const llvm::Module *) const {
  PDG->print(OS, (getPassName().data()));
}

static RegisterPass<pdg::ProgramDependencyGraph>
    PDG("pdg", "Program Dependency Graph Construction", false, true);