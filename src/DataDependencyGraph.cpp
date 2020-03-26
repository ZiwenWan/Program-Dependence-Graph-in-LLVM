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

void pdg::DataDependencyGraph::collectAliasDependencies()
{
  auto &pdgUtils = PDGUtils::getInstance();
  auto funcW = pdgUtils.getFuncMap()[Func];
  auto instMap = pdgUtils.getInstMap();
  // auto storeVec = funcW->getStoreInstList();
  // auto loadVec = funcW->getLoadInstList();
  // auto intrinsicInstVec = funcW->getIntrinsicInstList();
  // auto castVec = funcW->getCastInstList();
  
  funcW->setAST(new AliasSetTracker(*AA));
  for (auto &BB : *Func)
  {
    for (auto &inst : BB)
    {
      funcW->getAST()->add(BB); // adding basicblock to aslias set tracker
    }
  }

  for (auto &AS : *funcW->getAST()) // adding alias relationship
  {
    for (auto iter1 = AS.begin(); iter1 != AS.end(); ++iter1)
    {
      Instruction *inst1 = dyn_cast<Instruction>(iter1->getValue());
      if (inst1 == nullptr)
        continue;
      for (auto iter2 = AS.begin(); iter2 != AS.end(); ++iter2)
      {
        if (iter1 == iter2)
          continue;
        Instruction *inst2 = dyn_cast<Instruction>(iter2->getValue());
        DDG->addDependency(instMap[inst1], instMap[inst2], DependencyType::DATA_ALIAS);
      }

      if (!isa<AllocaInst>(inst1) && !isa<GetElementPtrInst>(inst1))
        continue;
      for (auto user : inst1->users())
      {
        if (LoadInst *userInst = dyn_cast<LoadInst>(user))
          DDG->addDependency(instMap[inst1], instMap[userInst], DependencyType::DATA_ALIAS);
      }
    }
  }

  // if (Func->getName() == "dummy_dev_init")
  // {
  //   for (auto &BB : *Func)
  //   {
  //     for (auto &inst : BB)
  //     {
  //       if (LoadInst *li = dyn_cast<LoadInst>(&inst))
  //       {
  //         errs() << *li << "\n";
  //         auto &AS = funcW->getAST()->getAliasSetFor(MemoryLocation::get(li));
  //         errs() << "size: " << AS.size() << "\n";
  //         for (auto iter = AS.begin(); iter != AS.end(); ++iter)
  //         {
  //           errs() << "\t" << *iter->getValue() << "\n";
  //         }
  //       }
  //     }
  //   }
  // }

  // for (auto cti : castVec)
  // {
  //   if (auto inst = dyn_cast<Instruction>(cti->getOperand(0)))
  //   {
  //     DDG->addDependency(pdgUtils.getInstMap()[cti], pdgUtils.getInstMap()[inst], DependencyType::DATA_ALIAS);
  //     DDG->addDependency(pdgUtils.getInstMap()[inst], pdgUtils.getInstMap()[cti], DependencyType::DATA_ALIAS);
  //   }
  // }

  // for (auto ii : intrinsicInstVec)
  // {
  //   if (auto memIntrinsic = dyn_cast<MemTransferInst>(ii))
  //   {
  //     if (auto srcI = dyn_cast<Instruction>(memIntrinsic->getDest()))
  //     {
  //       if (auto destI = dyn_cast<Instruction>(memIntrinsic->getSource()))
  //       {
  //         DDG->addDependency(pdgUtils.getInstMap()[srcI], pdgUtils.getInstMap()[destI], DependencyType::DATA_ALIAS);
  //         DDG->addDependency(pdgUtils.getInstMap()[destI], pdgUtils.getInstMap()[srcI], DependencyType::DATA_ALIAS);
  //       }
  //     }
  //   }
  // }

  // for (StoreInst *si : storeVec)
  // {
  //   MemoryLocation s_loc = MemoryLocation::get(si);
  //   for (LoadInst *li : loadVec)
  //   {
  //     MemoryLocation l_loc = MemoryLocation::get(li);
  //     AliasResult andersAAResult = andersAA->query(s_loc, l_loc);
  //     if (andersAAResult == MustAlias)
  //     {
  //       InstructionWrapper *loadInstW = pdgUtils.getInstMap()[li];
  //       InstructionWrapper *storeInstW = pdgUtils.getInstMap()[si];
  //       DDG->addDependency(storeInstW, loadInstW, DependencyType::DATA_ALIAS);
  //       DDG->addDependency(loadInstW, storeInstW, DependencyType::DATA_ALIAS);
  //     }
  //   }
    
  //   for (StoreInst *si1 : storeVec) {
  //     if (si == si1)
  //       continue;
  //     MemoryLocation s1_loc = MemoryLocation::get(si1);
  //     AliasResult andersAAResult = andersAA->query(s_loc, s1_loc);
  //     if (andersAAResult != NoAlias)
  //     {
  //       InstructionWrapper *store1InstW = pdgUtils.getInstMap()[si];
  //       InstructionWrapper *store2InstW = pdgUtils.getInstMap()[si1];
  //       DDG->addDependency(store1InstW, store2InstW, DependencyType::DATA_ALIAS);
  //       DDG->addDependency(store2InstW, store1InstW, DependencyType::DATA_ALIAS);
  //     }
  //   }
  // }

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
  // AU.addRequired<MemoryDependenceWrapperPass>();
  // AU.addRequired<CFLSteensAAWrapperPass>();
  // AU.addRequired<CFLAndersAAWrapperPass>();
  AU.addRequired<AAResultsWrapperPass>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::DataDependencyGraph>
    DDG("ddg", "Data Dependency Graph Construction", false, true);