#include "AccessInfoTracker.hpp"

using namespace llvm;

char pdg::AccessInfoTracker::ID = 0;

bool pdg::AccessInfoTracker::runOnModule(Module &M)
{
  auto &pdgUtils = PDGUtils::getInstance();
  PDG = &getAnalysis<pdg::ProgramDependencyGraph>();

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration())
    {
      continue;
    }
    getIntraFuncReadWriteInfoForFunc(F);
  }

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration())
    {
      continue;
    }
    getInterFuncReadWriteInfo(F);
  }

  std::string file_name = M.getSourceFileName();
  file_name += ".txt";
  idl_file.open(file_name);
  idl_file << M.getName().str() << " {\n";

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration())
    {
      continue;
    }
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

void pdg::AccessInfoTracker::propergateAccessInfoToParent(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator treeI)
{
  AccessType newAccessType = (*treeI)->getAccessType();
  if (argW->getTree(TreeType::FORMAL_IN_TREE).depth(treeI) == 0)
  {
    return;
  }

  auto parentTI = argW->getTree(TreeType::FORMAL_IN_TREE).parent(treeI);
  while (argW->getTree(TreeType::FORMAL_IN_TREE).depth(parentTI) != 0)
  {
    //errs() << "Traversing back to parent\n";
    (*parentTI)->setAccessType(newAccessType);
    parentTI = argW->getTree(TreeType::FORMAL_IN_TREE).parent(parentTI);
  }
  (*parentTI)->setAccessType(newAccessType);
}

AccessType pdg::AccessInfoTracker::getAccessTypeForInstW(InstructionWrapper *instW)
{
  DependencyNode<InstructionWrapper> *dataDNode = PDG->_getPDG()->getNodeByData(instW);
  DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
  AccessType accessType = AccessType::NOACCESS;
  for (auto depPair : dataDList)
  {
    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
    DependencyType depType = depPair.second;
    if (depType == DependencyType::DATA_DEF_USE)
    {
      accessType = AccessType::READ;
      // check for store instruction.
      if (StoreInst *st = dyn_cast<StoreInst>(depInstW->getInstruction()))
      {
        if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
        {
          accessType = AccessType::WRITE;
          break;
        }
      }
    }
  }
  return accessType;
}

std::vector<Instruction *> pdg::AccessInfoTracker::getArgStoreInsts(Argument &arg)
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

Instruction *pdg::AccessInfoTracker::getArgStackAddr(Argument *arg)
{
  std::vector<Instruction *> argStoreInsts = getArgStoreInsts(*arg);
  for (Instruction *inst : argStoreInsts)
  {
    StoreInst *st = cast<StoreInst>(inst);
    if (st->getValueOperand() == cast<Value>(arg))
      return inst; // the pointer operand is the address
  }
  throw new NullPtrException("Unable to get arg stack addr...");
}

std::set<InstructionWrapper *> pdg::AccessInfoTracker::getAliasForArg(ArgumentWrapper *argW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Instruction* argStackAddr = getArgStackAddr(argW->getArg());
  std::set<InstructionWrapper *> aliasPtr;
  auto dataDList = PDG->getNodeDepList(argStackAddr);
    // collect alias instructions, including store and load instructions
  for (auto depPair : dataDList)
  {
    DependencyType depType = depPair.second;
    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
    if (depType == DependencyType::DATA_ALIAS && isa<StoreInst>(depInstW->getInstruction()))
    {
      aliasPtr.insert(depInstW);
    }
  }
  return aliasPtr;
}

// intra function
void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForArg(ArgumentWrapper *argW)
{
  auto treeI = argW->getTree(TreeType::FORMAL_IN_TREE).begin();
  AccessType accessType = AccessType::NOACCESS;
  // 1. process the root parameter.
  try
  {
    Instruction *argStackAddr = getArgStackAddr(argW->getArg()); // the store inst relate to the arg store addr
    (*treeI)->setAccessType(getAccessTypeForInstW(PDGUtils::getInstance().getInstMap()[argStackAddr]));
    // 2. process the underlying node
    // collect load insts from the stack addr
    auto depDataList = PDG->getNodeDepList(argStackAddr);
    std::vector<InstructionWrapper *> argLoadInstWs;
    for (auto depPair : depDataList)
    {
      if (depPair.second == DependencyType::DATA_RAW)
      {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        argLoadInstWs.push_back(depInstW); // collect read instructions
      }
    }

    // move to pointed value.
    treeI++;
    if (treeI == argW->getTree(TreeType::FORMAL_IN_TREE).end())
      return;

    for (InstructionWrapper *instW : argLoadInstWs)
    {
      AccessType accType = getAccessTypeForInstW(instW);
      (*treeI)->setAccessType(accType);
      if (accType == AccessType::WRITE)
        break;
    }

    if (treeI == argW->getTree(TreeType::FORMAL_IN_TREE).end())
      return;
    // analysis data dep, find read/write instructions operate one the stack addr for the arg

    // 3. process each treenode(field) separately
    //accessType = AccessType::NOACCESS;
    for (; treeI != argW->getTree(TreeType::FORMAL_IN_TREE).end(); ++treeI)
    {
      // if a treenode has a correspond GetElement pointer Instruction
      if (!(*treeI)->getGEPInstW())
        continue;
      // get acces info for the gep instruction, and store access information in treenode
      AccessType gepAccessType = getAccessTypeForInstW((*treeI)->getGEPInstW());
      AccessType oldAccessInfo = (*treeI)->getAccessType();
      if (static_cast<int>(gepAccessType) < static_cast<int>(oldAccessInfo))
      {
        // if access info not changed, continue processing
        continue;
      }
      //typeNodeW->setAccessType(gepAccessType);
      (*treeI)->setAccessType(gepAccessType);
      propergateAccessInfoToParent(argW, treeI);
    }
  }
  catch (std::exception &e)
  {
    errs() << e.what() << "\n";
    return;
  }
}

void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForFunc(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  for (auto argW : funcW->getArgWList())
  {
    getIntraFuncReadWriteInfoForArg(argW);
  }
}

// inter function.
void pdg::AccessInfoTracker::collectParamCallInstWForArg(ArgumentWrapper *argW, InstructionWrapper *aliasInstW)
{
  auto dataDList = PDG->getNodeDepList(aliasInstW->getInstruction());
  for (auto depPair : dataDList)
  {
    if (depPair.second == DependencyType::DATA_CALL_PARA)
    {
      InstructionWrapper *callInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
      argW->addParamCallInstW(std::make_pair(aliasInstW, callInstW));
    }
  }
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

void pdg::AccessInfoTracker::mergeTypeTreeAccessInfo(ArgumentWrapper *callerArgW, tree<InstructionWrapper *>::iterator mergeTo, tree<InstructionWrapper *>::iterator mergeFrom)
{
  for (; mergeTo != callerArgW->tree_end(TreeType::FORMAL_IN_TREE); ++mergeTo, ++mergeFrom)
  {
    if ((*mergeFrom)->getAccessType() > (*mergeTo)->getAccessType())
    {
      // here, we only copy the write state
      (*mergeTo)->setAccessType((*mergeFrom)->getAccessType());
      propergateAccessInfoToParent(callerArgW, mergeTo);
    }
  }
}

void pdg::AccessInfoTracker::mergeArgWAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW)
{
  ArgumentMatchType argMatchType = getArgMatchType(callerArgW->getArg(), calleeArgW->getArg());
  auto calleeFuncArgTreeI = calleeArgW->getTree(TreeType::FORMAL_IN_TREE).begin();

  if (argMatchType == ArgumentMatchType::EQUAL)
  {
    mergeTypeTreeAccessInfo(callerArgW, callerArgW->tree_end(TreeType::FORMAL_IN_TREE), calleeFuncArgTreeI);
  }

  if (argMatchType == ArgumentMatchType::CONTAINED)
  {
    auto callerArgWTI = callerArgW->tree_begin(TreeType::FORMAL_IN_TREE);
    auto callerArgWTIE = callerArgW->tree_end(TreeType::FORMAL_IN_TREE);
    for (; callerArgWTI != callerArgWTIE; ++callerArgWTI)
    {
      InstructionWrapper *gepInstW = (*callerArgWTI)->getGEPInstW();
      if (gepInstW == nullptr)
      {
        continue;
      }
      mergeTypeTreeAccessInfo(callerArgW, callerArgWTI, calleeFuncArgTreeI);
      return;
    }
  }
}

void pdg::AccessInfoTracker::getInterFuncReadWriteInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    for (auto paramCallInstPair : argW->getParamCallInstPair())
    {
      int paraIdx = getCallParamIdx(paramCallInstPair.first, paramCallInstPair.second);
      CallInst *calleeInst = dyn_cast<CallInst>(paramCallInstPair.second->getInstruction());
      Function *calledFunction = calleeInst->getCalledFunction();

      // handle indirect call
      if (calledFunction)
      {
        Type *t = calleeInst->getCalledValue()->getType();
        FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
        std::vector<llvm::Function *> indirect_call_candidates = PDG->collectIndirectCallCandidates(funcTy);
        for (llvm::Function *indirect_call : indirect_call_candidates)
        {
          FunctionWrapper *calleeFuncW = pdgUtils.getFuncMap()[indirect_call];
          ArgumentWrapper *calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
          mergeArgWAccessInfo(argW, calleeArgW);
        }
      }
      else
      {
        // direct call
        FunctionWrapper *calleeFuncW = pdgUtils.getFuncMap()[calledFunction];
        ArgumentWrapper *calleeArgW = calleeFuncW->getArgWByIdx(paraIdx);
        mergeArgWAccessInfo(argW, calleeArgW);
      }
    }
  }
}

void pdg::AccessInfoTracker::printFuncArgAccessInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  errs() << "For function: " << F.getName() << "\n";
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    printArgAccessInfo(argW);
  }
  errs() << "......... [ END " << F.getName() << " ] .........\n";
}

void pdg::AccessInfoTracker::printArgAccessInfo(ArgumentWrapper *argW)
{
  std::vector<std::string> access_name = {
      "No Access",
      "Read",
      "Write"};

  errs() << "Arg use information for arg no: " << argW->getArg()->getArgNo() << "\n";
  errs() << "Size of argW: " << argW->getTree(TreeType::FORMAL_IN_TREE).size() << "\n";

  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE);
       treeI != argW->tree_end(TreeType::FORMAL_IN_TREE);
       ++treeI)
  {
    InstructionWrapper *curTyNode = *treeI;
    Type *parentTy = curTyNode->getParentTreeNodeType();
    Type *curType = curTyNode->getTreeNodeType();

    errs() << "Num of child: " << tree<InstructionWrapper *>::number_of_children(treeI) << "\n";

    if (parentTy == nullptr)
    {
      errs() << "** Root type node **"
             << "\n";
      errs() << "Field name: " << curTyNode->getDIType()->getName().str()  << "\n";
      errs() << "Access Type: " << access_name[static_cast<int>(curTyNode->getAccessType())] << "\n";
      errs() << ".............................................\n";
      continue;
    }

    errs() << "sub field name: " << curTyNode->getDIType()->getName().str() << "\n";
    errs() << "Type name: " << getTypeNameByTag(curTyNode->getDIType()) << "\n";
    errs() << "Access Type: " << access_name[static_cast<int>(curTyNode->getAccessType())] << "\n";
    errs() << "..............................................\n";
  }
}

void pdg::AccessInfoTracker::generateIDLforFunc(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    generateIDLforArg(argW);
  }
}

void pdg::AccessInfoTracker::generateIDLforArg(ArgumentWrapper *argW)
{
  // TODO: add correct attribute later
  std::vector<std::string> attributes = {
      "[in]",
      "[out]"};

  // an lambda funciont determining whether a pointer is a struct pointer
  std::stringstream projection_str;
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE);
       treeI != argW->tree_end(TreeType::FORMAL_IN_TREE);
       ++treeI)
  {
    InstructionWrapper *curTyNode = *treeI;
    Type *parentTy = curTyNode->getParentTreeNodeType();
    Type *curType = curTyNode->getTreeNodeType();

    if (parentTy == nullptr)
    {
      // idl for root node.
      idl_file << "project <struct " << curTyNode->getDIType()->getName().str() << "> " << curTyNode->getDIType()->getName().str() << "{\n";
      continue;
    }

    if ((*treeI)->getAccessType() == AccessType::NOACCESS)
    {
      continue;
    }

    if (isStructPointer(curType))
    {
      int subtreeSize = argW->getTree(TreeType::FORMAL_IN_TREE).size(treeI);
      generateIDLforStructField(subtreeSize - 1, treeI, projection_str);
      continue;
    }

    // normal case
    idl_file << "\t" << getTypeNameByTag(curTyNode->getDIType()) << " " << curTyNode->getDIType()->getName().str() << ";\n";
  }
  idl_file << "}";
  idl_file << projection_str.str();
}

void pdg::AccessInfoTracker::generateIDLforStructField(int subtreeSize, tree<InstructionWrapper *>::iterator &treeI, std::stringstream &ss)
{
  InstructionWrapper* curTyNode = *treeI;
  ss << "\nproject <struct " << curTyNode->getDIType()->getName().str() << "> " << curTyNode->getDIType()->getName().str() << "\n";
  while (subtreeSize > 0)
  {
    treeI++;
    subtreeSize -= 1;
    Type *curType = (*treeI)->getTreeNodeType();

    if ((*treeI)->getAccessType() == AccessType::NOACCESS)
      continue;

    if (isStructPointer(curType))
    {
      generateIDLforStructField(subtreeSize - 1, treeI, ss);
      continue;
    }
    ss << "\t" << getTypeNameByTag(curTyNode->getDIType()) << " " << curTyNode->getDIType()->getName().str() << ";\n";
    // update all status
  }
  ss << "}\n";
}

bool pdg::isStructPointer(Type *ty)
{
  if (ty->isPointerTy())
  {
    return ty->getPointerElementType()->isStructTy();
  }
  return false;
}

std::string pdg::getTypeNameByTag(DIType *ty)
{
  if (!ty->getName().str().empty())
  {
    return ty->getName().str();
  }
  if (ty->getTag() == dwarf::DW_TAG_member)
  {
    ty = dyn_cast<DIDerivedType>(ty)->getBaseType().resolve();
  }
  switch (ty->getTag())
  {
  case dwarf::DW_TAG_array_type:
    ty = dyn_cast<DICompositeType>(ty)->getBaseType().resolve();
    return ty->getName().str() + " *";
  default:
    return "unknow type ";
  }
}

static RegisterPass<pdg::AccessInfoTracker>
    AccessInfoTracker("accinfo-track", "Argument access information tracking Pass", false, true);