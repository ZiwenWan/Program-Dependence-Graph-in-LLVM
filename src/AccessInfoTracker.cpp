#include "AccessInfoTracker.hpp"

using namespace llvm;

char pdg::AccessInfoTracker::ID = 0;

bool pdg::AccessInfoTracker::runOnModule(Module &M) {
  module = &M;
  PDG = &getAnalysis<ProgramDependencyGraph>();
  auto &pdgUtils = PDGUtils::getInstance();
  // get cross domain functions and setup kernel and driver side functions
  std::set<Function *> crossDomainFuncCalls = pdgUtils.computeCrossDomainFuncs(M);
  driverDomainFuncs = pdgUtils.computeDriverDomainFuncs(M);
  kernelDomainFuncs = pdgUtils.computeKernelDomainFuncs(M);
  driverExportFuncPtrNames = pdgUtils.computeDriverExportFuncPtrName();
  driverExportFuncPtrNameMap = pdgUtils.computeDriverExportFuncPtrNameMap();
  asyncCalls = PDG->inferAsynchronousCalledFunction();
  stringOperations.insert("strcpy");
  stringOperations.insert("strlen");
  stringOperations.insert("strlcpy");
  stringOperations.insert("strcmp");
  stringOperations.insert("strncmp");
  // start generating IDL
  computeSharedDataForGlobalVars();
  std::string file_name = "kernel";
  file_name += ".idl";
  idl_file.open(file_name);
  idl_file << "module kernel"
           << " {\n";
  for (Function *F : crossDomainFuncCalls)
  {
    if (F->isDeclaration() || F->empty())
      continue;
    computeFuncAccessInfo(*F);
    computeSharedDataInFunc(*F);
    generateIDLforFunc(*F);
  }
  idl_file << "}";
  idl_file.close();
  return false;
}

void pdg::AccessInfoTracker::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<pdg::ProgramDependencyGraph>();
  AU.addRequired<CallGraphWrapperPass>();
  AU.setPreservesAll();
}

std::string pdg::AccessInfoTracker::getRegisteredFuncPtrName(std::string funcName)
{
  if (driverExportFuncPtrNameMap.find(funcName) != driverExportFuncPtrNameMap.end())
    return driverExportFuncPtrNameMap[funcName];
  return funcName;
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
      // if (isa<LoadInst>(depInstW->getInstruction()) || isa<GetElementPtrInst>(depInstW->getInstruction()))
      accessType = AccessType::READ;
    }

    // check for store instruction.
    if (depType == DependencyType::DATA_DEF_USE)
    {
      if (StoreInst *st = dyn_cast<StoreInst>(depInstW->getInstruction()))
      {
        // if a value is used in a store instruction and is the store destination
        if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
        {
          if (isa<Argument>(st->getValueOperand())) // ignore the store inst that store arg to stack mem
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

void pdg::AccessInfoTracker::computeSharedDataForGlobalVars()
{
  auto globalObjectTrees = PDG->getGlobalObjectTrees();
  auto &pdgUtils = PDGUtils::getInstance();
  auto instMap = pdgUtils.getInstMap();
  for (auto pair : globalObjectTrees)
  {
    auto GV = pair.first;
    auto objectTree = pair.second;
    DIType* GVDIType = DIUtils::getGlobalVarDIType(*GV);
    if (!GVDIType)
      continue;
    std::set<std::string> accessedFields;
    // iterate through each node, find their addr variables and then analyze the accesses to the addr variables
    for (auto treeI = objectTree.begin(); treeI != objectTree.end(); ++treeI)
    {
      DIType *fieldDIType = (*treeI)->getDIType();
      // hanlde static defined functions, assume all functions poineter that are linked with defined function in device driver module should be used by kernel.
      if (DIUtils::isFuncPointerTy((*treeI)->getDIType()))
      {
        std::string funcptrName = DIUtils::getDIFieldName((*treeI)->getDIType());
        if (driverExportFuncPtrNames.find(funcptrName) != driverExportFuncPtrNames.end())
        {
          std::string fieldID = DIUtils::computeFieldID(GVDIType, fieldDIType);
          accessedFields.insert(fieldID);
        }
        continue;
      }

      // get valdep pair, and check for intraprocedural accesses
      auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
      for (auto valDepPair : valDepPairList)
      {
        auto dataW = valDepPair.first->getData();
        AccessType accType = getAccessTypeForInstW(dataW);
        if (accType != AccessType::NOACCESS)
        {
          std::string fieldID = DIUtils::computeFieldID(GVDIType, fieldDIType);
          accessedFields.insert(fieldID);
          // compute field accessed in asynchronous called functions
          Instruction* inst = dataW->getInstruction();
          if (inst != nullptr)
          {
            Function* parentFunc = inst->getFunction();
            if (asyncCalls.find(parentFunc) != asyncCalls.end())
              accessedFieldsInAsyncCalls.insert(fieldID);
          }
        }
      }
    }

    std::string GVName = DIUtils::getDITypeName(GVDIType);
    if (sharedDataTypeMap.find(GVName) == sharedDataTypeMap.end())
      sharedDataTypeMap[GVName] = accessedFields;
    else 
      sharedDataTypeMap[GVName].insert(accessedFields.begin(), accessedFields.end());
  }
}

void pdg::AccessInfoTracker::computeArgAccessInfo(ArgumentWrapper* argW, TreeType treeTy)
{
  auto argTree = argW->getTree(treeTy);
  if (argTree.size() == 0)
    return;
  // assert(argTree.size() != 0 && "find empty formal tree. Stop computing arg access info");
  auto func = argW->getArg()->getParent();
  auto treeI = argW->getTree(treeTy).begin();
  // check for debugging information availability
  if ((*treeI)->getDIType() == nullptr)
  {
    errs() << "Empty debugging info for " << func->getName() << " - " << argW->getArg()->getArgNo() << "\n";
    return;
  }
  // we compute access information only for pointers
  if ((*treeI)->getDIType()->getTag() != dwarf::DW_TAG_pointer_type)
  {
    errs() << func->getName() << " - " << argW->getArg()->getArgNo() << " Find non-pointer type parameter, do not track...\n";
    return;
  }

  computeIntraprocArgAccessInfo(argW, *func);
  computeInterprocArgAccessInfo(argW, *func);
}

// intraprocedural
void pdg::AccessInfoTracker::computeIntraprocArgAccessInfo(ArgumentWrapper *argW, Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto instMap = pdgUtils.getInstMap();
  // iterate through each node, find their addr variables and then analyze the accesses to the addr variables
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    // hanlde static defined functions, assume all functions poineter that are linked with defined function in device driver module should be used by kernel.
    if (DIUtils::isFuncPointerTy((*treeI)->getDIType()))
    {
      std::string funcptrName = DIUtils::getDIFieldName((*treeI)->getDIType());
      if (driverExportFuncPtrNames.find(funcptrName) != driverExportFuncPtrNames.end())
      {
        (*treeI)->setAccessType(AccessType::READ);
        continue;
      }
    }

    // get valdep pair, and check for intraprocedural accesses
    auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
    for (auto valDepPair : valDepPairList)
    {
      auto dataW = valDepPair.first->getData();
      AccessType accType = getAccessTypeForInstW(dataW);
      if (static_cast<int>(accType) > static_cast<int>((*treeI)->getAccessType()))
        (*treeI)->setAccessType(accType);
    }
  }
}

void pdg::AccessInfoTracker::computeInterprocArgAccessInfo(ArgumentWrapper* argW, Function &F)
{
  errs() << "start computing inter proc info for: " << F.getName() << " - " << argW->getArg()->getArgNo() << "\n";
  auto &pdgUtils = PDGUtils::getInstance();
  auto instMap = pdgUtils.getInstMap();
  std::map<std::string, AccessType> interprocAccessFieldMap;
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
    for (auto valDepPair : valDepPairList)
    {
      auto dataW = valDepPair.first->getData();
      // compute interprocedural access info in the receiver domain
      auto depNodePairs = PDG->getNodesWithDepType(dataW, DependencyType::DATA_CALL_PARA);
      for (auto depNodePair : depNodePairs)
      {
        auto depInstW = depNodePair.first->getData();
        if (CallInst *ci = dyn_cast<CallInst>(depInstW->getInstruction()))
        {
          int callArgIdx = getCallOperandIdx(dataW->getInstruction(), ci);
          if (callArgIdx < 0) // invalid idx
            continue;
          if (Function *calledFunc = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
          {
            if (calledFunc->isDeclaration() || calledFunc->empty())
              continue;
            auto accessFieldMap = computeInterprocAccessedFieldMap(*calledFunc, callArgIdx);
            interprocAccessFieldMap.insert(accessFieldMap.begin(), accessFieldMap.end());
          }
        }
      }
    }
  }

  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    DIType* parentDIType = nullptr;
    DIType* curNodeDIType = (*treeI)->getDIType();
    if (tree<InstructionWrapper *>::depth(treeI) != 0)
    {
      auto ParentI = tree<InstructionWrapper *>::parent(treeI);
      parentDIType = (*ParentI)->getDIType();
    }
    std::string fieldId = DIUtils::computeFieldID(parentDIType, curNodeDIType);
    if (interprocAccessFieldMap.find(fieldId) != interprocAccessFieldMap.end())
      (*treeI)->setAccessType(interprocAccessFieldMap[fieldId]);
  }
}

int pdg::AccessInfoTracker::getCallOperandIdx(Value* operand, CallInst* callInst)
{
  int argNo = 0;
  for (auto arg_iter = callInst->arg_begin(); arg_iter != callInst->arg_end(); ++arg_iter)
  {
    if (Instruction *tmpInst = dyn_cast<Instruction>(&*arg_iter))
    {
      if (tmpInst == operand)
        return argNo;
    }
    argNo++;
  }
  if (argNo == callInst->getNumArgOperands())
    return -1;
  return argNo;
}

// void pdg::AccessInfoTracker::computeInterprocArgAccessInfo(ArgumentWrapper* argW, Function &F, std::set<Function*> receiverDomainTrans)
// {
//   errs() << "start computing inter proc info for: " << F.getName() << " - " << argW->getArg()->getArgNo() << "\n";
//   auto &pdgUtils = PDGUtils::getInstance();
//   auto instMap = pdgUtils.getInstMap();
//   for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
//   {
//     // if has an address variable, then simply check if any alias exists in the transitive closure.
//     auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
//     for (auto valDepPair : valDepPairList)
//     {
//       auto dataW = valDepPair.first->getData();
//       // compute interprocedural access info in the receiver domain
//       std::set<Value *> aliasSet = findAliasInDomain(*(dataW->getInstruction()), F, receiverDomainTrans);
//       for (auto alias : aliasSet)
//       {
//         if (Instruction *i = dyn_cast<Instruction>(alias))
//         {
//           // analyze alias read/write info
//           auto aliasW = instMap[i];
//           AccessType aliasAccType = getAccessTypeForInstW(aliasW);
//           if (static_cast<int>(aliasAccType) > static_cast<int>((*treeI)->getAccessType()))
//             (*treeI)->setAccessType(aliasAccType);
//         }
//       }
//     }

//     // if the list is empty, then this node doesn't has any address variable inside a function
//     if (valDepPairList.size() != 0)
//       continue;
//     // no need for checking the root node 
//     if (tree<InstructionWrapper*>::depth(treeI) == 0)
//       continue;
//     //  get parent node
//     auto parentI = tree<InstructionWrapper*>::parent(treeI);
//     // check if parent is a struct 
//     DIType* parentDIType = (*parentI)->getDIType();
//     if (!DIUtils::isStructTy(parentDIType))
//       continue;
//     auto parentValDepList = PDG->getNodesWithDepType(*parentI, DependencyType::VAL_DEP);
//     if (parentValDepList.size() == 0)
//       continue;
    
//     unsigned childIdx = (*treeI)->getNodeOffset();
//     auto dataW = parentValDepList.front().first->getData();
//     assert(dataW->getInstruction() && "cannot find parent instruction while computing interproce access info.");
//     // get struct layout
//     DIType* childDIType = (*treeI)->getDIType();
//     auto dataLayout = module->getDataLayout();
//     std::string parentStructName = DIUtils::getDIFieldName(parentDIType);
//     parentStructName = "struct." + parentStructName;
//     auto parentLLVMStructTy = module->getTypeByName(StringRef(parentStructName));
//     if (!parentLLVMStructTy)
//       continue;
//     if (childIdx >= parentLLVMStructTy->getNumElements())
//       continue;
//     // compute element offset in byte
//     auto parentStructLayout = dataLayout.getStructLayout(parentLLVMStructTy);
//     unsigned childOffsetInByte = parentStructLayout->getElementOffset(childIdx);
//     auto interProcAlias = findAliasInDomainWithOffset(*dataW->getInstruction(), F, childOffsetInByte, receiverDomainTrans);
//     for (auto alias : interProcAlias)
//     {
//       if (Instruction *i = dyn_cast<Instruction>(alias))
//       {
//         auto aliasW = instMap[i];
//         AccessType aliasAccType = getAccessTypeForInstW(aliasW);
//         if (static_cast<int>(aliasAccType) > static_cast<int>((*treeI)->getAccessType()))
//           (*treeI)->setAccessType(aliasAccType);
//       }
//     }
//   }
// }

std::map<std::string, AccessType> pdg::AccessInfoTracker::computeInterprocAccessedFieldMap(Function& callee, unsigned argNo)
{
  std::map<std::string, AccessType> accessFieldMap;
  auto &pdgUtils = PDGUtils::getInstance();
  auto funcMap = pdgUtils.getFuncMap();
  auto funcW = funcMap[&callee];
  ArgumentWrapper* argW = funcW->getArgWByIdx(argNo);
  if (argW == nullptr)
    return accessFieldMap;
  computeIntraprocArgAccessInfo(argW, callee); //TODO: has opportunity for optimization by avoid recomputing intra access info
  for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
  {
    if ((*treeI)->getAccessType() == AccessType::NOACCESS)
      continue;
    std::string parentFieldName = "";
    std::string fieldName = DIUtils::getDIFieldName((*treeI)->getDIType());
    if (tree<InstructionWrapper *>::depth(treeI) != 0)
    {
      auto ParentI = tree<InstructionWrapper *>::parent(treeI);
      std::string fieldId = DIUtils::computeFieldID((*ParentI)->getDIType(), (*treeI)->getDIType());
      if (!fieldId.empty())
        accessFieldMap.insert(std::make_pair(fieldId, (*treeI)->getAccessType()));
    }
  }
  return accessFieldMap;
}

std::set<Value *> pdg::AccessInfoTracker::findAliasInDomainWithOffset(Value &V, Function &F, unsigned offset, std::set<Function *> receiverDomainTrans)
{
  auto &pdgUtils = PDGUtils::getInstance();
  sea_dsa::DsaAnalysis *m_dsa = pdgUtils.getDsaAnalysis();
  sea_dsa::Graph *sourceFuncGraph = &m_dsa->getDsaAnalysis().getGraph(F);
  std::set<Value *> interprocAlias;
  if (!sourceFuncGraph)
    return interprocAlias;

  if (!sourceFuncGraph->hasCell(V))
    return interprocAlias;

  auto const &c1 = sourceFuncGraph->getCell(V);
  auto const &s1 = c1.getNode()->getAllocSites();

  for (Function *transFunc : receiverDomainTrans)
  {
    if (transFunc == &F)
      continue;
    sea_dsa::Graph *transFuncGraph = &m_dsa->getDsaAnalysis().getGraph(*transFunc);
    if (!transFuncGraph)
      assert(transFuncGraph && "cannot construct points to graph for transitive function.");

    for (auto instI = inst_begin(transFunc); instI != inst_end(transFunc); ++instI)
    {
      Instruction *curInst = &*instI;
      if (!transFuncGraph->hasCell(*curInst))
        continue;
      auto const &c2 = transFuncGraph->getCell(*curInst);
      auto const &s2 = c2.getNode()->getAllocSites();
      for (auto const a1 : s1)
      {
        if (s2.find(a1) != s2.end())
        {
          if (c2.getOffset() == offset)
            interprocAlias.insert(curInst);
        }
      }
    }
  }
  return interprocAlias;
}

std::set<Value *> pdg::AccessInfoTracker::findAliasInDomain(Value &V, Function &F, std::set<Function*> domainTransitiveClosure)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::set<Value *> aliasInDomain;
  sea_dsa::DsaAnalysis *m_dsa = pdgUtils.getDsaAnalysis();
  sea_dsa::Graph *sourceFuncGraph = &m_dsa->getDsaAnalysis().getGraph(F); // get source points to graph
  assert(sourceFuncGraph && "cannot construct points to graph for source cross-domain function.");

  for (Function *transFunc : domainTransitiveClosure)
  {
    if (transFunc == &F)
      continue;
    sea_dsa::Graph *transFuncGraph = &m_dsa->getDsaAnalysis().getGraph(*transFunc);
    assert(transFuncGraph && "cannot construct points to graph for transitive function.");
    if (F.getName() == "dummy_dev_init")
    {
      errs() << "trans func: " << transFunc->getName() << "\n";
      errs() << V << "\n";
    }
    for (auto I = inst_begin(transFunc); I != inst_end(transFunc); ++I)
    {
      Instruction* curInst = &*I;
      if (!sourceFuncGraph->hasCell(V) || !transFuncGraph->hasCell(*curInst))
        continue;
      // assert(sourceFuncGraph->hasCell(V) && transFuncGraph->hasCell(*I) && "cannot find cell in points to graph.");
      auto const &c1 = sourceFuncGraph->getCell(V);
      auto const &c2 = transFuncGraph->getCell(*I);
      auto const &s1 = c1.getNode()->getAllocSites();
      auto const &s2 = c2.getNode()->getAllocSites();
      // confirm alias and add it to domain alias set.
      // if (transFunc->getName() == "file_inode")
      // {
      //   for (auto const a1 : s1)
      //   {
      //     auto inst1 = dyn_cast<Instruction>(a1);
      //     errs() << "alloc1: " << *a1 << " - " << inst1->getFunction()->getName() << "\n";
      //   }
      //   for (auto const a2 : s2)
      //   {
      //     auto inst2 = dyn_cast<Instruction>(a2);
      //     errs() << "alloc2: " << *a2 << " - " << inst2->getFunction()->getName() << "\n";
      //   }
      // }
      for (auto const a1 : s1)
      {
        if (s2.find(a1) != s2.end())
        {
          if (F.getName() == "dummy_dev_init")
            errs() << "find alias in " << transFunc->getName() << "\n";
          aliasInDomain.insert(&*I);
          break;
        }
      }
    }
  }
  return aliasInDomain;
}

void pdg::AccessInfoTracker::computeFuncAccessInfo(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  // for arguments
  for (auto argW : funcW->getArgWList())
    computeArgAccessInfo(argW, TreeType::FORMAL_IN_TREE);
  // for return value
  computeArgAccessInfo(funcW->getRetW(), TreeType::FORMAL_IN_TREE);
}


pdg::ArgumentMatchType pdg::AccessInfoTracker::getArgMatchType(Argument *arg1, Argument *arg2)
{
  Type *arg1_type = arg1->getType();
  Type *arg2_type = arg2->getType();

  if (arg1_type == arg2_type)
    return pdg::ArgumentMatchType::EQUAL;

  if (arg1_type->isPointerTy())
    arg1_type = (dyn_cast<PointerType>(arg1_type))->getElementType();

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
        return pdg::ArgumentMatchType::CONTAINED;
    }
  }

  return pdg::ArgumentMatchType::NOTCONTAINED;
}

void pdg::AccessInfoTracker::mergeArgAccessInfo(ArgumentWrapper *callerArgW, ArgumentWrapper *calleeArgW, tree<InstructionWrapper *>::iterator callerTreeI)
{
  if (callerArgW == nullptr || calleeArgW == nullptr)
    return;
  auto callerFunc = callerArgW->getArg()->getParent();
  auto calleeFunc = calleeArgW->getArg()->getParent();
  if (callerFunc == nullptr || calleeFunc == nullptr)
    return;

  unsigned callerParamTreeSize = callerArgW->getTree(TreeType::FORMAL_IN_TREE).size(callerTreeI);
  unsigned calleeParamTreeSize = calleeArgW->getTree(TreeType::FORMAL_IN_TREE).size();

  if (callerParamTreeSize != calleeParamTreeSize)
    return;

  auto calleeTreeI = calleeArgW->tree_begin(TreeType::FORMAL_IN_TREE);
  for (; callerTreeI != callerArgW->tree_end(TreeType::FORMAL_IN_TREE) && calleeTreeI != calleeArgW->tree_end(TreeType::FORMAL_IN_TREE); ++callerTreeI, ++calleeTreeI)
  {
    if (callerTreeI == 0 || calleeTreeI == 0)
      return;
    if (static_cast<int>((*callerTreeI)->getAccessType()) < static_cast<int>((*calleeTreeI)->getAccessType())) {
      (*callerTreeI)->setAccessType((*calleeTreeI)->getAccessType());
    }
  }
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

    Type *parentTy = curTyNode->getParentLLVMType();
    Type *curType = curTyNode->getLLVMType();

    errs() << "Num of child: " << tree<InstructionWrapper *>::number_of_children(treeI) << "\n";

    if (parentTy == nullptr)
    {
      errs() << "** Root type node **" << "\n";
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

  // swap the function name with its registered function pointer. Just to align with the IDL syntax
  auto funcName = F.getName().str();
  funcName = getRegisteredFuncPtrName(funcName);
  // handle return type, concate with function name
  if (PDG->isStructPointer(F.getReturnType())) // generate alloc(caller) as return struct pointer is from the other side
  {
    auto retFuncName = funcName;
    if (retTypeName.back() == '*')
    {
      retTypeName.pop_back();
      retFuncName.push_back('*');
    }
    retTypeName = "projection " + retTypeName + "_" + retFuncName;
    std::string retAttributeStr = getReturnAttributeStr(F);
    if (!retAttributeStr.empty())
      retTypeName = retTypeName + retAttributeStr;
  }
  
  idl_file << "\trpc " << retTypeName << " " << funcName << "( ";
  // handle parameters
  for (auto argW : pdgUtils.getFuncMap()[&F]->getArgWList())
  {
    Argument &arg = *argW->getArg();
    Type *argType = arg.getType();
    DIType *argDIType = DIUtils::getArgDIType(arg);
    auto &dbgInstList = pdgUtils.getFuncMap()[&F]->getDbgInstList();
    std::string argName = DIUtils::getArgName(arg);
    // infer annotation
    if (argType->isPointerTy())
    {
      auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE);
      if (treeI != argW->tree_end(TreeType::FORMAL_IN_TREE))
      {
        treeI++;
        std::string annotation = inferFieldAnnotation(*treeI);
        if (!annotation.empty())
          argName = argName + " " + annotation;
      }
    }

    if (PDG->isStructPointer(argType))
    {
      auto argFuncName = funcName;
      auto argTypeName = DIUtils::getArgTypeName(arg);
      auto argName = DIUtils::getArgName(arg);
      if (argTypeName.back() == '*')
      {
        argTypeName.pop_back();
        argFuncName.push_back('*');
      }

      if (argTypeName.find("ops") == std::string::npos) // handling projection for function structs
        argTypeName = argTypeName + "_" + argFuncName;
      else
        argTypeName.push_back('*');
      
      // generate array annotation
      uint64_t arrSize = getArrayArgSize(arg, F);
      if (arrSize > 0)
        argName = argName + "[" + std::to_string(arrSize) + "]";
      idl_file << "projection " << argTypeName << " " << argName;
    }
    else if (PDG->isFuncPointer(argType))
    {
      Function *indirectCalledFunc = nullptr;
      Function *indirectFunc = module->getFunction(switchIndirectCalledPtrName(argName));
      idl_file << "rpc " << DIUtils::getFuncSigName(DIUtils::getLowestDIType(DIUtils::getArgDIType(arg)), indirectCalledFunc, argName, "");
    }
    else if (argType->isArrayTy())
    {
      std::string argTypeName = DIUtils::getArgTypeName(arg);
      std::string argStr = argTypeName + " " + argName;
      uint64_t arrSize = getArrayArgSize(arg, F);
      if (arrSize > 0)
        argStr = argStr + "[" + std::to_string(arrSize) + "]";
      idl_file << argStr;
    }
    else
      idl_file << DIUtils::getArgTypeName(arg) << " " << argName;

    if (argW->getArg()->getArgNo() < F.arg_size() - 1 && !argName.empty())
      idl_file << ", ";
  }
  idl_file << " );\n\n";
}

uint64_t pdg::AccessInfoTracker::getArrayArgSize(Value &V, Function& F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto m_dsa = pdgUtils.getDsaAnalysis();
  sea_dsa::Graph *G = &m_dsa->getDsaAnalysis().getGraph(F);
  if (!G || !G->hasCell(V))
    return 0;
  auto const &c = G->getCell(V);
  auto const &s = c.getNode()->getAllocSites();
  for (auto const a1 : s)
  {
    Value* tmp = const_cast<Value*>(a1);
    if (AllocaInst *ai = dyn_cast<AllocaInst>(tmp))
    {
      PointerType *allocType = ai->getType();
      Type *pointedTy = allocType->getElementType();
      if (pointedTy->isArrayTy())
        return pointedTy->getArrayNumElements();
    }
    // TODO: need to handle dynamic allocated array
  }
  return 0;
}

bool pdg::AccessInfoTracker::mayAlias(Value &V1, Value &V2, Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto m_dsa = pdgUtils.getDsaAnalysis();
  sea_dsa::Graph *G = &m_dsa->getDsaAnalysis().getGraph(F);
  if (!G)
    return false;
  if (!G->hasCell(V1) || !G->hasCell(V2))
    return false;
  auto const &c1 = G->getCell(V1);
  auto const &c2 = G->getCell(V2);
  auto const &s1 = c1.getNode()->getAllocSites();
  auto const &s2 = c2.getNode()->getAllocSites();
  for (auto const a1 : s1)
  {
    if (s2.find(a1) != s2.end())
      return true;
  }
  return false;
}

std::set<Instruction*> pdg::AccessInfoTracker::getIntraFuncAlias(Instruction *inst)
{
  Function *F = inst->getFunction();
  std::set<Instruction*> aliasSet;
  for (auto instI = inst_begin(F); instI != inst_end(F); instI++)
  {
    if (&*instI == inst)
      continue;
    if (mayAlias(*inst, *instI, *F)) 
      aliasSet.insert(&*instI);
  }
  return aliasSet;
}

std::string pdg::AccessInfoTracker::getReturnAttributeStr(Function &F)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto m_dsa = pdgUtils.getDsaAnalysis();
  sea_dsa::Graph *G = &m_dsa->getDsaAnalysis().getGraph(F);
  auto funcMap = pdgUtils.getFuncMap();
  auto funcW = funcMap[&F];
  auto retInstList = funcW->getReturnInstList();
  for (auto retInst : retInstList)
  {
    if (!isa<Instruction>(retInst->getReturnValue()))
      continue;
    Instruction* retVal = cast<Instruction>(retInst->getReturnValue());
    auto aliasSet = getIntraFuncAlias(retVal);
    for (auto alias : aliasSet)
    {
      if (!G->hasCell(*alias))
        continue;
      auto const &c = G->getCell(*alias);
      auto const &s = c.getNode()->getAllocSites();
      for (auto const a : s)
      {
        Value* tempV = const_cast<Value*>(a);
        if (CallInst *ci = dyn_cast<CallInst>(tempV))
        {
          if (Function *calledFunc = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
          {
            std::string funcName = calledFunc->getName().str();
            if (funcName.find("zalloc") != std::string::npos || funcName.find("malloc") != std::string::npos)
              return "[alloc(caller)]";
          }
        }
      }
    }
  }
  return "";
}

void pdg::AccessInfoTracker::generateIDLforFunc(Function &F)
{
  // if a function is defined on the same side, no need to generate IDL rpc for this function.
  errs() << "Start generating IDL for " << F.getName() << "\n";
  auto &pdgUtils = PDGUtils::getInstance();
  FunctionWrapper *funcW = pdgUtils.getFuncMap()[&F];
  for (auto argW : funcW->getArgWList())
  {
    generateIDLforArg(argW, TreeType::FORMAL_IN_TREE);
  }
  generateIDLforArg(funcW->getRetW(), TreeType::FORMAL_IN_TREE);
  // don't generate rpc for driver export function pointers
  if (driverExportFuncPtrNameMap.find(F.getName().str()) == driverExportFuncPtrNameMap.end())
    generateRpcForFunc(F);
}

void pdg::AccessInfoTracker::generateIDLforArg(ArgumentWrapper *argW, TreeType treeTy, std::string funcName, bool handleFuncPtr)
{
  auto &pdgUtils = PDGUtils::getInstance();
  if (argW->getTree(TreeType::FORMAL_IN_TREE).size() == 0)
    return;

  Function &F = *argW->getArg()->getParent();

  if (funcName.empty())
    funcName = F.getName().str();

  funcName = getRegisteredFuncPtrName(funcName);

  auto &dbgInstList = pdgUtils.getFuncMap()[&F]->getDbgInstList();
  std::string argName = DIUtils::getArgName(*(argW->getArg()));
  auto treeBegin = argW->tree_begin(TreeType::FORMAL_IN_TREE);
  DIType* argDIType = (*treeBegin)->getDIType();

  std::queue<tree<InstructionWrapper*>::iterator> treeNodeQ;
  treeNodeQ.push(argW->tree_begin(treeTy));
  std::queue<tree<InstructionWrapper*>::iterator> funcPtrQ;

  while (!treeNodeQ.empty())
  {
    auto treeI = treeNodeQ.front();
    treeNodeQ.pop();
    auto curDIType = (*treeI)->getDIType();

    if (curDIType == nullptr) continue;
    std::stringstream projection_str;
    bool accessed = false;
    // only process sturct pointer and function pointer, these are the only types that we should generate projection for
    if (!DIUtils::isStructPointerTy(curDIType) &&
        !DIUtils::isFuncPointerTy(curDIType) &&
        !DIUtils::isUnionPointerTy(curDIType) &&
        !DIUtils::isStructTy(curDIType))
      continue;

    DIType* baseType = DIUtils::getLowestDIType(curDIType);

    if (!DIUtils::isStructTy(baseType) && !DIUtils::isUnionType(baseType))
      continue;

    if (DIUtils::isStructPointerTy(curDIType) || DIUtils::isUnionPointerTy(curDIType))
      treeI++;

    for (int i = 0; i < tree<InstructionWrapper *>::number_of_children(treeI); ++i)
    {
      auto childT = tree<InstructionWrapper *>::child(treeI, i);
      auto childDIType = (*childT)->getDIType();
      std::string fieldID = DIUtils::computeFieldID(argDIType, childDIType);
      // determien if a field is accessed in asynchrnous context. If so, add it to projection.
      if (accessedFieldsInAsyncCalls.find(fieldID) != accessedFieldsInAsyncCalls.end())
      {
        // retrieve the access type for a field accessed in asynchornous context
        if (globalFieldAccessInfo.find(fieldID) != globalFieldAccessInfo.end())
        {
          auto accType = globalFieldAccessInfo[fieldID];
          (*childT)->setAccessType(accType);
        }
      }
      if ((*childT)->getAccessType() == AccessType::NOACCESS)
        continue;
      // check if an accessed field is in the set of shared data
      if (!isChildFieldShared(argDIType, childDIType))
        continue;
      // only check access status under cross boundary case. If not cross, we do not check and simply perform
      // normal field finding analysis.
      // alloc attribute
      std::string fieldAnnotation = inferFieldAnnotation(*childT);
      if (DIUtils::isFuncPointerTy(childDIType))
      {
        // generate rpc for the indirect function call
        std::string funcName = DIUtils::getDIFieldName(childDIType);
        // generate rpc for defined function in isolated driver. Also, if kernel hook a function to a function pointer, we need to caputre this write
        if (driverExportFuncPtrNames.find(funcName) == driverExportFuncPtrNames.end())
          continue;
        // for each function pointer, swap it with the function name registered to the pointer.
        funcName = switchIndirectCalledPtrName(funcName);
        Function *indirectFunc = module->getFunction(funcName);
        if (indirectFunc == nullptr)
          continue;
        projection_str << "\t\trpc " << DIUtils::getFuncSigName(DIUtils::getLowestDIType(childDIType), indirectFunc, DIUtils::getDIFieldName(childDIType)) << ";\n";
        // put all function pointer in a queue, and process them all together later
        funcPtrQ.push(childT);
      }
      else if (DIUtils::isStructPointerTy(childDIType))
      {
        // for struct pointer, generate a projection
        auto tmpName = DIUtils::getDITypeName(childDIType);
        auto tmpFuncName = funcName;
        // formatting functions
        while (tmpName.back() == '*')
        {
          tmpName.pop_back();
          tmpFuncName.push_back('*');
        }

        std::string fieldTypeName = tmpName;
        if (fieldTypeName.find("ops") == std::string::npos) // specific handling for struct ops
          fieldTypeName = tmpName + "_" + tmpFuncName;
        else 
          fieldTypeName = tmpName + "*";
        
        projection_str << "\t\t"
                       << "projection " << fieldTypeName << fieldAnnotation << " "
                       << " " << DIUtils::getDIFieldName(childDIType) << ";\n";
        treeNodeQ.push(childT);
      }
      else if (DIUtils::isStructTy(childDIType))
      {
        auto fieldTypeName = DIUtils::getDITypeName(childDIType);
        projection_str << "\t\t"
                       << "projection " << fieldTypeName << fieldAnnotation << " "
                       << " " << DIUtils::getDIFieldName(childDIType)<< "_" << funcName << ";\n";
        treeNodeQ.push(childT);
        continue;
      }
      else if (DIUtils::isUnionType(childDIType))
      {
        auto fieldTypeName = DIUtils::getDITypeName(childDIType) + funcName;
        // continue;
        projection_str << "\t\t"
                       << "union " << fieldTypeName << " "
                       << " " << DIUtils::getDIFieldName(childDIType) << ";\n";
        continue;
      }
      else
      {
        std::string fieldName = DIUtils::getDIFieldName(childDIType);
        if (!fieldName.empty())
          projection_str << "\t\t" + DIUtils::getDITypeName(childDIType) << " " << getAccessAttributeName(childT) << " " << DIUtils::getDIFieldName(childDIType) << ";\n";
      }
    }

    // if any child field is accessed, we need to print out the projection for the current struct.
    if (DIUtils::isStructTy(baseType))
    {
      std::stringstream temp;
      std::string structName = DIUtils::getDIFieldName(curDIType); // the name is stored in tag member.
      // a very specific handling for generating IDL for function struct ops
      if (structName.find("ops") == std::string::npos)
      {
        temp << "\tprojection "
             << "< struct " << structName << " > " << structName << "_" << funcName << " "
             << " {\n"
             << projection_str.str() << " \t}\n\n";
      }
      else
      {
        if (seenFuncOps.find(structName) == seenFuncOps.end()) // only generate projection for struct ops at the first time we see it.
        {
          temp << "\tprojection "
               << "< struct " << structName << " > " << structName << " "
               << " {\n"
               << projection_str.str() << " \t}\n\n";
          seenFuncOps.insert(structName);
        }
      }
      projection_str = std::move(temp);
    }
    else if (DIUtils::isUnionType(baseType))
    {
      std::stringstream temp;
      std::string unionName = DIUtils::getDIFieldName(baseType);
      // a very specific handling for generating IDL for function ops
      temp << "\tprojection "
           << "< union " << unionName << " > " << unionName << "_" << funcName << " "
           << " {\n"
           << projection_str.str() << " \t}\n\n";
      projection_str = std::move(temp);
    }
    else
      projection_str.str(""); 
    // process function pointers. Need to find all candidate called functions, and get all the access information from them.
    // while (!funcPtrQ.empty())
    // {
    //   auto funcT = funcPtrQ.front();
    //   funcPtrQ.pop();
    //   // for each function pointer, we generate a 
    //   std::string funcPtrName = DIUtils::getDIFieldName((*funcT)->getDIType());
    //   if (processedFuncPtrNames.find(funcPtrName) == processedFuncPtrNames.end())
    //     generateIDLforFuncPtrWithDI((*funcT)->getDIType(), F.getParent(), funcPtrName);
    // }
    idl_file << projection_str.str();
    accessed = false;
  }
}

std::string pdg::AccessInfoTracker::inferFieldAnnotation(InstructionWrapper *instW)
{
  auto valDepPairList = PDG->getNodesWithDepType(instW, DependencyType::VAL_DEP);
  for (auto valDepPair : valDepPairList)
  {
    auto dataW = valDepPair.first->getData();
    if (!dataW->getInstruction())
      continue;
    auto dataDList = PDG->getNodeDepList(dataW->getInstruction());
    for (auto depPair : dataDList)
    {
      InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
      DependencyType depType = depPair.second;
      Instruction *depInst = depInstW->getInstruction();
      if (!depInst)
        continue;
      if (depType == DependencyType::DATA_DEF_USE)
      {
        if (CallInst *ci = dyn_cast<CallInst>(depInst))
        {
          if (Function *calledFunc = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
          {
            if (calledFunc != nullptr)
            {
              std::string calledFuncName = calledFunc->getName().str();
              if (stringOperations.find(calledFuncName) != stringOperations.end())
              {
                std::string retStr = "[string]";
                return retStr;
              }
            }
          }
        }
        // alloc annotation
        if (StoreInst *ci = dyn_cast<StoreInst>(depInst)) {
            if (ci->getPointerOperand() == dataW->getInstruction()) {
              if (isa<GlobalVariable>(ci->getValueOperand()->stripPointerCasts()))
                return "[alloc(caller)] [out]";
            }
        }
      }
    }
  }

  return "";
}

std::string pdg::getAccessAttributeName(tree<InstructionWrapper *>::iterator treeI)
{
  std::vector<std::string> access_attribute = {
      "No Access",
      "",
      "[out]"};
  int accessIdx = static_cast<int>((*treeI)->getAccessType());
  return access_attribute[accessIdx];
}

std::string pdg::AccessInfoTracker::getArgAccessInfo(Argument &arg)
{
  auto &pdgUtils = PDGUtils::getInstance();
  std::vector<std::string> mod_info = {"U", "R", "W", "T"};
  ArgumentWrapper *argW = pdgUtils.getFuncMap()[arg.getParent()]->getArgWByArg(arg);
  return mod_info[static_cast<int>((*argW->getTree(TreeType::FORMAL_IN_TREE).begin())->getAccessType())];
}

// compute shared data
void pdg::AccessInfoTracker::computeSharedDataInFunc(Function &F)
{
  // for each argument, computes it debugging information type
  for (auto &arg : F.args())
  {
    // do not process non-struct ptr type, struct type is coersed
    DIType* argDIType = DIUtils::getArgDIType(arg);
    if (!DIUtils::isStructPointerTy(argDIType))
      continue;
    // check if shared fields for this struct type is already done
    std::string argTypeName = DIUtils::getArgTypeName(arg);
    if (sharedDataTypeMap.find(argTypeName) == sharedDataTypeMap.end())
      sharedDataTypeMap.insert(std::pair<std::string, std::set<std::string>>(argTypeName, std::set<std::string>()));
    std::set<std::string> accessedFieldsInArg = computeSharedDataForType(argDIType);
    sharedDataTypeMap[argTypeName].insert(accessedFieldsInArg.begin(), accessedFieldsInArg.end());
  }
}

std::set<std::string> pdg::AccessInfoTracker::computeSharedDataForType(DIType *dt)
{
  // compute accessed fields in driver/kernel domain separately. 
  std::set<std::string> accessedFieldsInDriver = computeAccessedDataInDomain(dt, driverDomainFuncs);
  std::set<std::string> accessedFieldsInKernel = computeAccessedDataInDomain(dt, kernelDomainFuncs);
  // then, take the union of the accessed fields in the two domains.
  std::set<std::string> sharedFields;
  for (auto str : accessedFieldsInDriver)
  {
    if (accessedFieldsInKernel.find(str) != accessedFieldsInKernel.end())
      sharedFields.insert(str);
  }
  return sharedFields;
}

std::set<std::string> pdg::AccessInfoTracker::computeAccessedDataInDomain(DIType* dt, std::set<Function*> domain)
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto funcMap = pdgUtils.getFuncMap();
  std::set<std::string> accessedFieldsInDomain;
  for (auto func : domain)
  {
    auto funcW = funcMap[func];
    for (auto &arg : func->args())
    {
      DIType* argDIType = DIUtils::getArgDIType(arg);
      // check if debugging type matches
      auto n1 = DIUtils::getDITypeName(argDIType);
      auto n2 = DIUtils::getDITypeName(dt);
      if (n1 != n2)
        continue;
      // if type names match, compute the accessed fields id and store it.
      ArgumentWrapper *argW = funcW->getArgWByArg(arg);
      auto argFormalTree = argW->getTree(TreeType::FORMAL_IN_TREE);
      std::set<std::string> accessedFields = computeAccessedFieldsForDIType(argFormalTree, argDIType);
      accessedFieldsInDomain.insert(accessedFields.begin(), accessedFields.end());
      // special handling: compute accessed fields in asynchronous call. Need to put all such fields into the transitive closure of cross-domain call function
      if (asyncCalls.find(func) != asyncCalls.end())
        accessedFieldsInAsyncCalls.insert(accessedFields.begin(), accessedFields.end());
    }
  }
  return accessedFieldsInDomain;
}

std::set<std::string> pdg::AccessInfoTracker::computeAccessedFieldsForDIType(tree<InstructionWrapper *> objectTree, DIType *rootDIType)
{
  std::set<std::string> accessedFields;
  for (auto treeI = objectTree.begin(); treeI != objectTree.end(); ++treeI)
  {
    // hanlde function pointer exported by driver domain
    DIType* fieldDIType = (*treeI)->getDIType();
    std::string fieldName = DIUtils::getDIFieldName(fieldDIType);
    if (fieldName.find("ops") != std::string::npos)
    {
        std::string fieldID = DIUtils::computeFieldID(rootDIType, fieldDIType);
        accessedFields.insert(fieldID);
        globalFieldAccessInfo.insert(std::make_pair(fieldID, AccessType::READ));
        continue;
    }

    if (DIUtils::isFuncPointerTy(fieldDIType))
    {
      if (driverExportFuncPtrNames.find(fieldName) != driverExportFuncPtrNames.end())
      {
        std::string fieldID = DIUtils::computeFieldID(rootDIType, fieldDIType);
        accessedFields.insert(fieldID);
        globalFieldAccessInfo.insert(std::make_pair(fieldID, AccessType::READ));
      }
     continue; 
    }

    // get valdep pair, and check for intraprocedural accesses
    auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
    for (auto valDepPair : valDepPairList)
    {
      auto dataW = valDepPair.first->getData();
      AccessType accType = getAccessTypeForInstW(dataW);
      if (accType != AccessType::NOACCESS)
      {
        std::string fieldID = DIUtils::computeFieldID(rootDIType, fieldDIType);
        accessedFields.insert(fieldID);
        globalFieldAccessInfo.insert(std::make_pair(fieldID, accType));
      }
    }
  }
  return accessedFields;
}

// std::set<std::string> pdg::AccessInfoTracker::computeAccessFieldsForArg(ArgumentWrapper *argW, DIType* rootDIType)
// {
//   std::set<std::string> accessedFields;
//   // iterate through each node, find their addr variables and then analyze the accesses to the addr variables
//   for (auto treeI = argW->tree_begin(TreeType::FORMAL_IN_TREE); treeI != argW->tree_end(TreeType::FORMAL_IN_TREE); ++treeI)
//   {
//     // hanlde function pointer exported by driver domain
//     DIType* fieldDIType = (*treeI)->getDIType();
//     std::string fieldName = DIUtils::getDIFieldName(fieldDIType);
//     if (fieldName.find("ops") != std::string::npos)
//     {
//         std::string fieldID = computeFieldID(rootDIType, fieldDIType);
//         accessedFields.insert(fieldID);
//         continue;
//     }

//     if (DIUtils::isFuncPointerTy(fieldDIType))
//     {
//       if (driverExportFuncPtrNames.find(fieldName) != driverExportFuncPtrNames.end())
//       {
//         std::string fieldID = computeFieldID(rootDIType, fieldDIType);
//         accessedFields.insert(fieldID);
//       }
//      continue; 
//     }

//     // get valdep pair, and check for intraprocedural accesses
//     auto valDepPairList = PDG->getNodesWithDepType(*treeI, DependencyType::VAL_DEP);
//     for (auto valDepPair : valDepPairList)
//     {
//       auto dataW = valDepPair.first->getData();
//       AccessType accType = getAccessTypeForInstW(dataW);
//       if (accType != AccessType::NOACCESS)
//       {
//         std::string fieldID = computeFieldID(rootDIType, fieldDIType);
//         accessedFields.insert(fieldID);
//       }
//     }
//   }
//   return accessedFields;
// }

bool pdg::AccessInfoTracker::isChildFieldShared(DIType* argDIType, DIType* fieldDIType)
{
  // if field is a function pointer, and its name is in function pointer list exported by driver, then we retrun it.
  if (DIUtils::isFuncPointerTy(fieldDIType))
  {
    std::string fieldName = DIUtils::getDIFieldName(fieldDIType);
    if (driverExportFuncPtrNames.find(fieldName) != driverExportFuncPtrNames.end())
      return true;
  }

  // compute field id and check if it is presents in the set of shared data computed for the arg Type
  std::string argTypeName = DIUtils::getDITypeName(argDIType);
  if (sharedDataTypeMap.find(argTypeName) == sharedDataTypeMap.end())
  {
    errs() << "[WARNING] " << "cannot find struct type " << argTypeName << "\n";
    return false; 
  }
  std::string fieldID = DIUtils::computeFieldID(argDIType, fieldDIType);
  auto sharedFields = sharedDataTypeMap[argTypeName];
  if (sharedFields.find(fieldID) == sharedFields.end())
    return false;
  return true;
}

std::string pdg::AccessInfoTracker::switchIndirectCalledPtrName(std::string funcptrName)
{
  for (auto it = driverExportFuncPtrNameMap.begin(); it != driverExportFuncPtrNameMap.end(); ++it)
  {
    if (it->second == funcptrName)
    {
      return it->first;
    }
  }
  return funcptrName;
}

static RegisterPass<pdg::AccessInfoTracker>
    AccessInfoTracker("idl-gen", "Argument access information tracking Pass", false, true);