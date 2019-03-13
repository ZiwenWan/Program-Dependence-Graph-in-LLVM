#include "AccessInfoTracker.hpp"

using namespace llvm;

char pdg::AccessInfoTracker::ID = 0;

bool pdg::AccessInfoTracker::runOnModule(Module &M)
{
  std::set<std::string> func_list = {
      "unregister_foobar",
      "alloc_foobardev",
      "register_foobar",
      "free_foobardev"};

  auto &pdgUtils = PDGUtils::getInstance();
  PDG = &getAnalysis<pdg::ProgramDependencyGraph>();
  
  if (!USEDEBUGINFO) {
    errs() << "[WARNING] No debug information avaliable... \nUse [-debug 1] in the pass to generate debug information\n";
    exit(0);
  }
  
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    // if (F.isDeclaration() || func_list.find(F.getName().str()) == func_list.end())
    if (F.isDeclaration())
      continue;

    getIntraFuncReadWriteInfoForFunc(F);
  }

  // inter-procedure access info analysis
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration())
      continue;

    getInterFuncReadWriteInfo(F);
  }

  std::string file_name = "accinfo";
  file_name += ".txt";
  idl_file.open(file_name);
  idl_file << "module " << M.getSourceFileName() << " {\n";

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;

    if (F.isDeclaration())
      continue;

    printFuncArgAccessInfo(F);
    generateIDLforFunc(F);
  }

  idl_file << "}";
  idl_file.close();
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
    if (depType == DependencyType::DATA_DEF_USE || depType == DependencyType::DATA_RAW)
    {
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

void pdg::AccessInfoTracker::printRetValueAccessInfo(Function &Func, std::set<std::string> func_list)
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
    throw new ArgParameterTreeSizeIsZero("Argment tree is empty... Every param should have at least one node...\n");

  auto treeI = argW->getTree(TreeType::FORMAL_IN_TREE).begin();
  if (!(*treeI)->getTreeNodeType()->isPointerTy()) {
    errs() << "Find non-pointer type parameter, do not track...\n";
    return;
  }

  AccessType accessType = AccessType::NOACCESS;
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
    for (auto valDepPair : valDepPairList) {
      auto dataW = valDepPair.first->getData();
      AccessType accType = getAccessTypeForInstW(dataW);
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

void pdg::AccessInfoTracker::mergeArgAccessInfo(ArgumentWrapper* callerArgW, ArgumentWrapper* calleeArgW, tree<InstructionWrapper *>::iterator callerParamI)
{
  unsigned callerParamTreeSize = callerArgW->getTree(TreeType::FORMAL_IN_TREE).size(callerParamI);
  unsigned calleeParamTreeSize = calleeArgW->getTree(TreeType::FORMAL_IN_TREE).size();
  errs() << callerParamTreeSize << " - " << calleeParamTreeSize << "\n";
  if ( callerParamTreeSize != calleeParamTreeSize ) {
    return;
  }

  auto calleeParamI = calleeArgW->tree_begin(TreeType::FORMAL_IN_TREE);
  for (; callerParamI != callerArgW->tree_end(TreeType::FORMAL_IN_TREE); ++callerParamI, ++calleeParamI)
  {
    if (static_cast<int>((*callerParamI)->getAccessType()) < static_cast<int>((*calleeParamI)->getAccessType()))
    {
      (*callerParamI)->setAccessType((*calleeParamI)->getAccessType());
    }
  }
}

void pdg::AccessInfoTracker::getInterFuncReadWriteInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
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
          if (Function* f = dyn_cast<Function>(callInst->getCalledValue()->stripPointerCasts()))
          {
            // direct call
            FunctionWrapper *calleeFuncW = pdgUtils.getFuncMap()[f];
            ArgumentWrapper *calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
            mergeArgAccessInfo(argW, calleeArgW, tree<InstructionWrapper*>::parent(treeI));
          } else {
            Type *t = callInst->getCalledValue()->getType();
            FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
            auto indirect_call_candidates = PDG->collectIndirectCallCandidates(funcTy);
            for (auto indirect_call : indirect_call_candidates)
            {
              auto calleeFuncW   = pdgUtils.getFuncMap()[indirect_call];
              auto calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
              mergeArgAccessInfo(argW, calleeArgW, tree<InstructionWrapper*>::parent(treeI));
            }
          }
        }
      }
    }

  }
}

int pdg::AccessInfoTracker::getCallParamIdx(const InstructionWrapper* instW, const InstructionWrapper* callInstW)
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

  errs() << "Arg use information for arg no: " << argW->getArg()->getArgNo() << "\n";
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
  DIType* funcRetType = DIUtils::getFuncRetDIType(F);
  std::string retTypeName = F.getName().str() + "_" + DIUtils::getDITypeName(DIUtils::getFuncRetDIType(F));
  // handle return type
  if (F.getReturnType()->isStructTy() || isStructPointer(F.getReturnType())) {
    retTypeName = "projection " + retTypeName;
    if (seen_projections.find(retTypeName) == seen_projections.end())
      retTypeName += " [alloc(caller)]";
    seen_projections.insert(retTypeName);
  }
  idl_file << "\trpc " << retTypeName << " " << F.getName().str() << "( ";
  // handle parameters
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList()) {
    Type* argType = argW->getArg()->getType();
    if (isStructPointer(argType) || argType->isStructTy())
      idl_file << "projection " << F.getName().str() << "_" << DIUtils::getArgTypeName(*argW->getArg()) << " " << DIUtils::getArgName(*argW->getArg());
    else
      idl_file << DIUtils::getArgTypeName(*argW->getArg()) << " " << DIUtils::getArgName(*argW->getArg());
    if (argW->getArg()->getArgNo() < F.arg_size() - 1)
      idl_file << ", ";
  }
  idl_file << " );\n\n";
}

void pdg::AccessInfoTracker::generateIDLForCallInsts(Function &F) {
  auto &pdgUtils = PDGUtils::getInstance();
  for (CallInst *CI : pdgUtils.getFuncMap()[&F]->getCallInstList())
  {
    Function* calledFunc = CI->getCalledFunction();
    if (calledFunc == nullptr)
    {
      if (Function *f = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts()))
        calledFunc = f;
      else
        return;
    }

    if (calledFunc->isDeclaration())
      continue;

    generateIDLforArg(pdgUtils.getCallMap()[CI]->getRetW(), TreeType::ACTUAL_IN_TREE);
  }
}

void pdg::AccessInfoTracker::generateIDLforFunc(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  for (auto argW : funcW->getArgWList())
  {
    generateIDLforArg(argW, TreeType::FORMAL_IN_TREE);
  }
  generateIDLforArg(funcW->getRetW(), TreeType::FORMAL_IN_TREE);
  generateRpcForFunc(F);
}

void pdg::AccessInfoTracker::generateIDLforArg(ArgumentWrapper *argW, TreeType ty)
{
  std::vector<std::string> attributes = { "[in]", "[out]"};
  Function &F = *argW->getArg()->getParent();
  // an lambda funciont determining whether a pointer is a struct pointer
  std::stringstream projection_str;
  for (auto treeI = argW->tree_begin(ty);
       treeI != argW->tree_end(ty) && treeI != nullptr;
       ++treeI)
  {
    InstructionWrapper *curTyNode = *treeI;
    if (argW->getTree(ty).depth(treeI) >= EXPAND_LEVEL)
      break;

    Type *parentTy = curTyNode->getParentTreeNodeType();
    Type *curType = curTyNode->getTreeNodeType();

    if (parentTy == nullptr)
    {
      if (!isStructPointer(curType) && !curType->isStructTy())
        return;

      if (!isStructPointer(curType) && curType->isStructTy())
        break;
      // idl for root node.
      // if (isStructPointer(curType))
      //   generateRpcForFunc(*argW->getArg()->getParent());
      // idl_file << "\tprojection <" << DIUtils::getDITypeName(curTyNode->getDIType()) << "> " << DIUtils::getDIFieldName(curTyNode->getDIType()) << " {\n";
      continue;
    }

    if ((*treeI)->getAccessType() == AccessType::NOACCESS)
      continue;
    // normal case

    if (curType->isStructTy())
    {
      int subtreeSize = argW->getTree(ty).size(treeI);
      treeI = generateIDLforStructField(argW, subtreeSize, treeI, projection_str, ty);
    }
    else
    {
        idl_file << "\t\t" << DIUtils::getDITypeName(curTyNode->getDIType()) << " " << DIUtils::getDIFieldName(curTyNode->getDIType()) << ";\n";
    }
  }
  // idl_file << "\t}\n";
  idl_file << projection_str.str();
}

tree<InstructionWrapper*>::iterator pdg::AccessInfoTracker::generateIDLforStructField(ArgumentWrapper *argW, int subtreeSize, tree<InstructionWrapper *>::iterator treeI, std::stringstream &ss, TreeType ty)
{
  InstructionWrapper *curTyNode = *treeI;
  std::string func_name = argW->getArg()->getParent()->getName().str();
  ss << "\tprojection <struct " << DIUtils::getDITypeName(curTyNode->getDIType()) << "> " << DIUtils::getDITypeName(curTyNode->getDIType()) << "_" << func_name << " {\n";
  std::stringstream projection_str;
  while (subtreeSize > 0)
  {
    treeI++;
    subtreeSize -= 1;

    if (treeI == argW->tree_end(ty))
      break;

    curTyNode = (*treeI);
    Type *curType = (*treeI)->getTreeNodeType();

    if ((*treeI)->getAccessType() == AccessType::NOACCESS)
      continue;

    if (curType->isStructTy())
    {
      treeI = generateIDLforStructField(argW, subtreeSize, treeI, projection_str, ty);
      if (treeI == argW->tree_end(ty))
        break;
    }
    else
    {
      if (isFuncPointer(curType))
        ss << "\t\trpc " << getAccessAttribute(treeI) << " " << DIUtils::getDITypeName(curTyNode->getDIType()) << " " << DIUtils::getDIFieldName(curTyNode->getDIType()) << ";\n"; 
      else if (isStructPointer(curType)) {
        std::string typeName = DIUtils::getFuncDITypeName(curTyNode->getDIType(), func_name); 
        if (seen_projections.find(typeName) == seen_projections.end()) 
          typeName += " [alloc(callee)]";
        ss << "\t\tprojection " << typeName << " " << DIUtils::getDIFieldName(curTyNode->getDIType()) << ";\n";
      }
      else
        ss << "\t\t" << DIUtils::getDITypeName(curTyNode->getDIType()) << " " << getAccessAttribute(treeI) << " " << DIUtils::getDIFieldName(curTyNode->getDIType()) << ";\n";
    }
  }
  ss << "\t}\n\n";
  ss << projection_str.str();
  return treeI;
}

bool pdg::isFuncPointer(Type *ty)
{
  if (ty->isPointerTy())
    return dyn_cast<PointerType>(ty)->getElementType()->isFunctionTy();
  return false;
}

bool pdg::isStructPointer(Type *ty)
{
  if (ty->isPointerTy())
    return ty->getPointerElementType()->isStructTy();
  return false;
}

std::string pdg::getAccessAttribute(tree<InstructionWrapper *>::iterator treeI)
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