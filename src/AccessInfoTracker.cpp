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

  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    Function &F = *FI;
    if (F.isDeclaration()) {
      continue;
    }
    printFuncArgAccessInfo(F);
  }
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

std::set<InstructionWrapper *> pdg::AccessInfoTracker::getAliasForArg(ArgumentWrapper *argW)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<Instruction *> initialStoreInsts = getArgStoreInsts(*(argW->getArg()));
  std::set<InstructionWrapper *> aliasPtr;
  for (Instruction *storeInst : initialStoreInsts)
  {
    DependencyNode<InstructionWrapper> *dataDNode = PDG->_getPDG()->getNodeByData(pdgUtils.getInstMap()[storeInst]);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    // collect alias instructions, including store and load instructions
    for (auto depPair : dataDList)
    {
      DependencyType depType = depPair.second;
      InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
      if (depType == DependencyType::DATA_ALIAS)
      {
        aliasPtr.insert(depInstW);
      }
    }

    // collect alais gep instructions
    std::set<InstructionWrapper *> relevantGEPs = PDG->getAllRelevantGEP(*(argW->getArg()));
    aliasPtr.insert(relevantGEPs.begin(), relevantGEPs.end());
  }

  return aliasPtr;
}

// intra function
void pdg::AccessInfoTracker::getIntraFuncReadWriteInfoForArg(ArgumentWrapper *argW)
{
  auto treeI = argW->getTree(TreeType::FORMAL_IN_TREE).begin();
  AccessType accessType = AccessType::NOACCESS;

  // 1. processing the root node, which represent the argument as a whole
  std::vector<Instruction *> initialStoreInsts = getArgStoreInsts(*(argW->getArg()));
  for (Instruction *storeInst : initialStoreInsts)
  {
    if (StoreInst *st = dyn_cast<StoreInst>(storeInst))
    {
      if (!isa<Instruction>(st->getValueOperand()))
      {
        accessType = AccessType::WRITE;
        (*treeI)->setAccessType(accessType);
        break;
      }
    }
  }

  // 2. processing alias instruction for root
  std::set<InstructionWrapper *> aliasPtrInstWs = getAliasForArg(argW);
  for (InstructionWrapper *instW : aliasPtrInstWs)
  {
    // same here as single ptr
    if (StoreInst *st = dyn_cast<StoreInst>(instW->getInstruction()))
    {
      if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
      {
        accessType = AccessType::WRITE;
        break;
      }
    }

    AccessType _accessType = getAccessTypeForInstW(instW);
    if (_accessType == AccessType::READ)
    {
      accessType = _accessType;
    }
    if (_accessType == AccessType::WRITE)
    {
      accessType = _accessType;
      break;
    }
  }

  (*treeI)->setAccessType(accessType);

  // 3. process each treenode(field) separately
  accessType = AccessType::NOACCESS;
  for (; treeI != argW->getTree(TreeType::FORMAL_IN_TREE).end(); ++treeI)
  {
    // if a treenode has a correspond GetElement pointer Instruction
    if ((*treeI)->getGEPInstW())
    {
      // get acces info for the gep instruction, and store access information in treenode
      AccessType gepAccessType = getAccessTypeForInstW((*treeI)->getGEPInstW());
      AccessType oldAccessInfo = (*treeI)->getAccessType();
      if (gepAccessType < oldAccessInfo)
      {
        // if access info not changed, continue processing
        continue;
      }
      //typeNodeW->setAccessType(gepAccessType);
      (*treeI)->setAccessType(gepAccessType);
      propergateAccessInfoToParent(argW, treeI);
    }
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
  DependencyNode<InstructionWrapper> *dataDNode = PDG->_getPDG()->getNodeByData(aliasInstW);
  DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
  for (auto depPair : dataDList)
  {
    if (depPair.second == DependencyType::DATA_CALL_PARA)
    {
      InstructionWrapper *callInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
      argW->addParamCallInstW(std::make_pair (aliasInstW, callInstW));
    }
  }
}

pdg::ArgumentMatchType getArgMatchType(Argument *arg1, Argument *arg2)
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
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    errs() << "For Func: " << F.getName() << "\n";
    printArgAccessInfo(argW);
  }
}

void pdg::AccessInfoTracker::printArgAccessInfo(ArgumentWrapper *argW)
{
  std::vector<std::string> access_name = {
      "No Access",
      "Read",
      "Write"};
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE);
       treeI != argW->tree_end(TreeType::FORMAL_IN_TREE);
       ++treeI)
  {
    int acc_no = static_cast<int>((*treeI)->getAccessType());
    errs() << access_name[acc_no] << "\n";
  }
}

static RegisterPass<pdg::AccessInfoTracker>
    AccessInfoTracker("accinfo-track", "Argument access information tracking Pass", false, true);