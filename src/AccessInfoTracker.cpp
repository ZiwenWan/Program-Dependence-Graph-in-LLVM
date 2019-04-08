#include "AccessInfoTracker.hpp"

using namespace llvm;

char pdg::AccessInfoTracker::ID = 0;

bool pdg::AccessInfoTracker::runOnModule(Module &M)
{
  std::ifstream importedFuncs("imported_func.txt");
  std::ifstream definedFuncs("defineblackFuncList.txt");
  std::ifstream blackFuncs("blacklist.txt");

  for (std::string line; std::getline(blackFuncs, line);)
    blackFuncList.insert(line);

  for (std::string line; std::getline(importedFuncs, line);) {
    if (blackFuncList.find(line) == blackFuncList.end())
      importedFuncList.insert(line);
  }

  for (std::string line; std::getline(definedFuncs, line);)
    definedFuncList.insert(line);

  kernelFuncList = importedFuncList;

  auto &pdgUtils = PDGUtils::getInstance();
  PDG = &getAnalysis<pdg::ProgramDependencyGraph>();

  if (!USEDEBUGINFO)
  {
    errs() << "[WARNING] No debug information avaliable... \nUse [-debug 1] in the pass to generate debug information\n";
    exit(0);
  }

  bool reachFixPoint = false;
  int funcCallLevel = 3;
  while (!reachFixPoint && funcCallLevel > 0)
  {
    // intra-procedural analysis
    funcCallLevel -= 1;
    for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
    {
      Function &F = *FI;
#ifdef FUNC_LIST
      if (F.isDeclaration() || importedFuncList.find(F.getName().str()) == importedFuncList.end())
        continue;
#else
      if (F.isDeclaration())
        continue;
#endif
      getIntraFuncReadWriteInfoForFunc(F);
    }

    // inter-procedure access info analysis
    bool fixPoint = true;
    for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
    {
      Function &F = *FI;

#ifdef FUNC_LIST
      if (F.isDeclaration() || importedFuncList.find(F.getName().str()) == importedFuncList.end())
        continue;
#else
      if (F.isDeclaration())
        continue;
#endif
      
      fixPoint = fixPoint && getInterFuncReadWriteInfo(F);
    }
    reachFixPoint = fixPoint;
  }
  std::string file_name = "accinfo";
  file_name += ".txt";
  idl_file.open(file_name);
  idl_file << "module kernel()" << " {\n";

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;

#ifdef FUNC_LIST
    if (F.isDeclaration() || importedFuncList.find(F.getName().str()) == importedFuncList.end())
      continue;
#else
    if (F.isDeclaration())
      continue;
#endif
    // printFuncArgAccessInfo(F);
    if (definedFuncList.find(F.getName().str()) == definedFuncList.end())
      generateIDLforFunc(F);
  }

  idl_file << "}";
  idl_file.close();
  importedFuncs.close();
  definedFuncs.close();
  blackFuncs.close();
  return false;
}

void pdg::AccessInfoTracker::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<pdg::ProgramDependencyGraph>();
  AU.setPreservesAll();
}

AccessType pdg::AccessInfoTracker::getAccessTypeForInstW(const InstructionWrapper *instW)
{
  auto dataDList = PDG->getNodeDepList(instW->getInstruction());
  AccessType accessType = AccessType::NOACCESS;
  for (auto depPair : dataDList)
  {
    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
    DependencyType depType = depPair.second;
    // check for read
    if (!depInstW->getInstruction())
      continue;

    if (depType == DependencyType::DATA_DEF_USE)
    {
      if (isa<LoadInst>(depInstW->getInstruction()) || isa<GetElementPtrInst>(depInstW->getInstruction()))
        accessType = AccessType::READ;
    }

    // check for store instruction.
    if (depType == DependencyType::DATA_DEF_USE)
    {
      if (StoreInst *st = dyn_cast<StoreInst>(depInstW->getInstruction()))
      {
        if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
        {
          if (instW->getGraphNodeType() == GraphNodeType::ARG_ALLOC) // ignore the store inst that store arg to stack mem
            break;
          accessType = AccessType::WRITE;
          break;
        }
      }
    }
  }
  return accessType;
}

void pdg::AccessInfoTracker::printRetValueAccessInfo(Function &Func)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (CallInst *CI : pdgUtils.getFuncMap()[&Func]->getCallInstList())
  {
    CallWrapper *CW = pdgUtils.getCallMap()[CI];
    errs() << "Ret Value Acc Info.." << "\n";
    printArgAccessInfo(CW->getRetW(), TreeType::ACTUAL_IN_TREE);
    errs() << "......... [ END " << Func.getName() << " ] .........\n\n";
  }
}

// intra function
void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForArg(ArgumentWrapper *argW)
{
  auto argTree = argW->getTree(TreeType::FORMAL_IN_TREE);
  if (argTree.size() == 0)
    return;

  // throw new ArgParameterTreeSizeIsZero("Argment tree is empty... Every param should have at least one node...\n");
  auto treeI = argW->getTree(TreeType::FORMAL_IN_TREE).begin();
  if (!(*treeI)->getTreeNodeType()->isPointerTy())
  {
    errs() << argW->getArg()->getParent()->getName() << " Find non-pointer type parameter, do not track...\n";
    return;
  }

  AccessType accessType = AccessType::NOACCESS;
  errs() << "Intra Read/Write info for: " << argW->getArg()->getParent()->getName() << "\n";
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
    for (auto valDepPair : valDepPairList)
    {
      auto dataW = valDepPair.first->getData();
      errs() << "Dep mem: " << *dataW->getInstruction() << "\n";
      AccessType accType = getAccessTypeForInstW(dataW);
      if (static_cast<int>(accType) > static_cast<int>((*treeI)->getAccessType()))
        (*treeI)->setAccessType(accType);
    }
  }
}

void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForFunc(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  // for arguments
  for (auto argW : funcW->getArgWList())
  {
    getIntraFuncReadWriteInfoForArg(argW);
  }
  // for return value
  ArgumentWrapper *retW = funcW->getRetW();
  getIntraFuncReadWriteInfoForArg(retW);
}

pdg::ArgumentMatchType pdg::AccessInfoTracker::getArgMatchType(Argument *arg1, Argument *arg2)
{
  Type *arg1_type = arg1->getType();
  Type *arg2_type = arg2->getType();

  if (arg1_type == arg2_type)
  {
    return pdg::ArgumentMatchType::EQUAL;
  }

  if (arg1_type->isPointerTy())
  {
    arg1_type = (dyn_cast<PointerType>(arg1_type))->getElementType();
  }

  if (arg1_type->isStructTy())
  {
    StructType *arg1_st_type = dyn_cast<StructType>(arg1_type);
    for (unsigned i = 0; i < arg1_st_type->getNumElements(); ++i)
    {
      Type *arg1_element_type = arg1_st_type->getElementType(i);
      bool type_match = (arg1_element_type == arg2_type);

      if (arg2_type->isPointerTy())
      {
        bool pointed_type_match = ((dyn_cast<PointerType>(arg2_type))->getElementType() == arg1_element_type);
        type_match = type_match || pointed_type_match;
      }

      if (type_match)
      {
        return pdg::ArgumentMatchType::CONTAINED;
      }
    }
  }

  return pdg::ArgumentMatchType::NOTCONTAINED;
}

void pdg::AccessInfoTracker::mergeArgAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper *>::iterator callerParamI)
{
  unsigned callerParamTreeSize = callerArgW->getTree(TreeType::FORMAL_IN_TREE).size(callerParamI);
  unsigned calleeParamTreeSize = calleeArgW->getTree(TreeType::FORMAL_IN_TREE).size();
  if (callerParamTreeSize != calleeParamTreeSize)
    return;

  auto calleeParamI = calleeArgW->tree_begin(TreeType::FORMAL_IN_TREE);
  for (; callerParamI != callerArgW->tree_end(TreeType::FORMAL_IN_TREE) && calleeParamI != calleeArgW->tree_end(TreeType::FORMAL_IN_TREE); ++callerParamI, ++calleeParamI)
  {
    if (static_cast<int>((*callerParamI)->getAccessType()) < static_cast<int>((*calleeParamI)->getAccessType()))
      (*callerParamI)->setAccessType((*calleeParamI)->getAccessType());
  }
}

bool pdg::AccessInfoTracker::getInterFuncReadWriteInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  bool reachFixPoint = true;
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
    {
      // find related instruction nodes
      auto treeNodeDepVals = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
      for (auto pair : treeNodeDepVals)
      {
        auto depInstW = pair.first->getData();
        auto depCallInsts = PDG->getNodesWithDepType(depInstW, DependencyType::DATA_CALL_PARA);
        for (auto depCallPair : depCallInsts)
        {
          auto depCallInstW = depCallPair.first->getData();
          int paraIdx = getCallParamIdx(depInstW, depCallInstW);
          auto callInst = dyn_cast<CallInst>(depCallInstW->getInstruction());
          //direct call
          if (PDG->isIndirectCallOrInlineAsm(callInst))
          {
            // Type *t = callInst->getCalledValue()->getType();
            // FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
            auto indirect_call_candidates = PDG->collectIndirectCallCandidates(callInst->getFunctionType(), importedFuncList);
            for (auto indirect_call : indirect_call_candidates)
            {
              auto calleeFuncW = pdgUtils.getFuncMap()[indirect_call];
              auto calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
              mergeArgAccessInfo(argW, calleeArgW, tree<InstructionWrapper *>::parent(treeI));
              importedFuncList.insert(indirect_call->getName().str());
              reachFixPoint = false;
            }
          }
          else
          {
            auto f = callInst->getCalledFunction();
            if (f == nullptr)
              f = dyn_cast<Function>(callInst->getCalledValue()->stripPointerCasts());
            FunctionWrapper *calleeFuncW = pdgUtils.getFuncMap()[f];
            ArgumentWrapper *calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
            mergeArgAccessInfo(argW, calleeArgW, tree<InstructionWrapper *>::parent(treeI));
            if (importedFuncList.find(f->getName().str()) == importedFuncList.end())
            {
              importedFuncList.insert(f->getName().str());
              reachFixPoint = false;
            }
          }
        }
      }
    }
  }
  return reachFixPoint;
}

int pdg::AccessInfoTracker::getCallParamIdx(const InstructionWrapper *instW, const InstructionWrapper *callInstW)
{
  Instruction *inst = instW->getInstruction();
  Instruction *callInst = callInstW->getInstruction();
  if (inst == nullptr || callInst == nullptr)
    return -1;

  if (CallInst *CI = dyn_cast<CallInst>(callInst))
  {
    int paraIdx = 0;
    for (auto arg_iter = CI->arg_begin(); arg_iter != CI->arg_end(); ++arg_iter)
    {
      if (Instruction *tmpInst = dyn_cast<Instruction>(&*arg_iter))
      {
        if (tmpInst == inst)
          return paraIdx;
      }
      paraIdx++;
    }
  }
  return -1;
}

void pdg::AccessInfoTracker::printFuncArgAccessInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  errs() << "For function: " << F.getName() << "\n";
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  for (auto argW : funcW->getArgWList())
  {
    printArgAccessInfo(argW, TreeType::FORMAL_IN_TREE);
  }
  printArgAccessInfo(funcW->getRetW(), TreeType::FORMAL_IN_TREE);
  errs() << "......... [ END " << F.getName() << " ] .........\n\n";
}

void pdg::AccessInfoTracker::printArgAccessInfo(ArgumentWrapper *argW, TreeType ty)
{
  std::vector<std::string> access_name = {
      "No Access",
      "Read",
      "Write"};

  errs() << argW->getArg()->getParent()->getName() << " Arg use information for arg no: " << argW->getArg()->getArgNo() << "\n";
  errs() << "Size of argW: " << argW->getTree(ty).size() << "\n";

  for (auto treeI = argW->tree_begin(ty);
       treeI != argW->tree_end(ty);
       ++treeI)
  {
    if ((argW->getTree(ty).depth(treeI) > EXPAND_LEVEL))
      return;
    InstructionWrapper *curTyNode = *treeI;
    if (curTyNode->getDIType() == nullptr)
      return;

    Type *parentTy = curTyNode->getParentTreeNodeType();
    Type *curType = curTyNode->getTreeNodeType();

    errs() << "Num of child: " << tree<InstructionWrapper *>::number_of_children(treeI) << "\n";

    if (parentTy == nullptr)
    {
      errs() << "** Root type node **"
             << "\n";
      errs() << "Field name: " << DIUtils::getDIFieldName(curTyNode->getDIType()) << "\n";
      errs() << "Access Type: " << access_name[static_cast<int>(curTyNode->getAccessType())] << "\n";
      errs() << dwarf::TagString(curTyNode->getDIType()->getTag()) << "\n";
      errs() << ".............................................\n";
      continue;
    }

    errs() << "sub field name: " << DIUtils::getDIFieldName(curTyNode->getDIType()) << "\n";
    errs() << "Access Type: " << access_name[static_cast<int>(curTyNode->getAccessType())] << "\n";
    errs() << dwarf::TagString(curTyNode->getDIType()->getTag()) << "\n";
    errs() << "..............................................\n";
  }
}

void pdg::AccessInfoTracker::generateRpcForFunc(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  DIType *funcRetType = DIUtils::getFuncRetDIType(F);
  std::string retTypeName;
  if (funcRetType == nullptr)
    retTypeName = "void";
  else
    retTypeName = DIUtils::getDITypeName(funcRetType);
  // handle return type
  if (PDG->isStructPointer(F.getReturnType())) // generate alloc(caller) as return struct pointer is from the other side
  {
    retTypeName = "projection " + retTypeName + "_" + F.getName().str();
    if (caller_projections.find(retTypeName) == caller_projections.end())
    {
      retTypeName += "[alloc(caller)]";
      caller_projections.insert(retTypeName);
    }
  }
  idl_file << "\trpc " << retTypeName << " " << F.getName().str() << "( ";
  // handle parameters
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    Argument &arg = *argW->getArg();
    Type *argType = arg.getType();
    auto &dbgInstList = pdgUtils.getFuncMap()[&F]->getDbgDeclareInstList();
    std::string argName = DIUtils::getArgName(arg, dbgInstList);
    if (PDG->isStructPointer(argType))
    {
      std::string argTypeName = DIUtils::getArgTypeName(arg) + "_" + F.getName().str();
      if (callee_projections.find(argTypeName) == callee_projections.end())
      {
        argTypeName = " [alloc](callee) " + argTypeName;
        callee_projections.insert(argTypeName);
      }
      idl_file << "projection " << argTypeName << " " << argName;
    }
    else if (PDG->isFuncPointer(argType)) {
      idl_file << DIUtils::getFuncSigName(DIUtils::getLowestDIType(DIUtils::getArgDIType(arg)), argName);
      // generateIDLforFuncPtr(argType, argName);
    }
    else
      idl_file << DIUtils::getArgTypeName(arg) << " " << argName;

    if (argW->getArg()->getArgNo() < F.arg_size() - 1)
      idl_file << ", ";
  }
  idl_file << " );\n\n";
}

void pdg::AccessInfoTracker::generateIDLforFunc(Function &F)
{
  // if a function is defined on the same side, no need to generate IDL rpc for this function.
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  for (auto argW : funcW->getArgWList())
  {
    generateIDLforArg(argW, TreeType::FORMAL_IN_TREE);
  }
  generateIDLforArg(funcW->getRetW(), TreeType::FORMAL_IN_TREE);
  generateRpcForFunc(F);
}

void pdg::AccessInfoTracker::generateIDLforFuncPtr(Type *ty, std::string funcName)
{
  if (PointerType *pty = dyn_cast<PointerType>(ty))
  {
    if (FunctionType *fty = dyn_cast<FunctionType>(pty->getElementType()))
    {
      auto &pdgUtils = PDGUtils::getInstance();
      auto indirectCallTargets = PDG->collectIndirectCallCandidates(fty, kernelFuncList);
      if (indirectCallTargets.size() == 0)
      {
        errs() << funcName << ": Genearting for func ptr. Call targets size is 0." << "\n";
        return;
      }
      auto mergeToFunc = indirectCallTargets[0];
      auto mergeToFuncW = pdgUtils.getFuncMap()[mergeToFunc];

      for (int i = 0; i < mergeToFunc->arg_size(); ++i)
      {
        auto mergeToArgW = mergeToFuncW->getArgWByIdx(i);
        auto mergeToArg = mergeToArgW->getArg();
        if (!PDG->isStructPointer(mergeToArg->getType()))
          continue;
        auto oldMergeToArgTree = tree<InstructionWrapper*>(mergeToArgW->getTree(TreeType::FORMAL_IN_TREE));
        for (int j = 1; j < indirectCallTargets.size(); ++j)
        {
          auto mergeFromArgW = pdgUtils.getFuncMap()[indirectCallTargets[j]]->getArgWByIdx(i);
          mergeArgAccessInfo(mergeToArgW, mergeFromArgW, mergeToArgW->tree_begin(TreeType::FORMAL_IN_TREE));
        }
        // after merging, start generating projection
        generateIDLforArg(mergeToArgW, TreeType::FORMAL_IN_TREE, funcName, false);
        // printArgAccessInfo(mergeToArgW, TreeType::FORMAL_IN_TREE);
        mergeToArgW->setTree(oldMergeToArgTree, TreeType::FORMAL_IN_TREE);
      }
    }
  }
}

void pdg::AccessInfoTracker::generateIDLforArg(ArgumentWrapper *argW, TreeType treeTy, std::string funcName, bool handleFuncPtr)
{
  std::vector<std::string> attributes = { "[in]", "[out]"};
  Function &F = *argW->getArg()->getParent();
  if (funcName.empty())
    funcName = F.getName().str();
  // an lambda funciont determining whether a pointer is a struct pointer
  std::queue<tree<InstructionWrapper*>::iterator> treeNodeQ;
  treeNodeQ.push(argW->tree_begin(treeTy));
  while (!treeNodeQ.empty())
  {
    auto treeI = treeNodeQ.front();
    treeNodeQ.pop();
    auto tyNodeW = *treeI;

    std::stringstream projection_str;
    bool accessed = false;
    for (int i = 0; i < tree<InstructionWrapper *>::number_of_children(treeI); ++i)
    {
      auto childT = tree<InstructionWrapper *>::child(treeI, i);
      auto childType = (*childT)->getTreeNodeType();
      auto childDIType = (*childT)->getDIType();

      if ((*childT)->getAccessType() == AccessType::NOACCESS)
        continue;

      accessed = true;
      if (PDG->isFuncPointer(childType))  {
        // generate rpc for the indirect function call
        projection_str << "\t\trpc " << DIUtils::getFuncSigName(DIUtils::getLowestDIType(childDIType), DIUtils::getDIFieldName(childDIType)) << ";\n";
        // generate projection for struct pointer parameters in indirect call. Each struct projection summarize all call targets' information
        if (handleFuncPtr)
          generateIDLforFuncPtr(childType, DIUtils::getDIFieldName(childDIType));
      }
      else if (PDG->isStructPointer(childType))
      {
        // for struct pointer, generate a projection
        std::string fieldTypeName = DIUtils::getDITypeName(childDIType) + "_" + funcName;
        if (callee_projections.find(fieldTypeName) == callee_projections.end())
        {
          callee_projections.insert(fieldTypeName);
          fieldTypeName = "alloc[callee] " + fieldTypeName;
        }
        projection_str << "\t\t" << "projection " << fieldTypeName << " " << DIUtils::getDIFieldName(childDIType) << ";\n";
        treeNodeQ.push(childT);
      }
      else if (childType->isStructTy())
        treeNodeQ.push(childT);
      else
        projection_str << "\t\t" + DIUtils::getDITypeName(childDIType) << " " << getAccessAttributeName(childT) << " " << DIUtils::getDIFieldName(childDIType) << ";\n";
    }

    if ((*treeI)->getTreeNodeType()->isStructTy() && accessed)
    {
      std::stringstream temp;
      std::string structName = DIUtils::getDIFieldName((*treeI)->getDIType());
      temp << "\tprojection " << "< struct " << structName << " > " << structName << "_" << funcName << " {\n"
           << projection_str.rdbuf() << "\t}\n\n";
      projection_str = std::move(temp);
    }
    else
      projection_str.str("");

    idl_file << projection_str.str();
    accessed = false;
  }
}

std::string pdg::getAccessAttributeName(tree<InstructionWrapper *>::iterator treeI)
{
  std::vector<std::string> access_attribute = {
      "No Access",
      "[in]",
      "[in, out]"};
  return access_attribute[static_cast<int>((*treeI)->getAccessType())];
}

std::string pdg::AccessInfoTracker::getArgAccessInfo(Argument &arg)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<std::string> mod_info = {"U", "R", "W", "T"};
  ArgumentWrapper *argW = pdgUtils.getFuncMap()[arg.getParent()]->getArgWByArg(arg);
  return mod_info[static_cast<int>((*argW->getTree(TreeType::FORMAL_IN_TREE).begin())->getAccessType())];
}

static RegisterPass<pdg::AccessInfoTracker>
    AccessInfoTracker("accinfo-track", "Argument access information tracking Pass", false, true);