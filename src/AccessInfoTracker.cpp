#include "AccessInfoTracker.hpp"

using namespace llvm;

char pdg::AccessInfoTracker::ID = 0;

bool pdg::AccessInfoTracker::runOnModule(Module &M)
{
  auto &pdgUtils = PDGUtils::getInstance();
  PDG = &getAnalysis<pdg::ProgramDependencyGraph>();

  if (!USEDEBUGINFO) {
    errs() << "[WARNING] No debug information avaliable... \nUse [-debug 1] in the pass to generate debug information\n";
    exit(0);
  }

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
  auto dataDList = PDG->getNodeDepList(instW->getInstruction());
  AccessType accessType = AccessType::NOACCESS;
  for (auto depPair : dataDList)
  {
    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
    DependencyType depType = depPair.second;
    // if (depInstW->getInstruction())
    //   errs() << instW->getInstruction()->getFunction()->getName() << *depInstW->getInstruction() << "\n";
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

std::set<InstructionWrapper *> pdg::AccessInfoTracker::getAliasLoadInstsForArg(ArgumentWrapper *argW)
{
    std::set<InstructionWrapper *> argAliasLoadInstsSet;
    std::vector<Instruction* > argStackAddrs = getArgStackAddrs(argW->getArg());
    std::set<InstructionWrapper *> argAliasStoreInstsSet = getAliasStoreInstsForArg(argW);
    for (Instruction *argStackAddr : argStackAddrs)
    {
      auto depDataList = PDG->getNodeDepList(argStackAddr);
      for (auto depPair : depDataList)
      {
        if (depPair.second == DependencyType::DATA_RAW)
        {
          InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
          argAliasLoadInstsSet.insert(depInstW); // collect read instructions
        }
      }
    }
    return argAliasLoadInstsSet;
}

std::vector<Instruction*> pdg::AccessInfoTracker::getArgStackAddrs(Argument *arg)
{
  std::vector<Instruction *> argStoreInsts = getArgStoreInsts(*arg);
  std::vector<Instruction *> argStackAddrs;
  for (Instruction *inst : argStoreInsts)
  {
    StoreInst *st = cast<StoreInst>(inst);
    if (st->getValueOperand() == cast<Value>(arg))
      argStackAddrs.push_back(st); // the pointer operand is the address
  }
  return argStackAddrs;
}

std::set<InstructionWrapper *> pdg::AccessInfoTracker::getAliasStoreInstsForArg(ArgumentWrapper *argW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<Instruction *> argStackAddrs = getArgStackAddrs(argW->getArg());
  std::set<InstructionWrapper *> aliasPtr;
  for (Instruction *argStackAddr : argStackAddrs)
  {
    auto dataDList = PDG->getNodeDepList(argStackAddr);
    // collect alias instructions, including store and load instructions
    aliasPtr.insert(pdgUtils.getInstMap()[argStackAddr]);
    for (auto depPair : dataDList)
    {
      DependencyType depType = depPair.second;
      InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
      if (depType == DependencyType::DATA_ALIAS && isa<StoreInst>(depInstW->getInstruction()))
      {
        aliasPtr.insert(depInstW);
      }
    }
  }
  return aliasPtr;
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
  try
  {
   // 1. process the root parameter.
    std::vector<Instruction* > argStackAddrs = getArgStackAddrs(argW->getArg());
    std::set<InstructionWrapper *> argAliasStoreInstsSet = getAliasStoreInstsForArg(argW);
    for (InstructionWrapper *argStackAddrW : argAliasStoreInstsSet) // tracking for the pointer itself. May not quick useful when doing value passing, but do it for completeness
    {
      AccessType accType = getAccessTypeForInstW(argStackAddrW);
      (*treeI)->setAccessType(accType);
      if (accType == AccessType::WRITE)
        break;
    }

    if (argAliasStoreInstsSet.size() > 1) {
      (*treeI)->setAccessType(AccessType::WRITE); // means there are multiple writes to the address storing the pointer
    }

    // 2. process the underlying node
    // move to pointed value.
    treeI++;
    if (treeI == argW->getTree(TreeType::FORMAL_IN_TREE).end())
      return;

    // collect load insts from the stack addr
    std::vector<InstructionWrapper *> argLoadInstWs;
    for (Instruction *argStackAddr : argStackAddrs)
    {
      auto depDataList = PDG->getNodeDepList(argStackAddr);
      for (auto depPair : depDataList)
      {
        if (depPair.second == DependencyType::DATA_RAW)
        {
          InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
          argLoadInstWs.push_back(depInstW); // collect read instructions
        }
      }
    }

    for (InstructionWrapper *instW : argLoadInstWs)
    {
      AccessType accType = getAccessTypeForInstW(instW);
      (*treeI)->setAccessType(accType);
      if (accType == AccessType::WRITE)
        break;
    }

    treeI++;
    if (treeI == argW->getTree(TreeType::FORMAL_IN_TREE).end())
      return;
    // analysis data dep, find read/write instructions operate one the stack addr for the arg

    // 3. process each treenode(field) separately
    for (; treeI != argW->getTree(TreeType::FORMAL_IN_TREE).end(); ++treeI)
    {
      // if a treenode has a correspond GetElement pointer Instruction
      if (!(*treeI)->getGEPInstW())
        continue;
      // get acces info for the gep instruction, and store access information in treenode
      AccessType gepAccessType = getAccessTypeForInstW((*treeI)->getGEPInstW());
      AccessType oldAccessInfo = (*treeI)->getAccessType();
      if (static_cast<int>(gepAccessType) < static_cast<int>(oldAccessInfo))
        continue; // if access info not changed, continue processing

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
    try {
      getIntraFuncReadWriteInfoForArg(argW);
      collectParamCallInstWForArg(argW);
    } catch (std::exception &e) {
      errs() << e.what() << "\n";
    }
  }
}

bool pdg::AccessInfoTracker::reach(InstructionWrapper *instW1, InstructionWrapper *instW2)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::queue<InstructionWrapper *> instWQ;
  std::set<InstructionWrapper *> seenInstW;
  instWQ.push(instW1);
  seenInstW.insert(instW1);

  auto dataDList = PDG->getNodeDepList(instW1->getInstruction());
  for (auto depPair : dataDList)
  {
    DependencyType depType = depPair.second;
    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
    if (depInstW == instW2)
      return true;
    
    if (seenInstW.find(depInstW) == seenInstW.end())
    {
      instWQ.push(depInstW);
      seenInstW.insert(depInstW);
    }
  }

  while (!instWQ.empty())
  {
    InstructionWrapper *instW = instWQ.front();
    instWQ.pop();

    if (instW->getInstruction() == instW2->getInstruction())
      return true;

    auto dataDList = PDG->getNodeDepList(instW->getInstruction());
    for (auto depPair : dataDList)
    {
      DependencyType depType = depPair.second;
      if (depType == DependencyType::DATA_DEF_USE || depType == DependencyType::DATA_RAW)
      {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        if (seenInstW.find(depInstW) == seenInstW.end())
        {
          instWQ.push(depInstW);
          seenInstW.insert(depInstW);
        }
      }
    }
  }

  return false;
}

// inter function.
void pdg::AccessInfoTracker::collectParamCallInstWForArg(ArgumentWrapper *argW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  Function* func = argW->getArg()->getParent();
  std::vector<CallInst* > funcCallInsts = pdgUtils.getFuncMap()[func]->getCallInstList();
  for (auto argStackAddr : getArgStackAddrs(argW->getArg()))
  {
    for (CallInst *callinst : funcCallInsts) {
      InstructionWrapper *argStackAddrW = pdgUtils.getInstMap()[argStackAddr];
      InstructionWrapper *callInstW = pdgUtils.getInstMap()[callinst];
      auto dataDList = PDG->getNodeDepList(callinst);
      for (auto depPair : dataDList) {
        if (depPair.second == DependencyType::DATA_CALL_PARA) {
          InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
          if (reach(argStackAddrW, depInstW)) {
            // check if value could flow from argument to the parameter, if so make a pair between arg stored stack and the call inst
            argW->addParamCallInstW(std::make_pair(depInstW, callInstW));
          }
        }
      }
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
    if (static_cast<int>((*mergeFrom)->getAccessType()) > static_cast<int>((*mergeTo)->getAccessType()))
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
    mergeTypeTreeAccessInfo(callerArgW, callerArgW->tree_begin(TreeType::FORMAL_IN_TREE), calleeFuncArgTreeI);
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
      if (!calledFunction)
      {
        errs() << "Handling indr call" << "\n";
        Type *t = calleeInst->getCalledValue()->getType();
        FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
        std::vector<llvm::Function *> indirect_call_candidates = PDG->collectIndirectCallCandidates(funcTy);
        errs() << indirect_call_candidates.size() << "\n";
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

int pdg::AccessInfoTracker::getCallParamIdx(InstructionWrapper* instW, InstructionWrapper* callInstW)
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
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    printArgAccessInfo(argW);
  }
  errs() << "......... [ END " << F.getName() << " ] .........\n\n";
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
    if ((argW->getTree(TreeType::FORMAL_IN_TREE).depth(treeI) >= EXPAND_LEVEL))
      return;

    InstructionWrapper *curTyNode = *treeI;
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
    if (argW->getTree(TreeType::FORMAL_IN_TREE).depth(treeI) >= EXPAND_LEVEL)
      return;
    Type *parentTy = curTyNode->getParentTreeNodeType();
    Type *curType = curTyNode->getTreeNodeType();

    if (parentTy == nullptr)
    {
      // idl for root node.
      idl_file << "project <struct " << DIUtils::getDITypeName(curTyNode->getDIType()) << "> " << DIUtils::getDIFieldName(curTyNode->getDIType()) << "{\n";
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
    idl_file << "\t" << DIUtils::getDITypeName(curTyNode->getDIType()) << " " << curTyNode->getDIType()->getName().str() << ";\n";
  }
  idl_file << "}\n";
  idl_file << projection_str.str();
}

void pdg::AccessInfoTracker::generateIDLforStructField(int subtreeSize, tree<InstructionWrapper *>::iterator &treeI, std::stringstream &ss)
{
  InstructionWrapper *curTyNode = *treeI;
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
    ss << "\t" << DIUtils::getDITypeName(curTyNode->getDIType()) << " " << DIUtils::getDIFieldName(curTyNode->getDIType()) << ";\n";
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

std::string pdg::AccessInfoTracker::getArgAccessInfo(Argument &arg)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<std::string> mod_info = {"U", "R", "W", "T"};
  ArgumentWrapper *argW = pdgUtils.getFuncMap()[arg.getParent()]->getArgWByArg(arg);
  return mod_info[static_cast<int>((*argW->getTree(TreeType::FORMAL_IN_TREE).begin())->getAccessType())];
}

static RegisterPass<pdg::AccessInfoTracker>
    AccessInfoTracker("accinfo-track", "Argument access information tracking Pass", false, true);