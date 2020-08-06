#include "DataDependencyGraph.hpp"

using namespace llvm;

char pdg::DataDependencyGraph::ID = 0;

void pdg::DataDependencyGraph::initializeMemoryDependencyPasses()
{
  AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
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
  // auto &pdgUtils = PDGUtils::getInstance();
  for (inst_iterator instIt = inst_begin(Func); instIt != inst_end(Func); ++instIt)
  {
    getNodeByData(&*instIt); // add node to graph.
    Instruction *inst = dyn_cast<Instruction>(&*instIt);
    // if (auto st = dyn_cast<StoreInst>(inst))
    //   DDG->addDependency(pdgUtils.getInstMap()[cast<Instruction>(st->getValueOperand())], pdgUtils.getInstMap()[cast<Instruction>(st->getPointerOperand())], DependencyType::DATA_GENERAL);
    collectDefUseDependency(inst);
    collectCallInstDependency(inst);

    if (isa<LoadInst>(inst))
    {
      collectReadFromDependency(inst);
      collectRAWDependency(inst);
      // collectNonLocalDependency(inst);
    }


    if (isa<GetElementPtrInst>(inst))
      collectReadFromDependency(inst);
  }
}

bool pdg::DataDependencyGraph::isMustAlias(Value &V1, Value &V2, Function &F)
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
    {
      unsigned c1Offset = c1.getOffset();
      unsigned c2Offset = c2.getOffset();
      if (c1Offset == c2Offset)
        return true;
    }
  }
  return false;
}

void pdg::DataDependencyGraph::collectAliasDependencies()
{
  auto &pdgUtils = PDGUtils::getInstance();
  assert((pdgUtils.getDsaAnalysis() != nullptr) && "sea_dsa points to graph not constructed.");
  auto funcW = pdgUtils.getFuncMap()[Func];
  auto instMap = pdgUtils.getInstMap();
  for (auto I1 = inst_begin(Func); I1 != inst_end(Func); ++I1)
  {
    for (auto I2 = I1; I2 != inst_end(Func); ++I2)
    {
      if (I1 == I2)
        continue;
      if (isa<GetElementPtrInst>(&*I2))
        continue;
      if (isMustAlias(*I1, *I2, *Func))
        DDG->addDependency(instMap[&*I1], instMap[&*I2], DependencyType::DATA_ALIAS);
      if (CastInst *ci = dyn_cast<CastInst>(&*I2))
      {
        if (ci->getOperand(0) == &*I1)
          DDG->addDependency(instMap[&*I1], instMap[&*I2], DependencyType::DATA_ALIAS);
      }
    }
  }
}

void pdg::DataDependencyGraph::collectReadFromDependency(llvm::Instruction *inst)
{
  if (auto li = dyn_cast<LoadInst>(inst))
  {
    if (Instruction *pInst = dyn_cast<Instruction>(li->getPointerOperand()))
    {
      DDG->addDependency(PDGUtils::getInstance().getInstMap()[pInst],
                         PDGUtils::getInstance().getInstMap()[inst],
                         DependencyType::DATA_READ);
    }
  }

  if (auto gep = dyn_cast<GetElementPtrInst>(inst))
  {
    if (Instruction *pInst = dyn_cast<Instruction>(gep->getPointerOperand()))
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
  for (auto user : inst->users())
  {
    if (Instruction *pInst = dyn_cast<Instruction>(user))
    {
      // add info flow from the instruction to current instruction
      DDG->addDependency(PDGUtils::getInstance().getInstMap()[inst],
                         PDGUtils::getInstance().getInstMap()[pInst],
                         DependencyType::DATA_DEF_USE);
    }
  }
  // for (Instruction::const_op_iterator cuit = inst->op_begin();
  //      cuit != inst->op_end(); ++cuit)
  // {
  //   if (Instruction *pInst = dyn_cast<Instruction>(*cuit))
  //   {
  //     // add info flow from the instruction to current instruction
  //     DDG->addDependency(PDGUtils::getInstance().getInstMap()[pInst],
  //                        PDGUtils::getInstance().getInstMap()[inst],
  //                        DependencyType::DATA_DEF_USE);
  //   }
  // }
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
        DDG->addDependency(PDGUtils::getInstance().getInstMap()[tmpInst],
                           PDGUtils::getInstance().getInstMap()[inst],
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
  // MemoryLocation LI_Loc = MemoryLocation::get(LI);
  for (StoreInst *SI : StoreVec)
  {
    if (SI->getPointerOperand() == LI->getPointerOperand())
      _flowdep_set.push_back(SI);
    // MemoryLocation SI_Loc = MemoryLocation::get(SI);
    // AliasResult andersAAResult = andersAA->query(LI_Loc, SI_Loc);
    // AliasResult steensAAResult = steenAA->query(LI_Loc, SI_Loc);
    // if (andersAAResult != NoAlias || steensAAResult != NoAlias)
    // {
    //   _flowdep_set.push_back(SI);
    // }
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

// void pdg::DataDependencyGraph::collectNonLocalDependency(llvm::Instruction *inst)
// {
//   // dealing with non local pointer dependency, nonLocalPointer dep is stored in result small vector
//   SmallVector<NonLocalDepResult, 20> result;
//   // the return result is NonLocalDepResult. can use getAddress function
//   // now result stores all possible
//   for (NonLocalDepResult &I : result)
//   {
//     const MemDepResult &nonLocal_res = I.getResult();
//     InstructionWrapper *itInst = PDGUtils::getInstance().getInstMap()[inst];
//     InstructionWrapper *parentInst = PDGUtils::getInstance().getInstMap()[nonLocal_res.getInst()];

//     if (nonLocal_res.getInst() != nullptr)
//     {
//       DDG->addDependency(itInst, parentInst, DependencyType::DATA_GENERAL);
//     }
//     // ignore nullptr return res
//   }
// }

pdg::DependencyNode<pdg::InstructionWrapper> *pdg::DataDependencyGraph::getNodeByData(Instruction *inst)
{
  return DDG->getNodeByData(PDGUtils::getInstance().getInstMap()[inst]);
}

typename pdg::DependencyNode<pdg::InstructionWrapper>::DependencyLinkList pdg::DataDependencyGraph::getNodeDepList(Instruction *inst)
{
  return DDG->getNodeDepList(PDGUtils::getInstance().getInstMap()[inst]);
}

typename pdg::DependencyNode<pdg::InstructionWrapper>::DependencyLinkList pdg::DataDependencyGraph::getNodeDepList(InstructionWrapper *instW)
{
  return DDG->getNodeDepList(instW);
}

bool pdg::DataDependencyGraph::runOnFunction(Function &F)
{
  Func = &F;
  PDGUtils::getInstance().constructFuncMap(*F.getParent());
  initializeMemoryDependencyPasses();
  constructFuncMapAndCreateFunctionEntry();
  collectDataDependencyInFunc();
  collectAliasDependencies();
  return false;
}

void pdg::DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<AAResultsWrapperPass>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::DataDependencyGraph>
    DDG("ddg", "Data Dependency Graph Construction", false, true);