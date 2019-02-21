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
    if (F.isDeclaration() || func_list.find(F.getName().str()) == func_list.end())
      continue;

    getIntraFuncReadWriteInfoForFunc(F);
  }

  // inter-procedure access info analysis
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration() || func_list.find(F.getName().str()) == func_list.end())
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

    if (F.isDeclaration() || func_list.find(F.getName().str()) == func_list.end())
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

void pdg::AccessInfoTracker::propergateAccessInfoToParent(ArgumentWrapper *argW, tree<InstructionWrapper *>::iterator treeI)
{
  AccessType newAccessType = (*treeI)->getAccessType();
  if (argW->getTree(TreeType::FORMAL_IN_TREE).depth(treeI) == 0)
    return;

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

void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForCallInsts(Function& Func, std::set<std::string> func_list)
{
  auto &pdgUtils = PDGUtils::getInstance();
  for (CallInst *CI : pdgUtils.getFuncMap()[&Func]->getCallInstList())
  {
    if (CI->getCalledFunction() || isa<Function>(CI->getCalledValue()->stripPointerCasts()))
    {
      std::string funcName;
      if (CI->getCalledFunction())
        funcName = CI->getCalledFunction()->getName().str();

      if (Function *F = dyn_cast<Function>(&*CI->getCalledValue()->stripPointerCasts()))
        funcName = F->getName();

      if (func_list.find(funcName) == func_list.end())
        return;

      getIntraFuncReadWriteInfoForRetVal(pdgUtils.getCallMap()[CI]);
    }
  }
}

void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForRetVal(CallWrapper *callW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  ArgumentWrapper* retW = callW->getRetW();
  auto retActualInTree = retW->getTree(TreeType::ACTUAL_IN_TREE);
  if (retActualInTree.size() == 0) {
    return;
  }
    // throw new ArgParameterTreeSizeIsZero("Argment tree is empty... Every param should have at least one node...\n");
  
  auto treeI = retW->getTree(TreeType::ACTUAL_IN_TREE).begin();

  if (!(*treeI)->getTreeNodeType()->isPointerTy())
  {
    errs() << "Find non-pointer type parameter, do not track...\n";
    return;
  }

  AccessType accessType = AccessType::NOACCESS;
  CallInst *CI = callW->getCallInst();
  InstructionWrapper *CInstW = pdgUtils.getInstMap()[CI];
  try
  {
    // 1. process the root node.
      AccessType accType = getAccessTypeForInstW(CInstW);
      (*treeI)->setAccessType(accType);

      // 2. process the underlying node. move to pointed value.
      treeI++;
      if (treeI == retW->getTree(TreeType::ACTUAL_IN_TREE).end())
        return;

      // collect load insts from the stack addr
      std::vector<InstructionWrapper *> loadInstWs;
      auto depDataList = PDG->getNodeDepList(CI);
      for (auto depPair : depDataList)
      {
        if (depPair.second == DependencyType::DATA_RAW)
        {
          InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
          loadInstWs.push_back(depInstW); // collect read instructions
        }
      }

      for (InstructionWrapper *instW : loadInstWs)
      {
        AccessType accType = getAccessTypeForInstW(instW);
        (*treeI)->setAccessType(accType);
        if (accType == AccessType::WRITE)
          break;
      }

      treeI++;
      if (treeI == retW->getTree(TreeType::ACTUAL_IN_TREE).end())
        return;
      // analysis data dep, find read/write instructions operate one the stack addr for the arg

      // 3. process each treenode(field) separately
      for (; treeI != retW->getTree(TreeType::ACTUAL_IN_TREE).end(); ++treeI)
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
        propergateAccessInfoToParent(retW, treeI);
      }
    }
  catch (std::exception &e)
  {
    errs() << e.what() << "\n";
    return;
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

    // 2. process the underlying node. move to pointed value.
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
  // for arguments
  for (auto argW : funcW->getArgWList())
  {
    try {
      getIntraFuncReadWriteInfoForArg(argW);
      collectParamCallInstWForArg(argW);
    } catch (std::exception &e) {
      errs() << e.what() << "\n";
    }
  }
  // for return value
  ArgumentWrapper *retW = funcW->getRetW();
  getIntraFuncReadWriteInfoForArg(retW);
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
      for (auto depPair : dataDList)
      {
        if (depPair.second == DependencyType::DATA_CALL_PARA)
        {
          InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
          if (reach(argStackAddrW, depInstW))
          {
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

void pdg::AccessInfoTracker::mergeTypeTreeAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper *>::iterator mergeTo, tree<InstructionWrapper *>::iterator mergeFrom)
{
  for (; mergeTo != callerArgW->tree_end(TreeType::FORMAL_IN_TREE) && mergeFrom != calleeArgW->tree_end(TreeType::FORMAL_IN_TREE); ++mergeTo, ++mergeFrom)
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
    mergeTypeTreeAccessInfo(callerArgW, calleeArgW ,callerArgW->tree_begin(TreeType::FORMAL_IN_TREE), calleeFuncArgTreeI);
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
      mergeTypeTreeAccessInfo(callerArgW, calleeArgW, callerArgWTI, calleeFuncArgTreeI);
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