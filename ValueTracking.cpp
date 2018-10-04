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

void pdg::ProgramDependencyGraph::collectRelevantCallInstsForArg(ArgumentWrapper* argW, InstructionWrapper* instW) {
    DependencyNode<InstructionWrapper>* dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    for (auto depPair : dataDList) {
        int depType = depPair.second;
        if (depType == pdg::DATA_CALL_PARA) {
            InstructionWrapper* callInstW = const_cast<InstructionWrapper*>(depPair.first->getData());
            argW->addRelevantCallInsts(callInstW);
        }
    }
}

std::set<pdg::InstructionWrapper *>
pdg::ProgramDependencyGraph::getAliasPtrForArgInFunc(ArgumentWrapper *argW)
{
    Instruction* startStInst = getArgStoreInst(argW->getArg());
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instMap[startStInst]);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();

    std::set<InstructionWrapper*> aliasPtr;
    // collect alias load instructions
    for (auto depPair : dataDList) {
        int depType = depPair.second;
        InstructionWrapper* depInstW = const_cast<InstructionWrapper*>(depPair.first->getData());
        if (depType == DATA_RAW) {
            aliasPtr.insert(depInstW);
            collectRelevantCallInstsForArg(argW, depInstW);
        }
    }

    // collect alais gep instructions
    std::set<InstructionWrapper*> relevantGEPs = getAllRelevantGEP(argW->getArg());
    aliasPtr.insert(relevantGEPs.begin(), relevantGEPs.end());
    return aliasPtr;
}

int pdg::ProgramDependencyGraph::getAccessTypeForInstW(InstructionWrapper* instW) {
    DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(instW);
    DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
    int accessType = pdg::NOACCESS;
    for (auto depPair : dataDList)
    {
        InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
        int depType = depPair.second;
        if (depType == DATA_DEF_USE)
        {
            accessType = pdg::READ_FIELD;
        }
        if (depType == pdg::DATA_DEF_USE && isa<StoreInst>(depInstW->getInstruction()))
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
        if (depType == DATA_DEF_USE && accessType < pdg::WRITE_FIELD)
        {
            accessType = pdg::READ_FIELD;
        }
        if (depType == pdg::DATA_DEF_USE && isa<StoreInst>(depInstW->getInstruction()))
        {
            errs() << "Write: " << *depInstW->getInstruction() << "\n";
            accessType = pdg::WRITE_FIELD;
            break;
        }
    }
    return accessType;
}

void pdg::ProgramDependencyGraph::getIntraFuncReadWriteInfo(Function *func)
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

    for (ArgumentWrapper* argW : funcMap[func]->getArgWList()) {
        std::set<InstructionWrapper*> aliasPtrSet = getAliasPtrForArgInFunc(argW);
        for (InstructionWrapper* aliasInstW : aliasPtrSet) {
            collectRelevantCallInstsForArg(argW, aliasInstW);
        }
    }
}

void pdg::ProgramDependencyGraph::getInterFuncReadWriteInfo(llvm::Function* func) {
    FunctionWrapper *funcW = funcMap[func];
    errs() << "Start getting inter func arg use for func: " << func->getName() << "\n";
    for (ArgumentWrapper* argW : funcW->getArgWList())
    {
        for (InstructionWrapper* callInstW : argW->getRelevantCallInsts()) {
            // handle indirect call first
            llvm::Instruction* callInst = callInstW->getInstruction();
            if (callInst == nullptr) {
                return;
            }

            // start processing direct call
            // here, what we do basically is iterating through all related call instructions
            // find correspond argument wrapper, and merge the read and write information. 
            llvm::Function* calledFunc = dyn_cast<CallInst>(callInst)->getCalledFunction();
            if (calledFunc != nullptr)
            {
                FunctionWrapper *calledFuncW = funcMap[calledFunc];
                for (ArgumentWrapper *calledFuncArgW : calledFuncW->getArgWList())
                {
                    int argMatchType = getArgMatchType(argW->getArg(), calledFuncArgW->getArg());
                    auto calledFuncArgTreeIter = calledFuncArgW->getTree(FORMAL_IN_TREE).begin();
                    errs() << "Arg match type: " << calledFunc->getName() << "--" << argMatchType << "\n";
                    if (argMatchType == pdg::ArgumentContainType::EQUAL)
                    {
                        // if the argument type matches exactly, we
                        mergeTypeTreeReadAndWriteInfo(argW, argW->getTree(FORMAL_IN_TREE).begin(), calledFuncArgTreeIter);
                    }
                    if (argMatchType == pdg::ArgumentContainType::CONTAINED)
                    {
                        // here, we move the iterator to the sube tree passed to the other function, and then start from 
                        // that sub tree, we merge information recursively.
                        std::vector<std::pair<InstructionWrapper *, InstructionWrapper *>> tyNodeWithGEP = getParameterTreeNodeWithCorrespondGEP(argW, argW->getTree(FORMAL_IN_TREE).begin());
                        for (auto tyNodeGEPPair : tyNodeWithGEP) {
                            auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
                            auto treeIterEnd = argW->getTree(FORMAL_IN_TREE).end();
                            while (treeIter != treeIterEnd )
                            {
                                if ((*treeIter) != tyNodeGEPPair.first) {
                                    break;
                                }
                                ++treeIter;
                            }

                            if (treeIter == treeIterEnd)
                            {
                                return;
                            }
                            mergeTypeTreeReadAndWriteInfo(argW, treeIter, calledFuncArgTreeIter);
                        }
                    }
                }
            }
        }
    }
}

void pdg::ProgramDependencyGraph::getReadWriteInfoSingleValPtr(ArgumentWrapper *argW)
{
    auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
    int accessType = pdg::NOACCESS;
    // here, we get all possible alias loads, and alias gep. These pointers set representing
    // all possible location for the argument.
    std::set<InstructionWrapper *> aliasPtrInstWs = getAliasPtrForArgInFunc(argW);
    // for each alias ptr, we collect read and write to that location.
    // then, merging all information together.
    for (InstructionWrapper *instW : aliasPtrInstWs)
    {
        int _accessType = getAccessTypeForInstW(instW);
        if (_accessType == pdg::READ_FIELD)
        {
            accessType = _accessType;
        }
        if (_accessType == pdg::WRITE_FIELD)
        {
            accessType = _accessType;
        }
    }

    (*treeIter)->setAccessType(accessType);
    // formal in tree root
    // ++treeIter; // move the iterator point to underlying buffer typenode
    // actually, no need to track underlying buffer. The underlying buffer only means
    // value that is pointed to. It essentially a value. We are tracking memory, which
    // is the pointer.
}

void pdg::ProgramDependencyGraph::getReadWriteInfoAggregatePtr(ArgumentWrapper *argW)
{
    // here, our foucus is all gep instruction.
    // during connecting formal tree and function, the dependency between typenode and instructions
    // are already built. In this function, we only need to detect intra function dependencies.
    // Therefore, we only look at the read/write for the GEP.
    // start by iterating through the type tree.
    auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
    for (; treeIter != argW->getTree(FORMAL_IN_TREE).end(); ++treeIter)
    {
        // get typenode - gep pair
        std::vector<std::pair<pdg::InstructionWrapper *, pdg::InstructionWrapper *>> tyNodeGEPPairs = getParameterTreeNodeWithCorrespondGEP(argW, treeIter);
        for (auto tyNodeGEPPair : tyNodeGEPPairs)
        {
            InstructionWrapper *typeNodeW = tyNodeGEPPair.first;
            InstructionWrapper *gepInstW = tyNodeGEPPair.second;
            // get read/writ information for the GEP, and store to the typenode
            int gepAccessType = getAccessTypeForGEPInstW(gepInstW);
            int old_type = typeNodeW->getAccessType();

            if (gepAccessType <= old_type)
            {
                // if access info not changed, continue processing
                continue;
            }
            typeNodeW->setAccessType(gepAccessType);
        }
    }
}

int pdg::ProgramDependencyGraph::getArgMatchType(llvm::Argument *arg1, llvm::Argument *arg2)
{
    Type *arg1_type = arg1->getType();
    Type *arg2_type = arg2->getType();

    if (arg1_type == arg2_type)
    {
        return pdg::ArgumentContainType::EQUAL;
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
                bool pointed_type_match = (dyn_cast<PointerType>(arg2_type))->getElementType() == arg1_element_type;
                type_match = type_match || pointed_type_match;
            }

            if (type_match)
            {
                return pdg::ArgumentContainType::CONTAINED;
            }
        }
    }

    return pdg::ArgumentContainType::NOTCONTAINED;
}

#if 0
void pdg::ProgramDependencyGraph::printSensitiveFunctions() {
   std::vector<InstructionWrapper*> sensitive_nodes;
   std::set<llvm::GlobalValue *> senGlobalSet;
   //std::set<llvm::Instruction *> senInstSet;
   std::set<llvm::Function *> async_funcs;
   std::deque<const InstructionWrapper*> queue;
   std::set<InstructionWrapper* > coloredInstSet;

   auto global_annos = this->module->getNamedGlobal("llvm.global.annotations"); 
   if(global_annos){
       auto casted_array = cast<ConstantArray>(global_annos->getOperand(0));
       for (int i = 0; i < casted_array->getNumOperands(); i++) {
           auto casted_struct =
           cast<ConstantStruct>(casted_array->getOperand(i)); if (auto sen_gv
           =
           dyn_cast<GlobalValue>(casted_struct->getOperand(0)->getOperand(0)))
           {
               auto anno =
               cast<ConstantDataArray>(cast<GlobalVariable>(casted_struct->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();
               if (anno == "sensitive") {
                   errs() << "sensitive global found! value = " << *sen_gv <<
                   "\n"; senGlobalSet.insert(sen_gv);
               }
           }
           if (auto fn = dyn_cast<Function>(casted_struct->getOperand(0)->getOperand(0))) {
               auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(casted_struct->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();
               if (anno == "sensitive") {
                   async_funcs.insert(fn);
                   errs() << "async_funcs ++ sensitive " << fn->getName() <<
                   "\n";
               }
           }
       }
   }

   for(std::set<InstructionWrapper*>::iterator nodeIt = instnodes.begin();
   nodeIt != instnodes.end(); ++nodeIt){
       InstructionWrapper *InstW = *nodeIt;
       llvm::Instruction *pInstruction = InstW->getInstruction();

       for(int i = 0; i < sensitive_values.size(); i++){
           if(sensitive_values[i] == pInstruction){
               errs() << "sensitive_values " << i << " == "<< *pInstruction
               << "\n"; sensitive_nodes.push_back(InstW);
           }
       }

       if(InstW->getType() == GLOBAL_VALUE){
           GlobalValue *gv = dyn_cast<GlobalValue>(InstW->getValue());
           if(senGlobalSet.end() != senGlobalSet.find(gv)){
               errs() << "sensitive_global: " << *gv <<"\n";
               sensitive_nodes.push_back(InstW);
           }
       }
   }

   for(int i = 0; i < sensitive_nodes.size(); i++){
       queue.push_back(sensitive_nodes[i]);
   }

   errs() << "queue.size = " << queue.size() << "\n";

   while(!queue.empty()){
       InstructionWrapper *InstW = const_cast<InstructionWrapper*>(queue.front()); 
       if(InstW->getType() == INST){
           funcMap[InstW->getFunction()]->setVisited(true);
       }
       queue.pop_front();

       if(InstW->getValue() != nullptr) {
           coloredInstSet.insert(InstW);
       }

       DependencyNode<InstructionWrapper>* DNode = PDG->getNodeByData(InstW);
       for(int i = 0; i < DNode->getDependencyList().size(); i++){
           if(nullptr != DNode->getDependencyList()[i].first->getData()){
               InstructionWrapper *adjacent_InstW =
               *instnodes.find(const_cast<InstructionWrapper*>(DNode->getDependencyList()[i].first->getData()));
               if(true != adjacent_InstW->getFlag()){
                   queue.push_back(DNode->getDependencyList()[i].first->getData());
                   adjacent_InstW->setFlag(true); //label the adjacent node visited
               }
           }
       }
   }

   errs() << "\n\n++++++++++The FUNCTION List is as follows:++++++++++\n\n";

   unsigned int funcs_count = 0;
   unsigned int sen_funcs_count = 0;
   unsigned int ins_funcs_count = 0;

   std::set<FunctionWrapper*> sen_FuncSet;
   std::set<FunctionWrapper*> ins_FuncSet;

   std::map<const llvm::Function *, FunctionWrapper *>::iterator FI =
           funcMap.begin();
   std::map<const llvm::Function *, FunctionWrapper *>::iterator FE =
           funcMap.end();
   for (; FI != FE; ++FI) {
       if (!(*FI).first->isDeclaration()) {
           funcs_count++;
           if ((*FI).second->hasFuncOrFilePtr()) {
               errs() << (*FI).first->getName() << " hasFuncOrFilePtr()\n";
           }

           if ((*FI).second->isVisited()) {
               errs() << (*FI).first->getName() << " is
               colored(sensitive)\n";

               Function* func = (*FI).second->getFunction();
               errs() << "func name = " << func->getName() << "\n";

               sen_FuncSet.insert((*FI).second);
           } else {
               errs() << (*FI).first->getName() << " is uncolored\n";
               ins_FuncSet.insert((*FI).second);
           }
       }
   }
}
#endif