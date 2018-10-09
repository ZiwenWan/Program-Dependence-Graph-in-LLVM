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
    // collect alias instructions, including store and load instructions
    for (auto depPair : dataDList) {
        int depType = depPair.second;
        InstructionWrapper* depInstW = const_cast<InstructionWrapper*>(depPair.first->getData());
        if (depType == DATA_ALIAS) {
            aliasPtr.insert(depInstW);
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
        if (depType == pdg::DATA_DEF_USE)
        {
            if (StoreInst *st = dyn_cast<StoreInst>(depInstW->getInstruction()))
            {
                if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
                {
                    accessType = pdg::WRITE_FIELD;
                    break;
                }
            }
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
        if (depType == pdg::DATA_DEF_USE)
        {
            if (StoreInst *st = dyn_cast<StoreInst>(depInstW->getInstruction()))
            {
                if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction())
                {
                    accessType = pdg::WRITE_FIELD;
                    break;
                }
            }
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

void pdg::ProgramDependencyGraph::mergeTypeTreeReadAndWriteInfo(ArgumentWrapper* argW, tree<InstructionWrapper *>::iterator mergeTo, tree<InstructionWrapper *>::iterator mergeFrom)
{
    for (; mergeTo != argW->getTree(FORMAL_IN_TREE).end(); ++mergeTo, ++mergeFrom)
    {
        if ((*mergeFrom)->getAccessType() > (*mergeTo)->getAccessType())
        {
            errs() << "Merging info: " << (*mergeFrom)->getAccessType() << " - " << (*mergeTo)->getAccessType() << "\n";
            // here, we only copy the write state
            (*mergeTo)->setAccessType((*mergeFrom)->getAccessType());
        }
    }
}

void pdg::ProgramDependencyGraph::mergeArgWReadWriteInfo(ArgumentWrapper* callerArgW, ArgumentWrapper* calleeArgW) {
    // firstly detect if the argument type is actually matching.
    // if not, then do nothing

    int argMatchType = getArgMatchType(callerArgW->getArg(), calleeArgW->getArg());

    tree<InstructionWrapper*>::iterator calleeFuncArgTreeIter = calleeArgW->getTree(FORMAL_IN_TREE).begin();

    // Equal type case: 
    if (argMatchType == pdg::ArgumentContainType::EQUAL)
    {
        errs() << "Merging func arg use info: " << callerArgW->getArg()->getParent()->getName() << " - " << calleeArgW->getArg()->getParent()->getName() << "\n";
        // if the argument type matches exactly, simply merge the whole tree
        mergeTypeTreeReadAndWriteInfo(callerArgW, callerArgW->getTree(FORMAL_IN_TREE).begin(), calleeFuncArgTreeIter);
    }

    // Subfield case:
    if (argMatchType == pdg::ArgumentContainType::CONTAINED)
    {
        // here, we move the iterator to the sube tree passed to the other function, and then start from
        // that sub tree, we merge information recursively.
        auto callerArgWTI = callerArgW->getTree(FORMAL_IN_TREE).begin();
        auto callerArgWTIE = callerArgW->getTree(FORMAL_IN_TREE).end();
        for (; callerArgWTI != callerArgWTIE; ++callerArgWTI)
        {
            auto tyNodeWithGEPs = getParameterTreeNodeWithCorrespondGEP(callerArgW, callerArgWTI);
            if (tyNodeWithGEPs.size() == 0) {
                continue;
            }

            for (auto tyNodeGEPPair : tyNodeWithGEPs)
            {
                tree<InstructionWrapper *>::iterator treeIter = callerArgW->getTree(FORMAL_IN_TREE).begin();
                tree<InstructionWrapper *>::iterator treeIterEnd = callerArgW->getTree(FORMAL_IN_TREE).end();
                while (treeIter != treeIterEnd)
                {
                    if ((*treeIter) == tyNodeGEPPair.first)
                    {
                        break;
                    }
                    ++treeIter;
                }

                if (treeIter == treeIterEnd)
                {
                    return;
                }

                mergeTypeTreeReadAndWriteInfo(callerArgW, treeIter, calleeFuncArgTreeIter);
                break;
            }
        }
    }
}

int pdg::ProgramDependencyGraph::getCallParamIdx(InstructionWrapper* instW, InstructionWrapper* callInstW)
{
    Instruction *inst = instW->getInstruction();
    Instruction *callInst = callInstW->getInstruction();
    if (inst == nullptr || callInst == nullptr)
    {
        return -1;
    }

    if (CallInst *CI = dyn_cast<CallInst>(callInst))
    {
        int paraIdx = 0;
        for (auto arg_iter = CI->arg_begin(); arg_iter != CI->arg_end(); ++arg_iter)
        {
            if (Instruction *tmpInst = dyn_cast<Instruction>(&*arg_iter))
            {
                if (tmpInst == inst)
                {
                    return paraIdx;
                }
            }
            paraIdx++;
        }
    }
    return -1;
}

void pdg::ProgramDependencyGraph::getInterFuncReadWriteInfo(llvm::Function* func) {
    FunctionWrapper* funcW = funcMap[func];
    for (ArgumentWrapper* argW : funcW->getArgWList()) {
        std::set<InstructionWrapper*> aliasPtrInstWs = getAliasPtrForArgInFunc(argW);
        for (InstructionWrapper* aliasPtrInstW : aliasPtrInstWs) {
            DependencyNode<InstructionWrapper> *dataDNode = PDG->getNodeByData(aliasPtrInstW);
            DependencyNode<InstructionWrapper>::DependencyLinkList dataDList = dataDNode->getDependencyList();
            for (auto depPair : dataDList)
            {
                int depType = depPair.second;
                if (depType == DATA_CALL_PARA)
                {
                    InstructionWrapper *depInstW = const_cast<InstructionWrapper *>(depPair.first->getData());
                    int callParaIdx = getCallParamIdx(aliasPtrInstW, depInstW);
                    CallInst *calleeInst = dyn_cast<CallInst>(depInstW->getInstruction());
                    Function* calleeFunc = calleeInst->getCalledFunction();

                    if (calleeFunc == nullptr)
                    {
                        Type *t = calleeInst->getCalledValue()->getType();
                        FunctionType *funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
                        errs() << "Collecting indirect call in func: " << func->getName() << "\n";
                        std::vector<llvm::Function *> indirect_call_candidates = collectIndirectCallCandidates(funcTy);
                        for (llvm::Function *indirect_call : indirect_call_candidates)
                        {
                            FunctionWrapper *calleeFuncW = funcMap[indirect_call];
                            ArgumentWrapper *calleeArgW = getArgWByIdx(calleeFuncW, callParaIdx);
                            mergeArgWReadWriteInfo(argW, calleeArgW);
                        }
                    }
                    else
                    {
                        FunctionWrapper *calleeFuncW = funcMap[calleeFunc];
                        ArgumentWrapper *calleeArgW = getArgWByIdx(calleeFuncW, callParaIdx);
                        mergeArgWReadWriteInfo(argW, calleeArgW);
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
        // alais has a store, means the same locations is accessed. We mark the argW as 
        // being written
        if (StoreInst* st = dyn_cast<StoreInst>(instW->getInstruction())) {
            if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction() ) {
                errs() << "Testing " << argW->getArg()->getParent()->getName() << *instW->getInstruction() << "\n";
                accessType = pdg::WRITE_FIELD;
                break;
            }
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

void pdg::ProgramDependencyGraph::getReadWriteInfoAggregatePtr(ArgumentWrapper *argW)
{
    // here, our foucus is all gep instruction.
    // during connecting formal tree and function, the dependency between typenode and instructions
    // are already built. In this function, we only need to detect intra function dependencies.
    // Therefore, we only look at the read/write for the GEP.
    // firstly, need to process the root node firstly
    auto treeIter = argW->getTree(FORMAL_IN_TREE).begin();
    std::set<InstructionWrapper *> aliasPtrInstWs = getAliasPtrForArgInFunc(argW);
    // for each alias ptr, we collect read and write to that location.
    // then, merging all information together.
    int accessType = pdg::NOACCESS;
    for (InstructionWrapper *instW : aliasPtrInstWs)
    {
        // same here as single ptr 
        if (StoreInst* st = dyn_cast<StoreInst>(instW->getInstruction())) {
            if (dyn_cast<Instruction>(st->getPointerOperand()) == instW->getInstruction() ) {
                accessType = pdg::WRITE_FIELD;
                break;
            }
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
    // then, start processing each field
    // iterating through the type tree.
    for (; treeIter != argW->getTree(FORMAL_IN_TREE).end(); ++treeIter)
    {
        // get typenode - gep pair
        auto tyNodeGEPPairs = getParameterTreeNodeWithCorrespondGEP(argW, treeIter);
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
                bool pointed_type_match = ((dyn_cast<PointerType>(arg2_type))->getElementType() == arg1_element_type);
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