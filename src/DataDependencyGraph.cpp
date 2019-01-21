#include "DataDependencyGraph.hpp"

using namespace llvm;

char pdg::DataDependencyGraph::ID = 0;

void pdg::DataDependencyGraph::initializeMemoryDependencyPasses()
{
  steenAA = &getAnalysis<CFLSteensAAWrapperPass>().getResult();
  andersAA = &getAnalysis<CFLAndersAAWrapperPass>().getResult();
  MD = &getAnalysis<MemoryDependenceWrapperPass>().getMemDep();
}

void pdg::DataDependencyGraph::constructFuncMapAndCreateFunctionEntry()
{
  auto& pdgUtils = PDGUtils::getInstance();
  if (pdgUtils.getFuncMap()[Func]->getEntryW() == nullptr)
  {
    InstructionWrapper *entryW = new InstructionWrapper(Func, GraphNodeType::ENTRY);
    pdgUtils.getFuncInstWMap()[Func].insert(entryW);
    pdgUtils.getFuncMap()[Func]->setEntryW(entryW);
  }
}

void pdg::DataDependencyGraph::collectDataDependencyInFunc()
{
  for (inst_iterator instIt = inst_begin(Func); instIt != inst_end(Func); ++instIt)
  {
    getNodeByData(&*instIt); // add node to graph.
    Instruction *inst = dyn_cast<Instruction>(&*instIt);
    collectDefUseDependency(inst);
    collectCallInstDependency(inst);

    if (isa<LoadInst>(inst))
    {
      collectReadFromDependency(inst);
      collectRAWDependency(inst);
      collectNonLocalDependency(inst);
    }
  }
}

void pdg::DataDependencyGraph::collectAliasInst()
{
  std::vector<StoreInst *> storeVec = PDGUtils::getInstance().getFuncMap()[Func]->getStoreInstList();
  std::vector<LoadInst *> loadVec = PDGUtils::getInstance().getFuncMap()[Func]->getLoadInstList();

  for (StoreInst *si : storeVec)
  {
    MemoryLocation s_loc = MemoryLocation::get(si);
    for (LoadInst *li : loadVec)
    {
      MemoryLocation l_loc = MemoryLocation::get(li);
      AliasResult andersAAResult = andersAA->alias(s_loc, l_loc);
      // AliasResult steensAAResult = steenAA->alias(s_loc, l_loc);
      if (andersAAResult != NoAlias)
      {
        InstructionWrapper *loadInstW = PDGUtils::getInstance().getInstMap()[li];
        InstructionWrapper *storeInstW = PDGUtils::getInstance().getInstMap()[si];
        DDG->addDependency(storeInstW, loadInstW, DependencyType::DATA_ALIAS);
        DDG->addDependency(loadInstW, storeInstW, DependencyType::DATA_ALIAS);
      }
    }
    
    for (StoreInst *si1 : storeVec) {
      if (si == si1)
        continue;
      MemoryLocation s1_loc = MemoryLocation::get(si1);
      AliasResult andersAAResult = andersAA->alias(s_loc, s1_loc);
      if (andersAAResult != NoAlias) {
        InstructionWrapper *store1InstW = PDGUtils::getInstance().getInstMap()[si];
        InstructionWrapper *store2InstW = PDGUtils::getInstance().getInstMap()[si1];
        DDG->addDependency(store1InstW, store2InstW, DependencyType::DATA_ALIAS);
        DDG->addDependency(store2InstW, store1InstW, DependencyType::DATA_ALIAS);
      }
    }
  }

  // add alias info for two load instructions
  for (LoadInst *li1 : loadVec)
  {
    for (LoadInst *li2 : loadVec)
    {
      if (li1 == li2)
      {
        continue;
      }
      MemoryLocation li1_loc = MemoryLocation::get(li1);
      MemoryLocation li2_loc = MemoryLocation::get(li2);

      Type *li1LocTy = li1->getPointerOperandType();
      Type *li2LocTy = li2->getPointerOperandType();

      if (li1 != li2)
      {
        continue;
      }
      AliasResult AA_result = andersAA->alias(li1_loc, li2_loc);
      if (AA_result != NoAlias)
      {
        InstructionWrapper *loadInstW1 = PDGUtils::getInstance().getInstMap()[li1];
        InstructionWrapper *loadInstW2 = PDGUtils::getInstance().getInstMap()[li2];
        DDG->addDependency(loadInstW1, loadInstW2, DependencyType::DATA_ALIAS);
      }
    }
  }
}

void pdg::DataDependencyGraph::collectReadFromDependency(llvm::Instruction *inst)
{
  if (LoadInst *li = dyn_cast<LoadInst>(inst))
  {
    if (Instruction *pInst = dyn_cast<Instruction>(li->getPointerOperand()))
    {
      DDG->addDependency(PDGUtils::getInstance().getInstMap()[pInst],
                         PDGUtils::getInstance().getInstMap()[inst],
                         DependencyType::DATA_READ);
    }
  }
}

void pdg::DataDependencyGraph::collectDefUseDependency(llvm::Instruction *inst)
{
  // check for def-use dependencies
  for (Instruction::const_op_iterator cuit = inst->op_begin();
       cuit != inst->op_end(); ++cuit)
  {
    if (Instruction *pInst = dyn_cast<Instruction>(*cuit))
    {
      // add info flow from the instruction to current instruction
      DDG->addDependency(PDGUtils::getInstance().getInstMap()[pInst],
                         PDGUtils::getInstance().getInstMap()[inst],
                         DependencyType::DATA_DEF_USE);
    }
  }
}

void pdg::DataDependencyGraph::collectCallInstDependency(llvm::Instruction *inst)
{
  if (CallInst *callInst = dyn_cast<CallInst>(inst))
  {
    if (callInst->getCalledFunction() != nullptr && callInst->getCalledFunction()->isDeclaration())
      return;

    for (auto arg_iter = callInst->arg_begin(); arg_iter != callInst->arg_end(); ++arg_iter)
    {
      if (Instruction *tmpInst = dyn_cast<Instruction>(&*arg_iter))
      {
        // DDG->addDependency(PDGUtils::getInstance().getInstMap()[tmpInst],
        //                    PDGUtils::getInstance().getInstMap()[inst],
        //                    DependencyType::DATA_CALL_PARA);
        DDG->addDependency(PDGUtils::getInstance().getInstMap()[inst],
                           PDGUtils::getInstance().getInstMap()[tmpInst],
                           DependencyType::DATA_CALL_PARA);
      }
    }
  }
}

std::vector<Instruction *> pdg::DataDependencyGraph::getRAWDepList(Instruction *pLoadInst)
{
  std::vector<Instruction *> _flowdep_set;
  std::vector<StoreInst *> StoreVec = PDGUtils::getInstance().getFuncMap()[Func]->getStoreInstList();
  // for each Load Instruction, find related Store Instructions(alias considered)
  LoadInst *LI = dyn_cast<LoadInst>(pLoadInst);
  MemoryLocation LI_Loc = MemoryLocation::get(LI);
  for (StoreInst *SI : StoreVec)
  {
    MemoryLocation SI_Loc = MemoryLocation::get(SI);
    AliasResult andersAAResult = andersAA->alias(LI_Loc, SI_Loc);
    AliasResult steensAAResult = steenAA->alias(LI_Loc, SI_Loc);
    if (andersAAResult != NoAlias || steensAAResult != NoAlias)
    {
      _flowdep_set.push_back(SI);
    }
  }
  return _flowdep_set;
}

void pdg::DataDependencyGraph::collectRAWDependency(llvm::Instruction *inst)
{
  // dealing with dependencies in a function
  std::vector<Instruction *> flowdep_set = getRAWDepList(inst);

  for (unsigned i = 0; i < flowdep_set.size(); i++)
  {
    DDG->addDependency(PDGUtils::getInstance().getInstMap()[flowdep_set[i]],
                       PDGUtils::getInstance().getInstMap()[inst],
                       DependencyType::DATA_RAW);
  }

  flowdep_set.clear();
}

void pdg::DataDependencyGraph::collectNonLocalDependency(llvm::Instruction *inst)
{
  // dealing with non local pointer dependency, nonLocalPointer dep is stored in result small vector
  SmallVector<NonLocalDepResult, 20> result;
  // the return result is NonLocalDepResult. can use getAddress function
  MD->getNonLocalPointerDependency(inst, result);
  // now result stores all possible
  for (NonLocalDepResult &I : result)
  {
    const MemDepResult &nonLocal_res = I.getResult();
    InstructionWrapper *itInst = PDGUtils::getInstance().getInstMap()[inst];
    InstructionWrapper *parentInst = PDGUtils::getInstance().getInstMap()[nonLocal_res.getInst()];

    if (nonLocal_res.getInst() != nullptr)
    {
      DDG->addDependency(itInst, parentInst, DependencyType::DATA_GENERAL);
    }
    // ignore nullptr return res
  }
}

pdg::DependencyNode<pdg::InstructionWrapper> *pdg::DataDependencyGraph::getNodeByData(Instruction *inst)
{
  return DDG->getNodeByData(PDGUtils::getInstance().getInstMap()[inst]);
}

typename pdg::DependencyNode<pdg::InstructionWrapper>::DependencyLinkList pdg::DataDependencyGraph::getNodeDepList(Instruction *inst)
{
  return DDG->getNodeDepList(PDGUtils::getInstance().getInstMap()[inst]);
}

bool pdg::DataDependencyGraph::runOnFunction(Function &F)
{
  Func = &F;
  PDGUtils::getInstance().constructFuncMap(*F.getParent());
  initializeMemoryDependencyPasses();
  constructFuncMapAndCreateFunctionEntry();
  collectDataDependencyInFunc();
  collectAliasInst();
  return false;
}

void pdg::DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<MemoryDependenceWrapperPass>();
  AU.addRequired<CFLSteensAAWrapperPass>();
  AU.addRequired<CFLAndersAAWrapperPass>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::DataDependencyGraph>
    DDG("ddg", "Data Dependency Graph Construction", false, true);