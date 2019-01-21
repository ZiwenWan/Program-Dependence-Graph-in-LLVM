#include "ProgramDependencyGraph.hpp"

using namespace llvm;

char pdg::ProgramDependencyGraph::ID = 0;

void pdg::ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<ControlDependencyGraph>();
  AU.addRequired<DataDependencyGraph>();
  AU.setPreservesAll();
}

int pdg::EXPAND_LEVEL;
int pdg::USEDEBUGINFO;
llvm::cl::opt<int> expandLevel("l", llvm::cl::desc("Parameter tree expand level"), llvm::cl::value_desc("level"));
llvm::cl::opt<int> useDebugInfo("d", llvm::cl::desc("use debug information"), llvm::cl::value_desc("debugInfo"));

bool pdg::ProgramDependencyGraph::runOnModule(Module &M)
{ 
  if (!expandLevel)
    expandLevel = 4;
  if (!useDebugInfo)
    useDebugInfo = 0;
  EXPAND_LEVEL = expandLevel;
  USEDEBUGINFO = useDebugInfo;

  errs() << "Expand level " << EXPAND_LEVEL << "\n";
  errs() << "Using Debug Info " << USEDEBUGINFO << "\n";
  module = &M;
  auto &pdgUtils = PDGUtils::getInstance();
  pdgUtils.constructFuncMap(M);
  pdgUtils.collectGlobalInsts(M);
  int user_def_func_num = 0;
  // copy dependencies from DDG/CDG to PDG
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function *Func = dyn_cast<Function>(FI);
    if (Func->isDeclaration())
      continue;

    user_def_func_num++; // count function has definition as user defined
    pdgUtils.categorizeInstInFunc(*Func);
    cdg = &getAnalysis<ControlDependencyGraph>(*Func);
    ddg = &getAnalysis<DataDependencyGraph>(*Func);

    for (InstructionWrapper *instW : pdgUtils.getFuncInstWMap()[Func])
    {
      addNodeDependencies(instW);
    }

    if (!pdgUtils.getFuncMap()[Func]->hasTrees())
    {
      buildFormalTreeForFunc(Func);
      drawFormalParameterTree(Func, TreeType::FORMAL_IN_TREE);
      drawFormalParameterTree(Func, TreeType::FORMAL_OUT_TREE);
      connectFunctionAndFormalTrees(Func);
      pdgUtils.getFuncMap()[Func]->setTreeFlag(true);
    }
  }

  // start process CallInst
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function *Func = dyn_cast<Function>(FI);
    if (Func->isDeclaration())
      continue;

    auto callInstsList = pdgUtils.getFuncMap()[Func]->getCallInstList();
    for (CallInst *inst : callInstsList)
    {
      if (!processCallInst(pdgUtils.getInstMap()[inst]))
        continue;
    }
  }

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    if (FI->isDeclaration())
      continue;

    auto argWList = pdgUtils.getFuncMap()[&*FI]->getArgWList();
    for (auto argW : argWList) {
      errs() << FI->getName() << ": " << argW->getTree(TreeType::FORMAL_IN_TREE).size() << "\n";
    }
  }
  return false;
}

bool pdg::ProgramDependencyGraph::processIndirectCallInst(CallInst *CI, InstructionWrapper *instW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Type *t = CI->getCalledValue()->getType();
  FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
  // collect all possible function with same function signature
  std::vector<Function *> indirect_call_candidates = collectIndirectCallCandidates(funcTy);
  if (indirect_call_candidates.size() == 0)
  {
    errs() << "cannot find possible indirect call candidates.." << "\n";
    return false;
  }
  CallWrapper *callW = new CallWrapper(CI, indirect_call_candidates);
  pdgUtils.getCallMap()[CI] = callW;
  errs() << "indirect call, called Type t = " << *t << "\n";
  // build formal tree for all candidiates.
  for (Function *indirect_called_func : indirect_call_candidates)
  {
    if (indirect_called_func->isDeclaration())
      continue;
    if (indirect_called_func->arg_empty())
      continue;
    if (pdgUtils.getFuncMap()[indirect_called_func]->hasTrees())
      continue;
    errs() << "Building tree for indirect func: " << indirect_called_func->getName() << "\n";
    buildFormalTreeForFunc(indirect_called_func);
    drawFormalParameterTree(indirect_called_func, TreeType::FORMAL_IN_TREE);
    drawFormalParameterTree(indirect_called_func, TreeType::FORMAL_OUT_TREE);
    connectFunctionAndFormalTrees(indirect_called_func);
    pdgUtils.getFuncMap()[indirect_called_func]->setTreeFlag(true);
  }
  buildActualParameterTrees(CI);
  drawActualParameterTree(CI, TreeType::ACTUAL_IN_TREE);
  drawActualParameterTree(CI, TreeType::ACTUAL_OUT_TREE);
  // connect actual tree with all possible candidaites.
  if (connectAllPossibleFunctions(CI, indirect_call_candidates))
  {
    instW->setVisited(true);
  }
  return true;
}

bool pdg::ProgramDependencyGraph::processCallInst(InstructionWrapper *instW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  llvm::Instruction *inst = instW->getInstruction();
  if (inst != nullptr && isa<CallInst>(inst) && !instW->getVisited())
  {
    CallInst *CI = dyn_cast<CallInst>(inst);
    Function *callee = CI->getCalledFunction();

    if (callee == nullptr)
      return processIndirectCallInst(CI, instW); // indirect function call get func type for indirect call inst

    // handle intrinsic functions
    if (callee->isIntrinsic())
      return false;

    // special cases done, common function
    CallWrapper *callW = new CallWrapper(CI);
    pdgUtils.getCallMap()[CI] = callW;
    if (!callee->isDeclaration())
    {
      if (!callee->arg_empty())
      {
        if (pdgUtils.getFuncMap()[callee]->hasTrees() != true)
        {
          buildFormalTreeForFunc(callee);
          drawFormalParameterTree(callee, TreeType::FORMAL_IN_TREE);
          drawFormalParameterTree(callee, TreeType::FORMAL_OUT_TREE);
          connectFunctionAndFormalTrees(callee);
          pdgUtils.getFuncMap()[callee]->setTreeFlag(true);
        }
        buildActualParameterTrees(CI);
        drawActualParameterTree(CI, TreeType::ACTUAL_IN_TREE);
        drawActualParameterTree(CI, TreeType::ACTUAL_OUT_TREE);
      } // end if !callee

      connectCallerAndCallee(instW, callee);
      // link typenode inst with argument inst
      std::vector<ArgumentWrapper *> argList = pdgUtils.getFuncMap()[callee]->getArgWList();
      for (ArgumentWrapper *argW : argList)
      {
        tree<InstructionWrapper *>::iterator TI = argW->getTree(TreeType::FORMAL_IN_TREE).begin();
        tree<InstructionWrapper *>::iterator TE = argW->getTree(TreeType::FORMAL_IN_TREE).end();
        for (; TI != TE; ++TI)
        {
          if (PDG->isDepends(instW, *TI))
          {
            PDG->addDependency(instW, *TI, DependencyType::STRUCT_FIELDS);
          }
        }
      }
    }
  }
  return true;
}

void pdg::ProgramDependencyGraph::addNodeDependencies(InstructionWrapper *instW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  // processing Global instruction
  if (instW->getInstruction() != nullptr)
  {
    if (LoadInst *LDInst = dyn_cast<LoadInst>(instW->getInstruction()))
    {
      for (auto GlobalInstW : pdgUtils.getGlobalInstsSet())
      {
        // iterate users of the global value
        for (User *U : GlobalInstW->getValue()->users())
        {
          if (Instruction *userInst = dyn_cast<Instruction>(U))
          {
            InstructionWrapper *userInstW = pdgUtils.getInstMap()[userInst];
            PDG->addDependency(GlobalInstW, userInstW, DependencyType::GLOBAL_DEP);
          }
        }
      }
    }
  }

  // copy data dependency
  // auto *dataDNode = ddg->getNodeByData(instW);
  // auto dataDList = dataDNode->getDependencyList();
  auto dataDList = ddg->getNodeDepList(instW->getInstruction());
  for (auto dependencyPair : dataDList)
  {
    InstructionWrapper *DNodeW2 = const_cast<InstructionWrapper *>(dependencyPair.first->getData());
    PDG->addDependency(instW, DNodeW2, dependencyPair.second);
  }

  // copy control dependency
  if (instW->getGraphNodeType() == GraphNodeType::ENTRY)
  {
    Function *parentFunc = instW->getFunction();
    for (InstructionWrapper *instW2 : pdgUtils.getFuncInstWMap()[parentFunc])
    {
      PDG->addDependency(instW, instW2, DependencyType::CONTROL);
    }
  }
}

void pdg::ProgramDependencyGraph::buildFormalTreeForFunc(Function *Func)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (auto argW : pdgUtils.getFuncMap()[Func]->getArgWList())
  {
    // build formal in tree first
    buildFormalTreeForArg(*argW->getArg(), TreeType::FORMAL_IN_TREE);
    // then, copy formal in tree content to formal out tree
    argW->copyTree(argW->getTree(TreeType::FORMAL_IN_TREE), TreeType::FORMAL_OUT_TREE);
  }
  pdgUtils.getFuncMap()[Func]->setTreeFlag(true);
}

void pdg::ProgramDependencyGraph::buildFormalTreeForArg(Argument &arg, TreeType treeTy)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Function *Func = arg.getParent();
  try
  {
    DIType *argDIType = DIUtils::getArgDIType(arg);
    InstructionWrapper *treeTyW = new TreeTypeWrapper(arg.getParent(), GraphNodeType::FORMAL_IN, &arg, arg.getType(), nullptr, 0, argDIType);
    pdgUtils.getFuncInstWMap()[Func].insert(treeTyW);
    //find the right arg, and set tree root
    ArgumentWrapper *argW = pdgUtils.getFuncMap()[Func]->getArgWByArg(arg);
    auto treeRoot = argW->getTree(treeTy).set_head(treeTyW);
    if (argW->getTree(treeTy).size() == 0) {
      std::string err_msg = "Function " + Func->getName().str() + " arg " +
                            DIUtils::getArgName(arg) +
                            " has param tree of size 0. Abort...";
      errs() << err_msg << "\n" ;
      exit(0);
    }
    std::string Str;
    raw_string_ostream OS(Str);
    //FILE*, bypass, no need to buildTypeTree
    if ("%struct._IO_FILE*" == OS.str() || "%struct._IO_marker*" == OS.str())
    {
      errs() << "OS.str() = " << OS.str() << " FILE* appears, stop buildTypeTree\n";
    }
    else if (treeTyW->getTreeNodeType()->isPointerTy() && treeTyW->getTreeNodeType()->getContainedType(0)->isFunctionTy())
    {
      errs() << *arg.getParent()->getFunctionType() << " DEBUG 312: in buildFormalTree: function pointer arg = " << *treeTyW->getTreeNodeType() << "\n";
    }
    else
    {
      if (USEDEBUGINFO) 
        buildTypeTreeWithDI(arg, treeTyW, treeTy, DIUtils::getBaseDIType(argDIType));
      else
        buildTypeTree(arg, treeTyW, treeTy);
    }
  } catch (std::exception &e) {
    errs() << e.what() << "\n";
  }
}

bool pdg::ProgramDependencyGraph::hasRecursiveType(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator insert_loc)
{
  TreeType treeTy = TreeType::FORMAL_IN_TREE;
  int height = argW->getTree(treeTy).depth(insert_loc);
  if (height != 0)
  {
    bool recursion_flag = false;
    tree<InstructionWrapper *>::iterator backTreeIt = insert_loc;
    while (height > 0)
    {
      backTreeIt = argW->getTree(treeTy).parent(backTreeIt);
      if ((*insert_loc)->getTreeNodeType() == (*backTreeIt)->getTreeNodeType())
      {
        recursion_flag = true;
        break;
      }
      height -= 1;
    }
    // process next type, because this type brings in a recursion
    if (recursion_flag)
      return true;
  }
  return false;
}

bool pdg::ProgramDependencyGraph::isFilePtrOrFuncTy(Type *ty)
{
  //if field is a function Ptr
  if (ty->isFunctionTy())
  {
    std::string Str;
    raw_string_ostream OS(Str);
    OS << ty;
    return true;
  }

  if (ty->isPointerTy())
  {
    Type *childEleTy = dyn_cast<PointerType>(ty)->getElementType();
    if (childEleTy->isStructTy())
    {
      std::string Str;
      raw_string_ostream OS(Str);
      OS << ty;
      //FILE*, bypass, no need to buildTypeTree
      if ("%struct._IO_FILE*" == OS.str() || "%struct._IO_marker*" == OS.str())
        return true;
    }
  }
  return false;
}

InstructionWrapper *pdg::ProgramDependencyGraph::buildPointerTypeNode(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator insert_loc)
{
  auto &pdgUtils = PDGUtils::getInstance();
  TreeType treeTy = TreeType::FORMAL_IN_TREE;
  Argument &arg = *argW->getArg();
  PointerType *pt = dyn_cast<PointerType>(curTyNode->getTreeNodeType());
  Type *pointedNodeTy = pt->getElementType();
  InstructionWrapper *pointedTypeW = new TreeTypeWrapper(arg.getParent(),
                                                         GraphNodeType::PARAMETER_FIELD,
                                                         &arg,
                                                         pointedNodeTy,
                                                         curTyNode->getTreeNodeType(),
                                                         0);
  pdgUtils.getFuncInstWMap()[arg.getParent()].insert(pointedTypeW);
  argW->getTree(treeTy).append_child(insert_loc, pointedTypeW);
  return pointedTypeW;
}

InstructionWrapper *pdg::ProgramDependencyGraph::buildPointerTypeNodeWithDI(ArgumentWrapper *argW, InstructionWrapper *curTyNode, tree<InstructionWrapper *>::iterator insert_loc, DIType *dt)
{
  auto &pdgUtils = PDGUtils::getInstance();
  TreeType treeTy = TreeType::FORMAL_IN_TREE;
  Argument &arg = *argW->getArg();
  PointerType *pt = dyn_cast<PointerType>(curTyNode->getTreeNodeType());
  Type *pointedNodeTy = pt->getElementType();
  try
  {
    InstructionWrapper *pointedTypeW = new TreeTypeWrapper(arg.getParent(),
                                                           GraphNodeType::PARAMETER_FIELD,
                                                           &arg,
                                                           pointedNodeTy,
                                                           curTyNode->getTreeNodeType(),
                                                           0,
                                                           DIUtils::getBaseDIType(dt));
    pdgUtils.getFuncInstWMap()[arg.getParent()].insert(pointedTypeW);
    argW->getTree(treeTy).append_child(insert_loc, pointedTypeW);
    return pointedTypeW;
  } catch (std::exception &e) {
    errs() << e.what() << "\n";
    exit(0);
  }
}

void pdg::ProgramDependencyGraph::buildTypeTree(Argument &arg, InstructionWrapper *treeTyW, TreeType treeTy)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Function *Func = arg.getParent();
  // setup instWQ to avoid recusion processing
  std::queue<InstructionWrapper *> instWQ;
  instWQ.push(treeTyW);
  ArgumentWrapper *argW = pdgUtils.getFuncMap()[Func]->getArgWByArg(arg);

  if (argW == nullptr)
    throw new ArgWrapperIsNullPtr("Argument Wrapper is nullptr");

  tree<InstructionWrapper *>::iterator insert_loc; // insert location in the parameter tree for the type wrapper
  int depth = 0;
  while (!instWQ.empty())
  {
    if (depth >= EXPAND_LEVEL)
      return;
    depth += 1;
    int qSize = instWQ.size();
    while (qSize > 0)
    {
      qSize -= 1;
      InstructionWrapper *curTyNode = instWQ.front();
      instWQ.pop();
      insert_loc = getInstInsertLoc(argW, curTyNode, treeTy);
      // handle recursion type using 1-limit approach
      // track back from child to parent, if find same type, stop building.
      // The type used here is form llvm type system.
      if (hasRecursiveType(argW, insert_loc))
        continue;
      // if is pointer type, create node for the pointed type
      Type *curNodeTy = curTyNode->getTreeNodeType();
      if (curNodeTy->isPointerTy())
      {
        InstructionWrapper* pointedTypeW = buildPointerTypeNode(argW, curTyNode, insert_loc);
        instWQ.push(pointedTypeW); // put the pointed node to queue
        continue;
      }
      // compose for struct
      if (!curNodeTy->isStructTy())
        continue;
      // processs non-pointer type
      Type *parentType = nullptr;
      // for struct type, insert all children to the tree
      for (unsigned int child_offset = 0; child_offset < curNodeTy->getNumContainedTypes(); child_offset++)
      {
        parentType = curTyNode->getTreeNodeType();
        // field sensitive processing. Get correspond gep and link tree node with gep.
        Type *childType = curNodeTy->getContainedType(child_offset);
        InstructionWrapper *gepInstW = getTreeNodeGEP(arg, child_offset, childType, parentType);
        InstructionWrapper *typeFieldW = new TreeTypeWrapper(arg.getParent(), GraphNodeType::PARAMETER_FIELD, &arg, childType, parentType, child_offset, gepInstW);
        // link gep with tree node
        if (gepInstW != nullptr)
          PDG->addDependency(typeFieldW, gepInstW, DependencyType::STRUCT_FIELDS);

        pdgUtils.getFuncInstWMap()[arg.getParent()].insert(typeFieldW);
        // start inserting formal tree instructions
        argW->getTree(treeTy).append_child(insert_loc, typeFieldW);

        //skip function ptr, FILE*
        if (isFilePtrOrFuncTy(childType))
          continue;

        instWQ.push(typeFieldW);
      }
    }
  }
}

void pdg::ProgramDependencyGraph::buildTypeTreeWithDI(Argument &arg, InstructionWrapper *treeTyW, TreeType treeTy, DIType *argDIType)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Function *Func = arg.getParent();
  // setup instWQ to avoid recusion processing
  std::queue<InstructionWrapper *> instWQ;
  std::queue<DIType *> DITypeQ;
  instWQ.push(treeTyW);
  DITypeQ.push(argDIType);
  ArgumentWrapper *argW = pdgUtils.getFuncMap()[Func]->getArgWByArg(arg);

  if (argW == nullptr)
    throw new ArgWrapperIsNullPtr("Argument Wrapper is nullptr");

  if (argDIType == nullptr)
    throw new ArgHasNoDITypeException("Argument Debug Type is nullptr");

  tree<InstructionWrapper *>::iterator insert_loc; // insert location in the parameter tree for the type wrapper
  int depth = 0;
  while (!instWQ.empty())
  {
    if (depth >= EXPAND_LEVEL)
      return;
    depth += 1;
    int qSize = instWQ.size();
    while (qSize > 0)
    {
      qSize -= 1;
      InstructionWrapper *curTyNode = instWQ.front();
      DIType *instDIType = DITypeQ.front();
      instWQ.pop();
      DITypeQ.pop();
      insert_loc = getInstInsertLoc(argW, curTyNode, treeTy);
      // handle recursion type using 1-limit approach
      // track back from child to parent, if find same type, stop building.
      // The type used here is form llvm type system.
      if (hasRecursiveType(argW, insert_loc))
        return;
      // if is pointer type, create a node represent pointer type.
      Type *curNodeTy = curTyNode->getTreeNodeType();
      if (curNodeTy->isPointerTy())
      {
        InstructionWrapper* pointedTypeW = buildPointerTypeNodeWithDI(argW, curTyNode, insert_loc, instDIType);
        instWQ.push(pointedTypeW); // put the pointed node to queue
        try {
          DITypeQ.push(DIUtils::getBaseDIType(instDIType));
        } catch (std::exception &e) {
          errs() << e.what() << "\n";
          exit(0);
        }
        continue;
      }

      if (!instDIType)
      {
        errs() << "No DIType left in the queue..." << "\n";
        return;
      }

      // compose for struct
      if (!curNodeTy->isStructTy())
        continue;

      instDIType = DIUtils::getLowestDIType(instDIType);
      if (!isa<DICompositeType>(instDIType))
        continue;

      // processs non-pointer type
      Type *parentType = nullptr;
      // for struct type, insert all children to the tree
      auto DINodeArr = dyn_cast<DICompositeType>(instDIType)->getElements();
      for (unsigned int child_offset = 0; child_offset < curNodeTy->getNumContainedTypes(); child_offset++)
      {
        parentType = curTyNode->getTreeNodeType();
        // field sensitive processing. Get correspond gep and link tree node with gep.
        Type *childType = curNodeTy->getContainedType(child_offset);
        DIType *childDIType = dyn_cast<DIType>(DINodeArr[child_offset]);
        InstructionWrapper *gepInstW = getTreeNodeGEP(arg, child_offset, childType, parentType);
        InstructionWrapper *typeFieldW = new TreeTypeWrapper(arg.getParent(), GraphNodeType::PARAMETER_FIELD, &arg, childType, parentType, child_offset, gepInstW, childDIType);
        // link gep with tree node
        if (gepInstW != nullptr)
          PDG->addDependency(typeFieldW, gepInstW, DependencyType::STRUCT_FIELDS);

        pdgUtils.getFuncInstWMap()[arg.getParent()].insert(typeFieldW);
        // start inserting formal tree instructions
        argW->getTree(treeTy).append_child(insert_loc, typeFieldW);
        //skip function ptr, FILE*
        if (isFilePtrOrFuncTy(childType))
          continue;
        instWQ.push(typeFieldW);
        DITypeQ.push(DIUtils::getBaseDIType(childDIType));
      }
    }
  }
}

void pdg::ProgramDependencyGraph::drawFormalParameterTree(Function *Func, TreeType treeTy)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto argWList = pdgUtils.getFuncMap()[Func]->getArgWList();
  for (ArgumentWrapper *argW : argWList)
  {
    for (tree<InstructionWrapper *>::iterator
             TI = argW->getTree(treeTy).begin(),
             TE = argW->getTree(treeTy).end();
         TI != TE; ++TI)
    {
      for (unsigned i = 0; i < TI.number_of_children(); i++)
      {
        InstructionWrapper *childW = *argW->getTree(treeTy).child(TI, i);
        PDG->addDependency(*TI, childW, DependencyType::PARAMETER);
      }
    }
  }
}

void pdg::ProgramDependencyGraph::connectFunctionAndFormalTrees(Function *callee)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (std::vector<ArgumentWrapper *>::iterator argI = pdgUtils.getFuncMap()[callee]->getArgWList().begin(),
                                                argE = pdgUtils.getFuncMap()[callee]->getArgWList().end();
       argI != argE; ++argI)
  {
    auto formalInTreeBeginI = (*argI)->getTree(TreeType::FORMAL_IN_TREE).begin();
    auto formalInTreeEndI = (*argI)->getTree(TreeType::FORMAL_IN_TREE).end();
    auto formalOutTreeBeginI = (*argI)->getTree(TreeType::FORMAL_OUT_TREE).begin();
    InstructionWrapper *formal_inW = *formalInTreeBeginI;
    InstructionWrapper *formal_outW = *formalOutTreeBeginI;

    // connect Function's EntryNode with formal in/out tree roots
    PDG->addDependency(pdgUtils.getFuncMap()[callee]->getEntryW(), formal_inW, DependencyType::PARAMETER);
    PDG->addDependency(pdgUtils.getFuncMap()[callee]->getEntryW(), formal_outW, DependencyType::PARAMETER);

    // two things: (1) formal-in --> callee's Store; (2) callee's Load --> formal-out
    for (tree<InstructionWrapper *>::iterator
             formal_in_TI = formalInTreeBeginI,
             formal_in_TE = formalInTreeEndI,
             formal_out_TI = formalOutTreeBeginI;
         formal_in_TI != formal_in_TE; ++formal_in_TI, ++formal_out_TI)
    {
      // connect formal-in and formal-out nodes formal-in --> formal-out
      PDG->addDependency(*formal_in_TI, *formal_out_TI, DependencyType::PARAMETER);

      // must handle nullptr case first
      if ((*formal_in_TI)->getTreeNodeType() == nullptr)
      {
        errs() << "connectFunctionAndFormalTrees: formal_in_TI->getFieldType() == nullptr !\n";
        break;
      }

      // link specific field with GEP if there is any.
      //linkTypeNodeWithGEPInst(*argI, formal_in_TI);

      // connect formal-in-tree type nodes with storeinst in call_func
      if (tree<InstructionWrapper *>::depth(formal_in_TI) == 0)
      {
        for (auto userIter = (*argI)->getArg()->user_begin();
             userIter != (*argI)->getArg()->user_end(); ++userIter)
        {
          if (Instruction *tmpInst = dyn_cast<Instruction>(*userIter))
          {
            PDG->addDependency(*formal_in_TI, pdgUtils.getInstMap()[tmpInst], DependencyType::DATA_GENERAL);
            (*formal_in_TI)->setVisited(true);
          }
        }
      }

      // 2. Callee's LoadInsts --> FORMAL_OUT in Callee function
      // must handle nullptr case first
      if ((*formal_out_TI)->getTreeNodeType() == nullptr)
      {
        errs() << "connectFunctionAndFormalTrees: LoadInst->FORMAL_OUT: "
                  "formal_out_TI->getFieldType() == nullptr!\n";
        break;
      }

      if ((*formal_out_TI)->getTreeNodeType() != nullptr)
      {
        for (LoadInst *loadInst : pdgUtils.getFuncMap()[callee]->getLoadInstList())
        {
          if ((*formal_out_TI)->getTreeNodeType() == loadInst->getPointerOperand()->getType()->getContainedType(0))
          {
            PDG->addDependency(pdgUtils.getInstMap()[loadInst],
                               *formal_out_TI,
                               DependencyType::DATA_GENERAL);
          }
        }
      }
    }
  }
}

bool pdg::ProgramDependencyGraph::connectAllPossibleFunctions(CallInst *CI, std::vector<Function *> indirect_call_candidates)
{
  auto &pdgUtils = PDGUtils::getInstance();
  InstructionWrapper *CInstW = pdgUtils.getInstMap()[CI];
  for (Function *func : indirect_call_candidates)
  {
    if (!connectCallerAndCallee(CInstW, func))
    {
      return false;
    }
  }
  return true;
}

bool pdg::ProgramDependencyGraph::connectCallerAndCallee(InstructionWrapper *instW, Function *callee)
{
  auto &pdgUtils = PDGUtils::getInstance();
  if (instW == nullptr || callee == nullptr)
    return false;

  // callInst in caller --> Entry Node in callee
  PDG->addDependency(instW, pdgUtils.getFuncMap()[callee]->getEntryW(), DependencyType::CONTROL);
  Function *caller = instW->getInstruction()->getFunction();
  // ReturnInst in callee --> CallInst in caller
  for (Instruction *retInst : pdgUtils.getFuncMap()[callee]->getReturnInstList())
  {
    for (InstructionWrapper *tmpInstW : pdgUtils.getFuncInstWMap()[caller])
    {
      if (retInst == tmpInstW->getInstruction())
      {
        if (dyn_cast<ReturnInst>(tmpInstW->getInstruction())->getReturnValue() != nullptr)
          PDG->addDependency(tmpInstW, instW, DependencyType::DATA_GENERAL);
        else
          errs() << "void ReturnInst: " << *tmpInstW->getInstruction();
      }
    }
  }

  // connect caller InstW with ACTUAL IN/OUT parameter trees
  CallInst *CI = dyn_cast<CallInst>(instW->getInstruction());

  for (ArgumentWrapper *argW : pdgUtils.getCallMap()[CI]->getArgWList())
  {
    InstructionWrapper *actual_inW = *(argW->getTree(TreeType::ACTUAL_IN_TREE).begin());
    InstructionWrapper *actual_outW = *(argW->getTree(TreeType::ACTUAL_OUT_TREE).begin());

    if (instW == actual_inW || instW == actual_outW)
      continue;

    PDG->addDependency(instW, actual_inW, DependencyType::PARAMETER);
    PDG->addDependency(instW, actual_outW, DependencyType::PARAMETER);
  }

  // old way, process four trees at the same time, remove soon
  std::vector<ArgumentWrapper *>::iterator formal_argI;
  formal_argI = pdgUtils.getFuncMap()[callee]->getArgWList().begin();

  std::vector<ArgumentWrapper *>::iterator formal_argE;
  formal_argE = pdgUtils.getFuncMap()[callee]->getArgWList().end();

  std::vector<ArgumentWrapper *>::iterator actual_argI;
  actual_argI = pdgUtils.getCallMap()[CI]->getArgWList().begin();

  std::vector<ArgumentWrapper *>::iterator actual_argE;
  actual_argE = pdgUtils.getCallMap()[CI]->getArgWList().end();

  // increase formal/actual tree iterator simutaneously
  for (; formal_argI != formal_argE; ++formal_argI, ++actual_argI)
  {
    // intra-connection between ACTUAL/FORMAL IN/OUT trees
    for (tree<InstructionWrapper *>::iterator
             actual_in_TI = (*actual_argI)->getTree(TreeType::ACTUAL_IN_TREE).begin(),
             actual_in_TE = (*actual_argI)->getTree(TreeType::ACTUAL_IN_TREE).end(),
             formal_in_TI = (*formal_argI)->getTree(TreeType::FORMAL_IN_TREE).begin(), // TI2
         formal_out_TI = (*formal_argI)->getTree(TreeType::FORMAL_OUT_TREE).begin(),   // TI3
         actual_out_TI = (*actual_argI)->getTree(TreeType::ACTUAL_OUT_TREE).begin();   // TI4
         actual_in_TI != actual_in_TE;
         ++actual_in_TI, ++formal_in_TI, ++formal_out_TI, ++actual_out_TI)
    {
      // connect trees: antual-in --> formal-in, formal-out --> actual-out
      PDG->addDependency(*actual_in_TI,
                         *formal_in_TI, DependencyType::PARAMETER);
      PDG->addDependency(*formal_out_TI,
                         *actual_out_TI, DependencyType::PARAMETER);

    } // end for(tree...) intra-connection between actual/formal

    // 3. ACTUAL_OUT --> LoadInsts in #Caller# function
    for (tree<InstructionWrapper *>::iterator
             actual_out_TI = (*actual_argI)->getTree(TreeType::ACTUAL_OUT_TREE).begin(),
             actual_out_TE = (*actual_argI)->getTree(TreeType::ACTUAL_OUT_TREE).end();
         actual_out_TI != actual_out_TE; ++actual_out_TI)
    {
      // must handle nullptr case first
      if ((*actual_out_TI)->getTreeNodeType() == nullptr)
      {
        errs() << "DEBUG ACTUAL_OUT->LoadInst: actual_out_TI->getFieldType() is empty!\n";
        break;
      }

      if ((*actual_out_TI)->getTreeNodeType() != nullptr)
      {
        for (LoadInst *loadInst : pdgUtils.getFuncMap()[instW->getFunction()]->getLoadInstList())
        {
          if ((*actual_out_TI)->getTreeNodeType() == loadInst->getType())
          {
            for (InstructionWrapper *tmpInstW : pdgUtils.getFuncInstWMap()[callee])
            {
              if (tmpInstW->getInstruction() == dyn_cast<Instruction>(loadInst))
                PDG->addDependency(*actual_out_TI, tmpInstW, DependencyType::DATA_GENERAL);
            }
          }
        }
      }
    }
  }

  return true;
}

void pdg::ProgramDependencyGraph::buildActualParameterTrees(CallInst *CI)
{
  auto &pdgUtils = PDGUtils::getInstance();
  // need to obtained called function and then copy the formal tree
  Function *called_func;
  // processing indirect call. Pick the first candidate function
  if (CI->getCalledFunction() == nullptr)
  {
    std::vector<Function *> indirect_call_candidate = collectIndirectCallCandidates(CI->getFunctionType());
    if (indirect_call_candidate.size() == 0)
    {
      errs() << "Parameter num 0, no need to build actual parameter tree"
             << "\n";
      return;
    }
    // get the first possible candidate
    called_func = indirect_call_candidate[0];
  }
  else
  {
    called_func = CI->getCalledFunction();
  }

  auto argI = pdgUtils.getCallMap()[CI]->getArgWList().begin();
  auto argE = pdgUtils.getCallMap()[CI]->getArgWList().end();

  auto argFI = pdgUtils.getFuncMap()[called_func]->getArgWList().begin();
  auto argFE = pdgUtils.getFuncMap()[called_func]->getArgWList().end();

  //copy Formal Tree to Actual Tree. Actual trees are used by call site.
  for (; argI != argE && argFI != argFE; ++argI, ++argFI)
  {
    (*argI)->copyTree((*argFI)->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_IN_TREE);
    (*argI)->copyTree((*argFI)->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_OUT_TREE);
  }
}

void pdg::ProgramDependencyGraph::drawActualParameterTree(CallInst *CI, pdg::TreeType treeTy)
{
  auto &pdgUtils = PDGUtils::getInstance();
  int ARG_POS = 0;
  for (std::vector<ArgumentWrapper *>::iterator argWI = pdgUtils.getCallMap()[CI]->getArgWList().begin();
       argWI != pdgUtils.getCallMap()[CI]->getArgWList().end();
       ++argWI)
  {
    Value *tmp_val = CI->getOperand(ARG_POS); // get the corresponding argument
    if (Instruction *tmpInst = dyn_cast<Instruction>(tmp_val))
    {
      Function *func = (*argWI)->getArg()->getParent();
      auto treeBegin = (*argWI)->getTree(TreeType::ACTUAL_IN_TREE).begin();
      // link each argument's instruction with actual tree head
      PDG->addDependency(pdgUtils.getInstMap()[tmpInst], *treeBegin, DependencyType::PARAMETER);
    }
    for (tree<InstructionWrapper *>::iterator TI = (*argWI)->getTree(treeTy).begin();
         TI != (*argWI)->getTree(treeTy).end();
         ++TI)
    {
      for (unsigned i = 0; i < TI.number_of_children(); i++)
      {
        InstructionWrapper *childW = *(*argWI)->getTree(treeTy).child(TI, i);
        PDG->addDependency(*TI, childW, DependencyType::PARAMETER);
      }
    }

    ARG_POS++;
  }
}

std::vector<Function *> pdg::ProgramDependencyGraph::collectIndirectCallCandidates(FunctionType *funcType)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<Function *> indirectCallList;
  std::map<const Function *, FunctionWrapper *>::iterator FI = pdgUtils.getFuncMap().begin();
  std::map<const Function *, FunctionWrapper *>::iterator FE = pdgUtils.getFuncMap().end();
  for (; FI != FE; ++FI)
  {
    Function *curFunc = const_cast<Function *>((*FI).first);
    // get Function type
    if (curFunc->getName() == "main")
      continue;
    // compare the indirect call function type with each function
    if (isFuncTypeMatch(funcType, curFunc->getFunctionType()))
      indirectCallList.push_back(curFunc);
  }

  return indirectCallList;
}

// -------------------------------
//
// Field sensitive functions
//
// -------------------------------

std::set<pdg::InstructionWrapper *> pdg::ProgramDependencyGraph::getAllRelevantGEP(Argument &arg)
{
  std::vector<Instruction *> initialStoreInsts = getArgStoreInsts(arg);
  std::set<InstructionWrapper *> relevantGEPs;
  std::queue<InstructionWrapper *> instWQ;
  auto &pdgUtils = PDGUtils::getInstance();

  //errs() << depInstW->getInstruction()->getFunction()->getName() << " " << *depInstW->getInstruction() << "\n";
  for (Instruction *storeInst : initialStoreInsts)
  {
    instWQ.push(pdgUtils.getInstMap()[storeInst]); // push the initial store instruction to the instQ
    auto dataDList = PDG->getNodeDepList(pdgUtils.getInstMap()[storeInst]);
    for (auto depPair : dataDList)
    {
      DependencyType depType = depPair.second;
      if (depType == DependencyType::DATA_ALIAS)
      {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        instWQ.push(depInstW);
      }
    }
  }

  while (!instWQ.empty())
  {
    InstructionWrapper *instW = instWQ.front();
    instWQ.pop();

    auto dataDList = PDG->getNodeDepList(instW);
    for (auto depPair : dataDList)
    {
      DependencyType depType = depPair.second;
      if (depType == DependencyType::DATA_DEF_USE)
      {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        instWQ.push(depInstW);
        if (depInstW->getInstruction() != nullptr && isa<GetElementPtrInst>(depInstW->getInstruction()))
          relevantGEPs.insert(depInstW);
      }
    }
  }

  return relevantGEPs;
}

InstructionWrapper *pdg::ProgramDependencyGraph::getTreeNodeGEP(Argument &arg, unsigned field_offset, Type *treeNodeTy, Type *parentNodeTy)
{
  std::set<InstructionWrapper *> RelevantGEPList = getAllRelevantGEP(arg);
  for (auto GEPInstW : RelevantGEPList)
  {
    int operand_num = GEPInstW->getInstruction()->getNumOperands();
    llvm::Value *last_idx = GEPInstW->getInstruction()->getOperand(operand_num - 1);
    // cast the last_idx to int type
    if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx))
    {
      // make sure type is matched
      if (!isa<GetElementPtrInst>(GEPInstW->getInstruction()) || parentNodeTy == nullptr)
        continue;
      auto GEP = dyn_cast<GetElementPtrInst>(GEPInstW->getInstruction());
      llvm::Type *GEPResTy = GEP->getResultElementType();
      llvm::Type *GEPSrcTy = GEP->getSourceElementType();

      // get access field idx from GEP
      int field_idx = constInt->getSExtValue();
      // plus one. Since for struct, the 0 index is used by the parent struct
      // type parent_type must be a pointer. Since only sub fields can have
      // parent that is not nullptr
      if (parentNodeTy->isPointerTy())
        parentNodeTy = parentNodeTy->getPointerElementType();

      // check the src type in GEP inst is equal to parent_type (GET FROM)
      // check if the offset is equal
      bool srcTypeMatch = (GEPSrcTy == parentNodeTy);
      bool resTypeMatch = (GEPResTy == treeNodeTy);
      bool offsetMatch = field_idx == field_offset;

      if (offsetMatch && resTypeMatch && srcTypeMatch)
        return GEPInstW;
    }
  }

  return nullptr;
}

std::vector<Instruction *> pdg::ProgramDependencyGraph::getArgStoreInsts(Argument &arg)
{
  std::vector<Instruction *> initialStoreInsts;
  for (auto UI = arg.user_begin(); UI != arg.user_end(); ++UI)
  {
    if (isa<StoreInst>(*UI))
    {
      Instruction *st = dyn_cast<Instruction>(*UI);
      initialStoreInsts.push_back(st);
    }
  }

  return initialStoreInsts;
}

bool pdg::ProgramDependencyGraph::isFuncTypeMatch(FunctionType *funcTy1, FunctionType *funcTy2)
{
  if (funcTy1->getNumParams() != funcTy2->getNumParams())
  {
    return false;
  }

  if (funcTy1->getReturnType() != funcTy2->getReturnType())
  {
    return false;
  }

  unsigned param_len = funcTy1->getNumParams();
  for (unsigned i = 0; i < param_len; ++i)
  {
    if (funcTy1->getParamType(i) != funcTy2->getParamType(i))
    {
      return false;
    }
  }
  return true;
}

tree<pdg::InstructionWrapper *>::iterator pdg::ProgramDependencyGraph::getInstInsertLoc(pdg::ArgumentWrapper *argW, InstructionWrapper *tyW, TreeType treeTy)
{
  tree<pdg::InstructionWrapper *>::iterator insert_loc = argW->getTree(treeTy).begin();
  while ((*insert_loc) != tyW && insert_loc != argW->getTree(treeTy).end())
  {
    insert_loc++;
  }
  return insert_loc;
}

typename pdg::DependencyNode<pdg::InstructionWrapper>::DependencyLinkList pdg::ProgramDependencyGraph::getNodeDepList(Instruction *inst)
{
  return PDG->getNodeDepList(PDGUtils::getInstance().getInstMap()[inst]);
}

static RegisterPass<pdg::ProgramDependencyGraph>
    PDG("pdg", "Program Dependency Graph Construction", false, true);