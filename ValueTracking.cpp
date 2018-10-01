#include "ProgramDependencies.h"

int pdg::ProgramDependencyGraph::getArgOpType(llvm::Argument *arg)
{
    InstructionWrapper *start_stW = instMap[getArgStoreInst(arg)];
    if (start_stW == nullptr) {
        errs() << "Cannot find start stored inst";
        return AccessType::NOACCESS;
    }

    // if find the start store instruction, find all relevant instruction and check type.
    // 1. get consecutive load instruction.
    DependencyNode<InstructionWrapper> *storeDataDNode = PDG->getNodeByData(start_stW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = storeDataDNode->getDependencyList();
    std::vector<InstructionWrapper*> loadWvec;
    for (auto dependencyPair : dataDList)
    {
        if (dependencyPair.second == DependencyType::DATA_RAW)
        {
            loadWvec.push_back(const_cast<InstructionWrapper *>(dependencyPair.first->getData()));
        }
    }

    // 2. find related store instruction.
    std::vector<InstructionWrapper *> storeWvec;
    for (InstructionWrapper *loadInst : loadWvec)
    {
        DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(loadInst);
        dataDList = dataDNode->getDependencyList();
        for (auto dependencyPair : dataDList)
        {
            InstructionWrapper *instW = const_cast<InstructionWrapper *>(dependencyPair.first->getData());
            if (instW->getInstruction() != nullptr && isa<StoreInst>(instW->getInstruction()))
            {
                return AccessType::WRITE_FIELD;
            }
        }
    }

    if (!loadWvec.empty())
    {
        return AccessType::READ_FIELD;
    }

    return AccessType::NOACCESS;
}

llvm::Instruction* pdg::ProgramDependencyGraph::getArgStoreInst(Argument *arg)
{
    for (auto UI = arg->user_begin(); UI != arg->user_end(); ++UI)
    {
        if (isa<StoreInst>(*UI))
        {
            Instruction* st = dyn_cast<Instruction>(*UI); 
            return st;
        }
    }
    return nullptr;
}

llvm::Instruction* pdg::ProgramDependencyGraph::getArgAllocaInst(Argument *arg)
{
    Instruction *inst = getArgStoreInst(arg);
    if (inst == nullptr)
    {
        return nullptr;
    }

    StoreInst *st = dyn_cast<StoreInst>(inst);
    if (isa<AllocaInst>(st->getPointerOperand()))
    {
        Instruction* ai = dyn_cast<Instruction>(st->getPointerOperand());
        return ai;
    }

    return nullptr;
}

int pdg::ProgramDependencyGraph::getAccessTypeForInstW(InstructionWrapper* instW) {
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    int accessType = pdg::NOACCESS;
    for (auto depPair : dataDList)
    {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        int depType = depPair.second;
        if (depType == DATA_READ_FROM)
        {
            accessType = pdg::READ_FIELD;
        }
        if (depType == pdg::DATA_WRITE_TO_REV)
        {
            accessType = pdg::WRITE_FIELD;
            break;
        }
    }

    return accessType;
}

int pdg::ProgramDependencyGraph::getAccessTypeForGEPInstW(InstructionWrapper* instW) {
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    int accessType = pdg::NOACCESS;
    for (auto depPair : dataDList) {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        int depType = depPair.second;
        if (depType == DATA_READ_FROM_REV)
        {
            accessType = pdg::READ_FIELD;
        }
        if (depType == pdg::DATA_WRITE_TO_REV)
        {
            accessType = pdg::WRITE_FIELD;
            break;
        }
        if (depType == pdg::DATA_DEF_USE) {
            if (depInstW->getInstruction() != nullptr && isa<StoreInst>(depInstW->getInstruction())) {
              accessType = pdg::WRITE_FIELD;
              break;
            }
        }
    }
    return accessType;
}

std::set<pdg;:InstructionWrapper*> pdg::ProgramDependencyGraph::getAliasValue(InstructionWrapper* instW) {
  if (instW == nullptr) {
    return nullptr;
  }

  DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
  DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
  std::set<InstructionWrapper *> aliasInstWs;

}

std::set<pdg::InstructionWrapper*> pdg::ProgramDependencyGraph::collectAliasInst(ArgumentWrapper* argW) {
    Instruction* startStoreInst = getArgStoreInst(argW->getArg());
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instMap[startStoreInst]);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList(); 
    std::set<InstructionWrapper*> aliasLoadInstWs;

    // using a bfs like process to collect all possible alias instruction
    std::queue<InstructionWrapper*> instQueue;
    for (auto DepPair : dataDList)  {
        // the dependency here should be read after write.
        // basically the read after the initial store instruction
        InstructionWrapper* instW =  const_cast<InstructionWrapper *>(DepPair.first->getData());
        if (DepPair.second == DATA_RAW) {
            aliasLoadInstWs.insert(instW);
            instQueue.push(instW);
        }
    }

    while (!instQueue.empty()) {
        InstructionWrapper* tmpInstW = instQueue.front();
        instQueue.pop();
        std::vector<InstructionWrapper*> storeInsts;
        dataDNode = PDG->getNodeByData(tmpInstW);
        dataDList = dataDNode->getDependencyList(); 
        for (auto DepPair : dataDList) {
            if (DepPair.second = DATA_WRITE_TO_REV) {
                storeInsts.push_back(const_cast<InstructionWrapper *>(DepPair.first->getData()));
            }
        }

        for (InstructionWrapper* storeInstW : storeInsts) {
            dataDNode = PDG->getNodeByData(storeInstW);
            dataDList = dataDNode->getDependencyList();
            for (auto DepPair : dataDList) {
                if (DepPair.second = DATA_RAW) {
                    InstructionWrapper* instW = const_cast<InstructionWrapper *>(DepPair.first->getData());
                    if (instW->getInstruction() != nullptr && !isa<LoadInst>(instW->getInstruction())) {
                        continue;
                    }
                    if (aliasLoadInstWs.find(instW) != aliasLoadInstWs.end()) {
                      continue;
                    }
                    aliasLoadInstWs.insert(instW);
                    instQueue.push(instW);
                }
            }
        }
    }

    return aliasLoadInstWs;
}


void pdg::ProgramDependencyGraph::getReadWriteInfo(Function *func)
{

    FunctionWrapper *funcW = funcMap[func];
    for (ArgumentWrapper *argW : funcW->getArgWList())
    {
        int argType = getArgType(argW->getArg());
        switch (argType)
        {
        case pdg::SINGLE_VALUE_PTR_TYPE:
            getReadWriteInfoSingleValPtr(argW);
            break;
        case pdg::AGGREGATE_VALUE_PTR_TYPE:
            getReadWriteInfoAggregatePtr(argW);
            break;
        default:
            break;
        }
    }
}

void pdg::ProgramDependencyGraph::getReadWriteInfoSingleValPtr(ArgumentWrapper *argW) {
    Instruction *store_inst = getArgStoreInst(argW->getArg());
    if (store_inst == nullptr) {
        return;
    }
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instMap[store_inst]);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList(); 
    std::set<InstructionWrapper*> aliasLoadInstWs = collectAliasInst(argW);

    auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
    int accessType = pdg::NOACCESS;

    // collect Read/Write Info
    for (InstructionWrapper* instW : aliasLoadInstWs) {
        if (instW->getInstruction() == nullptr) {
            continue;
        }
        int _accessType = getAccessTypeForInstW(instW);
        if (_accessType == pdg::READ_FIELD)
        {
            accessType = _accessType;
        }
        if (_accessType == pdg::WRITE_FIELD)
        {
            accessType = _accessType;
            break;
        }
    }

    (*treeIter)->setAccessType(accessType);
    // formal in tree root
    // ++treeIter; // move the iterator point to underlying buffer typenode

    // actually, no need to track underlying buffer. The underlying buffer only means 
    // value that is pointed to. It essentially a value. We are tracking memory, which
    // is the pointer.
}


void pdg::ProgramDependencyGraph::getReadWriteInfoAggregatePtr(ArgumentWrapper* argW) {
    // here, our foucus is all gep instruction. 
    // during connecting formal tree and function, the dependency between typenode and instructions
    // are already built. In this function, we only need to detect intra function dependencies. 
    // Therefore, we only look at the read/write for the GEP.
    // start by iterating through the type tree. 
    auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
    errs() << argW->getArg()->getParent()->getName() << "\n";
    for (; treeIter != argW->getTree(FORMAL_IN_TREE).end(); ++treeIter) {
        // get typenode - gep pair
        std::vector<std::pair<pdg::InstructionWrapper*, pdg::InstructionWrapper*>> tyNodeGEPPairs = getParameterTreeNodeWithCorrespondGEP(argW, treeIter);
        for (auto tyNodeGEPPair : tyNodeGEPPairs)  {
            InstructionWrapper* typeNodeW = tyNodeGEPPair.first;
            InstructionWrapper* gepInstW = tyNodeGEPPair.second;
            // get read/writ information for the GEP, and store to the typenode
            errs() << *gepInstW->getInstruction() << "\n";

            int gepAccessType = getAccessTypeForGEPInstW(gepInstW);
            int old_type = typeNodeW->getAccessType();

            if (gepAccessType <= old_type) {
              // if access info not changed, continue processing
              continue;
            }
            typeNodeW->setAccessType(gepAccessType);
        }
    }
}