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

  std::set<std::string> importedFuncList;
  std::set<std::string> blackFuncList;
  std::set<std::string> definedFuncList;

  std::ifstream importedFuncs("imported_func.txt");
  std::ifstream blackFuncs("blacklist.txt");
  std::ifstream definedFuncs("defined_func.txt");

  for (std::string line; std::getline(blackFuncs, line);)
    blackFuncList.insert(line);

  for (std::string line; std::getline(importedFuncs, line);)
  {
    if (blackFuncList.find(line) == blackFuncList.end())
      importedFuncList.insert(line);
  }

  for (std::string line; std::getline(definedFuncs, line);)
    definedFuncList.insert(line);

  errs() << "Expand level " << EXPAND_LEVEL << "\n";
  errs() << "Using Debug Info " << USEDEBUGINFO << "\n";

  module = &M;
  auto &pdgUtils = PDGUtils::getInstance();
  pdgUtils.constructFuncMap(M);
  pdgUtils.collectGlobalInsts(M);
  int user_def_func_num = 0;

  bool fixPoint = false;
  int callLevel = 4;
  while (!fixPoint && callLevel > 0) {
    callLevel -= 1;
    // copy dependencies from DDG/CDG to PDG
    for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
    {
      Function *Func = dyn_cast<Function>(FI);
      user_def_func_num++; // count function has definition as user defined
#ifdef FUNC_LIST
      if (Func->isDeclaration() || (importedFuncList.find(Func->getName().str()) == importedFuncList.end() && definedFuncList.find(Func->getName().str()) == definedFuncList.end()))
#else
      if (Func->isDeclaration())
#endif
        continue;
      buildPDGForFunc(Func);
    }

    // start process CallInst
    bool callInstFixPoint = true;
    for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
    {
      Function *Func = dyn_cast<Function>(FI);
#ifdef FUNC_LIST
      // if (Func->isDeclaration() || importedFuncList.find(Func->getName().str()) == importedFuncList.end())
      if (Func->isDeclaration() || (importedFuncList.find(Func->getName().str()) == importedFuncList.end() && definedFuncList.find(Func->getName().str()) == definedFuncList.end()))
#else
      if (Func->isDeclaration())
#endif
        continue;

      auto callInstsList = pdgUtils.getFuncMap()[Func]->getCallInstList();
      for (CallInst *ci : callInstsList)
      {
        if (!processCallInst(pdgUtils.getInstMap()[ci])) {
          errs() << "bad indirect call.." << "\n";
          continue;
        }
          // throw NullCalledFunction("Called Function is null. Possbile no matching function found for indirect call");

        if (isIndirectCallOrInlineAsm(ci))
        {
          for (auto f : collectIndirectCallCandidates(ci->getFunctionType(), definedFuncList)) // need modification
          {
              importedFuncList.insert(f->getName().str());
              callInstFixPoint = false;
          }
        }
        else
        {
          auto calledFunc = getCalledFunction(ci);
          if (importedFuncList.find(calledFunc->getName().str()) != importedFuncList.end())
          {
            importedFuncList.insert(calledFunc->getName().str());
            callInstFixPoint = false;
          }
        }
      }
    }
    fixPoint = fixPoint && callInstFixPoint;
  }

  importedFuncs.close();
  blackFuncs.close();
  return false;
}

void pdg::ProgramDependencyGraph::buildPDGForFunc(Function *Func)
{
  auto &pdgUtils = PDGUtils::getInstance();

  pdgUtils.categorizeInstInFunc(*Func);
  cdg = &getAnalysis<ControlDependencyGraph>(*Func);
  ddg = &getAnalysis<DataDependencyGraph>(*Func);

  for (InstructionWrapper *instW : pdgUtils.getFuncInstWMap()[Func])
    addNodeDependencies(instW);

  if (!pdgUtils.getFuncMap()[Func]->hasTrees())
    buildFormalTreeForFunc(Func);
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
    errs() << "cannot find possible indirect call candidates.. " << *CI << "\n";
    return false;
  }
  CallWrapper *callW = new CallWrapper(CI, indirect_call_candidates);
  pdgUtils.getCallMap()[CI] = callW;
  
  // build formal tree for all candidiates.
  for (Function *indirect_called_func : indirect_call_candidates)
  {
    if (indirect_called_func->isDeclaration())
      continue;
    if (indirect_called_func->arg_empty())
      continue;
    if (pdgUtils.getFuncMap()[indirect_called_func]->hasTrees())
      continue;
    buildPDGForFunc(indirect_called_func);
    // buildFormalTreeForFunc(indirect_called_func);
  }
  buildActualParameterTrees(CI);
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
    if (CI->isInlineAsm())
      return true;

    if (isIndirectCallOrInlineAsm(CI))
      return processIndirectCallInst(CI, instW); // indirect function call get func type for indirect call inst

    if (Function *f = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts())) // handle case for bitcast
      callee = f;
    // handle intrinsic functions
    if (callee->isIntrinsic()) {
      return true;
    }

    // special cases done, common function
    CallWrapper *callW = new CallWrapper(CI);
    pdgUtils.getCallMap()[CI] = callW;
    if (!callee->isDeclaration())
    {
      if (!callee->arg_empty())
      {
        if (!pdgUtils.getFuncMap()[callee]->hasTrees())
          buildPDGForFunc(callee);
        buildActualParameterTrees(CI);
      } // end if !callee

      connectCallerAndCallee(instW, callee);
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

std::vector<Instruction *> pdg::ProgramDependencyGraph::getArgStoreInsts(Argument &arg)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<Instruction *> initialStoreInsts;
  if (arg.getArgNo() == RETVALARGNO)
  {
    auto &pdgUtils = PDGUtils::getInstance();
    Function* func = arg.getParent();
    for (auto st : pdgUtils.getFuncMap()[func]->getStoreInstList())
    {
      // use type matching approach for return value
      if (st->getValueOperand()->getType() == arg.getType())
        initialStoreInsts.push_back(st);
    }
    return initialStoreInsts;
  }

  for (auto UI = arg.user_begin(); UI != arg.user_end(); ++UI)
  {
    if (auto st = dyn_cast<StoreInst>(*UI))
    {
      if (st->getValueOperand() == &arg)
        initialStoreInsts.push_back(st);
    }

    if (isa<CastInst>(*UI))
    {
      for (auto CIU = (*UI)->user_begin(); CIU != (*UI)->user_end(); ++CIU)
      {
        if (auto cist = dyn_cast<StoreInst>(*CIU))
        {
          if (cist->getValueOperand() == *UI) {
            initialStoreInsts.push_back(cist);
          }
        }
      }
    }
  }

  return initialStoreInsts;
}

Instruction *pdg::ProgramDependencyGraph::getArgAllocaInst(Argument &arg)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (auto dbgInst : pdgUtils.getFuncMap()[arg.getParent()]->getDbgDeclareInstList())
  {
    if (auto DLV = dyn_cast<DILocalVariable>(dbgInst->getVariable()))
    {
      if (DLV->isParameter())
      {
        Instruction *allocaInst = dyn_cast<Instruction>(dbgInst->getVariableLocation());
        return allocaInst;
      }
    }
  }
  errs() << arg.getParent()->getName() << " " << arg.getArgNo() << "\n";
  assert(false && "no viable alloca inst for argument.");
}

bool pdg::ProgramDependencyGraph::nameMatch(std::string str1, std::string str2)
{
  std::string deli = ".";
  unsigned str1FirstDeliPos = str1.find(deli);
  unsigned str1SecondDeliPos = str1.find(deli);
  unsigned str2FirstDeliPos = str2.find(deli);
  unsigned str2SecondDeliPos = str2.find(deli);
  str1 = str1.substr(str1FirstDeliPos, str1SecondDeliPos);
  str2 = str2.substr(str2FirstDeliPos, str2SecondDeliPos);
  return str1 == str2;
}

bool pdg::ProgramDependencyGraph::isFuncTypeMatch(FunctionType *funcTy1, FunctionType *funcTy2)
{
  if (funcTy1->getNumParams() != funcTy2->getNumParams())
    return false;

  auto func1RetType = funcTy1->getReturnType();
  auto func2RetType = funcTy2->getReturnType();
  
  if (func1RetType != func2RetType)
    return false;

  for (unsigned i = 0; i < funcTy1->getNumParams(); ++i)
  {
    if (funcTy1->getParamType(i) != funcTy2->getParamType(i))
    {
      if (isStructPointer(funcTy1->getParamType(i)) && isStructPointer(funcTy2->getParamType(i)))
      {
        std::string func1ParamName = funcTy1->getParamType(i)->getPointerElementType()->getStructName();
        std::string func2ParamName = funcTy2->getParamType(i)->getPointerElementType()->getStructName();
        if (nameMatch(func1ParamName, func2ParamName))
          continue;
      }
      return false;
    }
  }

  return true;
}

bool pdg::ProgramDependencyGraph::isIndirectCallOrInlineAsm(CallInst *CI)
{
  const Value *V = CI->getCalledValue();
  if (isa<Function>(V) || isa<Constant>(V))
     return false;
  if (CI->isInlineAsm())
    return true;
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

typename DependencyNode<InstructionWrapper>::DependencyLinkList pdg::ProgramDependencyGraph::getNodesWithDepType(const InstructionWrapper *instW, DependencyType depType) {
  auto node = PDG->getNodeByData(instW);
  return node->getNodesWithDepType(depType);
}
// --------------------------------------
// --------------------------------------
// Build tree functions
// --------------------------------------
// --------------------------------------

void pdg::ProgramDependencyGraph::buildFormalTreeForFunc(Function *Func)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto FuncW = pdgUtils.getFuncMap()[Func];
  for (auto argW : FuncW->getArgWList())
  {
    // build formal in tree first
    buildFormalTreeForArg(*argW->getArg(), TreeType::FORMAL_IN_TREE);
    // then, copy formal in tree content to formal out tree
    argW->copyTree(argW->getTree(TreeType::FORMAL_IN_TREE), TreeType::FORMAL_OUT_TREE);
  }
  buildFormalTreeForArg(*FuncW->getRetW()->getArg(), TreeType::FORMAL_IN_TREE);
  pdgUtils.getFuncMap()[Func]->setTreeFlag(true);
  drawFormalParameterTree(Func, TreeType::FORMAL_IN_TREE);
  drawFormalParameterTree(Func, TreeType::FORMAL_OUT_TREE);
  connectFunctionAndFormalTrees(Func);
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
    assert(argW->getTree(treeTy).size() != 0 && "parameter tree has size 0 after root build!");

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
      // for struct type, insert all children to the tree
      for (unsigned int child_offset = 0; child_offset < curNodeTy->getNumContainedTypes(); child_offset++)
      {
        Type *parentType = curTyNode->getTreeNodeType();
        // field sensitive processing. Get correspond gep and link tree node with gep.
        Type *childType = curNodeTy->getContainedType(child_offset);
        InstructionWrapper *typeFieldW = new TreeTypeWrapper(arg.getParent(), GraphNodeType::PARAMETER_FIELD, &arg, childType, parentType, child_offset);
        // link gep with tree node
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
  {
    errs() << arg.getArgNo() << "\n";
    return;
    // throw new ArgHasNoDITypeException("Argument Debug Type is nullptr");
  }

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
      if (hasRecursiveType(argW, insert_loc))
        return;
      // if is pointer type, create a node represent pointer type.
      Type *curNodeTy = curTyNode->getTreeNodeType();
      if (curNodeTy->isPointerTy())
      {
        InstructionWrapper* pointedTypeW = buildPointerTypeNodeWithDI(argW, curTyNode, insert_loc, instDIType);
        instWQ.push(pointedTypeW); // put the pointed node to queue
        try {
          DITypeQ.push(DIUtils::getBaseDIType(instDIType)); // put the debug info node to the queue
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

      Type *parentType = nullptr;
      // for struct type, insert all children to the tree
      auto DINodeArr = dyn_cast<DICompositeType>(instDIType)->getElements();
      if (DINodeArr.size() < curNodeTy->getStructNumElements())
        continue;

      for (unsigned int child_offset = 0; child_offset < curNodeTy->getStructNumElements(); child_offset++)
      {
        parentType = curTyNode->getTreeNodeType();
        // field sensitive processing. Get correspond gep and link tree node with gep.
        Type *childType = curNodeTy->getStructElementType(child_offset);
        DIType *childDIType = dyn_cast<DIType>(DINodeArr[child_offset]);
        InstructionWrapper *typeFieldW = new TreeTypeWrapper(arg.getParent(), GraphNodeType::PARAMETER_FIELD, &arg, childType, parentType, child_offset, childDIType);
        // does bit field processing
        if (curNodeTy->isIntegerTy() && instDIType->isBitField())
        {
          unsigned bitFieldPackingSizeInBits = curNodeTy->getIntegerBitWidth();
          while (bitFieldPackingSizeInBits > 0 && instDIType->isBitField())
          {
            typeFieldW->addBitFieldName(DIUtils::getDIFieldName(instDIType));
            uint64_t bitFieldSize = instDIType->getSizeInBits();
            bitFieldPackingSizeInBits -= bitFieldSize;
            instDIType = DITypeQ.front();
            if (!instDIType->isBitField())
              break;
            DITypeQ.pop();
          }
          continue;
        }

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

std::vector<InstructionWrapper *> pdg::ProgramDependencyGraph::getReadInstsOnInst(Instruction *inst)
{
  std::vector<InstructionWrapper *> ret;
  auto depList = getNodeDepList(inst);
  for (auto pair : depList)
  {
    if (pair.second == DependencyType::DATA_READ) 
      ret.push_back(const_cast<InstructionWrapper *>(pair.first->getData()));
  }
  return ret;
}

std::vector<InstructionWrapper *> pdg::ProgramDependencyGraph::getAllAlias(Instruction *inst)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto instW = pdgUtils.getInstMap()[inst];
  std::vector<InstructionWrapper*> ret;
  std::set<InstructionWrapper*> seen_nodes;
  std::queue<InstructionWrapper*> workQ;
  workQ.push(const_cast<InstructionWrapper *>(instW));
  seen_nodes.insert(const_cast<InstructionWrapper *>(instW));

  while (!workQ.empty())
  {
    auto I = workQ.front();
    workQ.pop();
    if (I->getInstruction() != nullptr) {
      auto depList = getNodeDepList(I->getInstruction());
      for (auto pair : depList)
      {
        if (pair.second != DependencyType::DATA_ALIAS)
          continue;
          
        auto tmpInstW = const_cast<InstructionWrapper *>(pair.first->getData());
        if (seen_nodes.find(tmpInstW) != seen_nodes.end())
          continue;

        seen_nodes.insert(tmpInstW);
        workQ.push(tmpInstW);
        ret.push_back(tmpInstW);
      }
    }
  }
  return ret;
}

void pdg::ProgramDependencyGraph::connectFunctionAndFormalTrees(Function *callee)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (std::vector<ArgumentWrapper *>::iterator argI = pdgUtils.getFuncMap()[callee]->getArgWList().begin(),
                                                argE = pdgUtils.getFuncMap()[callee]->getArgWList().end();
       argI != argE; ++argI)
  {
    auto formalInTreeBegin = (*argI)->tree_begin(TreeType::FORMAL_IN_TREE);
    auto formalOutTreeBegin = (*argI)->tree_begin(TreeType::FORMAL_OUT_TREE);
    // connect Function's EntryNode with formal in/out tree roots
    PDG->addDependency(pdgUtils.getFuncMap()[callee]->getEntryW(), *formalInTreeBegin, DependencyType::PARAMETER);
    PDG->addDependency(pdgUtils.getFuncMap()[callee]->getEntryW(), *formalOutTreeBegin, DependencyType::PARAMETER);
    // find store instructions represent values for root. The value operand of
    // sotre instruction is the argument we interested.
    auto argAlloc = getArgAllocaInst(*(*argI)->getArg());
    pdgUtils.getInstMap()[argAlloc]->setGraphNodeType(GraphNodeType::ARG_ALLOC);
    PDG->addDependency(*formalInTreeBegin, pdgUtils.getInstMap()[argAlloc], DependencyType::VAL_DEP);

    // two things: (1) formal-in --> callee's Store; (2) callee's Load --> formal-out
    for (tree<InstructionWrapper *>::iterator
             formal_in_TI = formalInTreeBegin,
             formal_in_TE = (*argI)->tree_end(TreeType::FORMAL_IN_TREE),
             formal_out_TI = formalOutTreeBegin;
         formal_in_TI != formal_in_TE; ++formal_in_TI, ++formal_out_TI)
    {
      if ((*formal_in_TI)->getTreeNodeType() == nullptr)
        break;
      // connect formal-in and formal-out nodes formal-in --> formal-out
      PDG->addDependency(*formal_in_TI, *formal_out_TI, DependencyType::PARAMETER);
      if (tree<InstructionWrapper*>::depth(formal_in_TI) == 0)
        continue;
      // for tree nodes that are not root, get parent's values and then find loadInst or GEP Inst from parent's loads instructions
      auto ParentI = tree<InstructionWrapper*>::parent(formal_in_TI);
      auto parentValDepNodes = getNodesWithDepType(*ParentI, DependencyType::VAL_DEP);
      for (auto pair : parentValDepNodes)
      {
        auto depInstW = pair.first->getData();
        auto depInstAliasList = getAllAlias(depInstW->getInstruction());
        depInstAliasList.push_back(const_cast<InstructionWrapper*>(depInstW));
        for (auto depInstAlias : depInstAliasList)
        {
          if (depInstAlias->getInstruction() == nullptr)
            continue;
          PDG->addDependency(*ParentI, depInstAlias, DependencyType::VAL_DEP); // add parent node with alias with Val_Dep 
          auto readInsts = getReadInstsOnInst(depInstAlias->getInstruction());
          for (auto readInstW : readInsts)
          {
            if (isa<LoadInst>(readInstW->getInstruction())) 
              PDG->addDependency(*formal_in_TI, readInstW, DependencyType::VAL_DEP);

            if (isa<GetElementPtrInst>(readInstW->getInstruction()))
            {
              if (isTreeNodeGEPMatch(*formal_in_TI, readInstW->getInstruction()))
                PDG->addDependency(*formal_in_TI, readInstW, DependencyType::VAL_DEP);
            }
          }
        }
      }

      // 2. Callee's LoadInsts --> FORMAL_OUT in Callee function
      // must handle nullptr case first
      // if ((*formal_out_TI)->getTreeNodeType() == nullptr)
      // {
      //   errs() << "connectFunctionAndFormalTrees: LoadInst->FORMAL_OUT: "
      //             "formal_out_TI->getFieldType() == nullptr!\n";
      //   break;
      // }

      // if ((*formal_out_TI)->getTreeNodeType() == nullptr)
      //   return;

      // for (LoadInst *loadInst : pdgUtils.getFuncMap()[callee]->getLoadInstList())
      // {
      //   if ((*formal_out_TI)->getTreeNodeType() == loadInst->getPointerOperand()->getType()->getContainedType(0))
      //     PDG->addDependency(pdgUtils.getInstMap()[loadInst], *formal_out_TI, DependencyType::DATA_GENERAL);
      // }
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

void pdg::ProgramDependencyGraph::connectActualTreeToFormalTree(CallInst *CI, Function *called_func)
{
  auto &pdgUtils = PDGUtils::getInstance();
  // old way, process four trees at the same time, remove soon
  std::vector<ArgumentWrapper *>::iterator formal_argI = pdgUtils.getFuncMap()[called_func]->getArgWList().begin();
  std::vector<ArgumentWrapper *>::iterator formal_argE = pdgUtils.getFuncMap()[called_func]->getArgWList().end();
  std::vector<ArgumentWrapper *>::iterator actual_argI = pdgUtils.getCallMap()[CI]->getArgWList().begin();

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
  }
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

  connectActualTreeToFormalTree(CI, callee);
  // // 3. ACTUAL_OUT --> LoadInsts in #Caller# function
  // for (tree<InstructionWrapper *>::iterator
  //          actual_out_TI = (*actual_argI)->getTree(TreeType::ACTUAL_OUT_TREE).begin(),
  //          actual_out_TE = (*actual_argI)->getTree(TreeType::ACTUAL_OUT_TREE).end();
  //      actual_out_TI != actual_out_TE; ++actual_out_TI)
  // {
  //   for (LoadInst *loadInst : pdgUtils.getFuncMap()[instW->getFunction()]->getLoadInstList())
  //   {
  //     if ((*actual_out_TI)->getTreeNodeType() != loadInst->getType())
  //       continue;

  //     for (InstructionWrapper *tmpInstW : pdgUtils.getFuncInstWMap()[callee])
  //     {
  //       if (tmpInstW->getInstruction() == dyn_cast<Instruction>(loadInst))
  //         PDG->addDependency(*actual_out_TI, tmpInstW, DependencyType::DATA_GENERAL);
  //     }
  //   }
  // }
  // }
  return true;
}

void pdg::ProgramDependencyGraph::copyFormalTreeToActualTree(CallInst *CI, Function *func)
{
  auto &pdgUtils = PDGUtils::getInstance();
  CallWrapper *CW = pdgUtils.getCallMap()[CI];
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[func];
  auto argI = CW->getArgWList().begin();
  auto argE = CW->getArgWList().end();
  auto argFI = funcW->getArgWList().begin();
  auto argFE = funcW->getArgWList().end();
  //copy Formal Tree to Actual Tree. Actual trees are used by call site.
  for (; argI != argE && argFI != argFE; ++argI, ++argFI)
  {
    (*argI)->copyTree((*argFI)->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_IN_TREE);
    (*argI)->copyTree((*argFI)->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_OUT_TREE);
  }

  // copy return value wrapper
  ArgumentWrapper *CIRetW = pdgUtils.getCallMap()[CI]->getRetW();
  ArgumentWrapper *FuncRetW = pdgUtils.getFuncMap()[func]->getRetW();

  if (CIRetW == nullptr)
    return;
  CIRetW->copyTree(FuncRetW->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_IN_TREE);
  CIRetW->copyTree(FuncRetW->getTree(TreeType::FORMAL_IN_TREE), TreeType::ACTUAL_OUT_TREE);
  // linkGEPsWithTree(CI);
}

void pdg::ProgramDependencyGraph::buildActualParameterTrees(CallInst *CI)
{
  auto &pdgUtils = PDGUtils::getInstance();
  // need to obtained called function and then copy the formal tree
  Function *called_func;
  // processing indirect call. Pick the first candidate function
  if (CI->getCalledFunction() == nullptr)
  {
    if (Function *f = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts())) // Call to bitcast case
    {
      called_func = f;
    }
    else
    {
      std::vector<Function *> indirect_call_candidate = collectIndirectCallCandidates(CI->getFunctionType());
      if (indirect_call_candidate.size() == 0)
      {
        errs() << "No possible matching candidate, no need to build actual parameter tree" << "\n";
        return;
      }
      // get the first possible candidate
      called_func = indirect_call_candidate[0];
    }
  }
  else
  {
    called_func = CI->getCalledFunction();
  }

  copyFormalTreeToActualTree(CI, called_func);
  drawActualParameterTree(CI, TreeType::ACTUAL_IN_TREE);
  drawActualParameterTree(CI, TreeType::ACTUAL_OUT_TREE);
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

std::vector<Function *> pdg::ProgramDependencyGraph::collectIndirectCallCandidates(FunctionType* funcType, const std::set<std::string> &filterFuncs)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<Function *> indirectCallList;
  for (auto &F : *module)
  {
    std::string funcName = F.getName().str();
    // get Function type
    if (funcName == "main")
      continue;
    // compare the indirect call function type with each function, filter out certian functions that should not be considered as call targets
    if (isFuncTypeMatch(funcType, F.getFunctionType()) && filterFuncs.find(funcName) != filterFuncs.end())
      indirectCallList.push_back(&F);
  }

  return indirectCallList;
}

Function *pdg::ProgramDependencyGraph::getCalledFunction(CallInst* CI)
{
  if (isIndirectCallOrInlineAsm(CI))
    return nullptr;
  Function* calledFunc = CI->getCalledFunction();
  if (calledFunc == nullptr)
    return dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts()); // handle case for bitcast
  return calledFunc;
  // assert(calledFunc != nullptr && "Cannot find called function");
  // return calledFunc;
}

// -------------------------------
//
// Field sensitive functions
//
// -------------------------------

bool pdg::ProgramDependencyGraph::isTreeNodeGEPMatch(InstructionWrapper* treeNode, Instruction* GEP)
{
  if (auto gepInst = dyn_cast<GetElementPtrInst>(GEP))
  {
    auto parentNodeTy = treeNode->getParentTreeNodeType();
    auto treeNodeTy = treeNode->getTreeNodeType();
    int field_offset = treeNode->getNodeOffset();
    int operand_num = gepInst->getNumOperands();
    llvm::Value *last_idx = gepInst->getOperand(operand_num - 1);
    // cast the last_idx to int type
    if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx))
    {
      // make sure type is matched
      if (parentNodeTy == nullptr)
        return false;
      auto GEP = dyn_cast<GetElementPtrInst>(gepInst);
      Type *GEPResTy = gepInst->getResultElementType();
      Type *GEPSrcTy = gepInst->getSourceElementType();
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
        return true;
    }
  }
  return false;
}

bool pdg::ProgramDependencyGraph::isFuncPointer(Type *ty)
{
  if (ty->isPointerTy())
    return dyn_cast<PointerType>(ty)->getElementType()->isFunctionTy();
  return false;
}

bool pdg::ProgramDependencyGraph::isStructPointer(Type *ty)
{
  if (ty->isPointerTy())
    return ty->getPointerElementType()->isStructTy();
  return false;
}


static RegisterPass<pdg::ProgramDependencyGraph>
    PDG("pdg", "Program Dependency Graph Construction", false, true);