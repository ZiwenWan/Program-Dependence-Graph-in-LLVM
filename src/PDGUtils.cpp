#include "PDGUtils.hpp" 
#include "llvm/IR/InstIterator.h"

using namespace llvm;

void pdg::PDGUtils::constructInstMap(Function &F)
{
  for (inst_iterator I = inst_begin(F); I != inst_end(F); ++I)
  {
    if (G_instMap.find(&*I) == G_instMap.end())
    {
      GraphNodeType nodeTy = GraphNodeType::INST;
      if (isa<CallInst>(&*I))
        nodeTy = GraphNodeType::CALL;
      if (isa<ReturnInst>(&*I))
        nodeTy = GraphNodeType::RETURN;

      InstructionWrapper *instW = new InstructionWrapper(&*I, nodeTy);
      G_instMap[&*I] = instW;
      G_funcInstWMap[&F].insert(instW);
      DIType *dt = getInstDIType(&*I);
      if (dt != nullptr)
        G_InstDITypeMap[&*I] = dt;
    }
  }
  categorizeInstInFunc(F);
}

void pdg::PDGUtils::constructFuncMap(Module &M)
{
  for (Module::iterator FI = M.begin(); FI != M.end(); ++FI)
  {
    if (FI->isDeclaration())
      continue;

    if (G_funcMap.find(&*FI) == G_funcMap.end())
    {
      FunctionWrapper *funcW = new FunctionWrapper(&*FI);
      G_funcMap[&*FI] = funcW;
      constructInstMap(*FI);
    }
  }
}

DIType *pdg::PDGUtils::getInstDIType(Instruction* inst)
{
  Function* parentFunc = inst->getFunction();
  auto dbgInsts = DIUtils::collectDbgInstInFunc(*parentFunc);
  std::vector<DbgInfoIntrinsic *> dbgInstList(dbgInsts.begin(), dbgInsts.end());
  if (AllocaInst *ai = dyn_cast<AllocaInst>(inst))
  {
    DIType* allocDIType = DIUtils::getInstDIType(ai, dbgInstList);
    return allocDIType;
  }
  
  if (LoadInst *li = dyn_cast<LoadInst>(inst))
  {
    if (Instruction *sourceInst = dyn_cast<Instruction>(li->getPointerOperand()))
    {
      if (G_InstDITypeMap.find(sourceInst) == G_InstDITypeMap.end())
        return nullptr;
      DIType* sourceInstDIType = G_InstDITypeMap[sourceInst];
      DIType* retDIType = DIUtils::stripAttributes(sourceInstDIType);
      return retDIType;
    }
    if (GlobalVariable *gv = dyn_cast<GlobalVariable>(li->getPointerOperand()))
    {
      DIType* sourceGlobalVarDIType = DIUtils::getGlobalVarDIType(*gv);
      return sourceGlobalVarDIType;
    }
  }

  if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(inst))
  {
    if (Instruction *sourceInst = dyn_cast<Instruction>(gep->getPointerOperand()))
    {
      if (G_InstDITypeMap.find(sourceInst) == G_InstDITypeMap.end())
        return nullptr;
      DIType *sourceInstDIType = G_InstDITypeMap[sourceInst];
      sourceInstDIType = DIUtils::stripAttributes(sourceInstDIType);
      if (DIUtils::isStructTy(sourceInstDIType) || DIUtils::isStructPointerTy(sourceInstDIType))
      {
        sourceInstDIType = DIUtils::getLowestDIType(sourceInstDIType);
        auto DINodeArr = dyn_cast<DICompositeType>(sourceInstDIType)->getElements();
        StructType *structTy = getStructTypeFromGEP(gep);
        if (structTy == nullptr)
          return nullptr;
        for (unsigned i = 0; i < DINodeArr.size(); ++i)
        {
          DIType *fieldDIType = dyn_cast<DIType>(DINodeArr[i]);
          if (isGEPOffsetMatchWithDI(structTy, fieldDIType, gep))
            return fieldDIType;
        }
      }
    }
  }

  if (CastInst *castInst = dyn_cast<CastInst>(inst))
  {
    if (Instruction *sourceInst = dyn_cast<Instruction>(castInst->getOperand(0)))
    {
      if (G_InstDITypeMap.find(sourceInst) == G_InstDITypeMap.end())
        return nullptr;
      return G_InstDITypeMap[sourceInst];
    }
  }

  return nullptr;
}

unsigned pdg::PDGUtils::getGEPAccessFieldOffset(GetElementPtrInst *gep)
{
  int operand_num = gep->getNumOperands();
  llvm::Value *last_idx = gep->getOperand(operand_num - 1);
  // cast the last_idx to int type
  if (llvm::ConstantInt *constInt = dyn_cast<ConstantInt>(last_idx))
    return constInt->getSExtValue();
  return -1;
}

bool pdg::PDGUtils::isGEPforBitField(GetElementPtrInst* gep)
{
  for (auto u : gep->users())
  {
    if (LoadInst *li = dyn_cast<LoadInst>(u))
    {
      for (auto user : li->users())
      {
        if (isa<LShrOperator>(user))
          return true;
      }
    }
  }
  return false;
}

Value *pdg::PDGUtils::getLShrOnGep(GetElementPtrInst* gep)
{
  for (auto u : gep->users())
  {
    if (LoadInst *li = dyn_cast<LoadInst>(u))
    {
      for (auto user : li->users())
      {
        if (isa<LShrOperator>(user))
          return user;
      }
    }
  }
  return nullptr;
}

uint64_t pdg::PDGUtils::getGEPOffsetInBits(StructType *structTy, GetElementPtrInst *gep)
{
  // get the accessed struct member offset from the gep instruction
  Module *module = gep->getFunction()->getParent();
  unsigned gepFieldOffset = getGEPAccessFieldOffset(gep);
  if (gepFieldOffset < 0 || gepFieldOffset >= structTy->getNumElements())
    return -1;
  // use the struct layout to figure out the offset in bits
  auto const &dataLayout = module->getDataLayout();
  auto const structLayout = dataLayout.getStructLayout(structTy);
  uint64_t fieldOffsetInBits = structLayout->getElementOffsetInBits(gepFieldOffset);
  // check if the gep may be used for accessing bit fields
  if (isGEPforBitField(gep))
  {
    // compute the accessed bit offset here
    if (auto LShrInst = dyn_cast<LShrOperator>(getLShrOnGep(gep)))
    {
      auto LShrOffsetOp = LShrInst->getOperand(1);
      if (ConstantInt *constInst = dyn_cast<ConstantInt>(LShrOffsetOp))
      {
        fieldOffsetInBits += constInst->getSExtValue();
      }
    }
  }
  // other situations?
  return fieldOffsetInBits;
}

bool pdg::PDGUtils::isGEPOffsetMatchWithDI(StructType *structTy, DIType *dt, Instruction *gep)
{
  if (structTy == nullptr)
    return false;
  if (auto gepInst = dyn_cast<GetElementPtrInst>(gep))
  {
    uint64_t gepAccessMemOffset = getGEPOffsetInBits(structTy, gepInst);
    if (dt == nullptr || gepAccessMemOffset < 0)
      return false;
    uint64_t debuggingOffset = dt->getOffsetInBits();
    if (gepAccessMemOffset == debuggingOffset)
      return true;
  }
  return false;
}

StructType *pdg::PDGUtils::getStructTypeFromGEP(Instruction *inst)
{
  if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(inst))
  {
    Value *baseAddr = gep->getPointerOperand();
    if (baseAddr->getType()->isPointerTy())
    {
      if (StructType *structTy = dyn_cast<StructType>(baseAddr->getType()->getPointerElementType()))
        return structTy;
    }
  }
  return nullptr;
}

void pdg::PDGUtils::collectGlobalInsts(Module &M)
{
  for (Module::global_iterator globalIt = M.global_begin(); globalIt != M.global_end(); ++globalIt)
  {
    InstructionWrapper *globalW = new InstructionWrapper(dyn_cast<Value>(&(*globalIt)), GraphNodeType::GLOBAL_VALUE);
    G_globalInstsSet.insert(globalW);
  }
}

void pdg::PDGUtils::categorizeInstInFunc(Function &F)
{
  // sort store/load/return/CallInst in function
  for (inst_iterator I = inst_begin(F), IE = inst_end(F); I != IE; ++I)
  {
    Instruction *inst = dyn_cast<Instruction>(&*I);
    if (isa<StoreInst>(inst))
      G_funcMap[&F]->addStoreInst(inst);

    if (isa<LoadInst>(inst))
      G_funcMap[&F]->addLoadInst(inst);

    if (isa<ReturnInst>(inst))
      G_funcMap[&F]->addReturnInst(inst);

    if (CallInst *ci = dyn_cast<CallInst>(inst))
    {
      if (isa<DbgInfoIntrinsic>(ci))
        G_funcMap[&F]->addDbgInst(inst);
      else
        G_funcMap[&F]->addCallInst(inst);
    }

    if (CastInst *bci = dyn_cast<CastInst>(inst))
      G_funcMap[&F]->addCastInst(inst);

    if (IntrinsicInst *ii = dyn_cast<IntrinsicInst>(inst))
      G_funcMap[&F]->addIntrinsicInst(inst);
  }
}

std::set<Function *> pdg::PDGUtils::computeDriverDomainFuncs(Module &M)
{
  std::set<Function *> driverDomainFuncs;
  std::ifstream driverFuncs("defined_func.txt");
  // construct boundary
  // construct driver domain functions
  for (std::string line; std::getline(driverFuncs, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (f != nullptr)
      driverDomainFuncs.insert(f);
  }

  return driverDomainFuncs;
}

std::set<Function *> pdg::PDGUtils::computeKernelDomainFuncs(Module &M)
{
  std::set<Function *> kernelDomainFuncs;
  auto driverDomainFuncs = computeDriverDomainFuncs(M);
  for (Function &F : M)
  {
    if (F.isDeclaration() || F.empty())
      continue;
    if (driverDomainFuncs.find(&F) == driverDomainFuncs.end())
      kernelDomainFuncs.insert(&F);
  }

  return kernelDomainFuncs;
}

std::set<Function *> pdg::PDGUtils::computeImportedFuncs(Module &M)
{
  std::set<Function *> ret;
  std::ifstream importedFuncs("imported_func.txt");
  for (std::string line; std::getline(importedFuncs, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (!f)
      continue;
    if (f->isDeclaration() || f->empty())
      continue;
    ret.insert(f);
  }
  importedFuncs.close();
  return ret;
}

std::set<std::string> pdg::PDGUtils::getBlackListFuncs()
{
  std::set<std::string> ret;
  std::ifstream blackListFuncs("liblcd_funcs.txt");
  for (std::string line; std::getline(blackListFuncs, line);)
  {
    ret.insert(line);
  }
  return ret;
}

std::set<Function *> pdg::PDGUtils::computeCrossDomainFuncs(Module &M)
{
  std::set<Function *> crossDomainFuncs;
  auto blackListFuncs = getBlackListFuncs();
  // cross-domain function from driver to kernel
  unsigned importedFuncNum = 0;
  std::ifstream importedFuncs("imported_func.txt");
  for (std::string line; std::getline(importedFuncs, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (!f)
      continue;
    if (f->isDeclaration() || f->empty() || blackListFuncs.find(f->getName()) != blackListFuncs.end())
      continue;
    crossDomainFuncs.insert(f);
    importedFuncNum++;
  }
  importedFuncs.close();
  // driver side functions
  // cross-domain function from kernel to driver
  std::ifstream static_func("static_func.txt");
  unsigned callBackFuncNum = 0;
  for (std::string line; std::getline(static_func, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (!f)
      continue;
    if (f->isDeclaration() || f->empty() || blackListFuncs.find(f->getName()) != blackListFuncs.end())
      continue;
    crossDomainFuncs.insert(f);
    callBackFuncNum++;
  }
  static_func.close();

  // init module function
  auto initFunc = M.getFunction("dummy_init_module");
  if (initFunc != nullptr)
    crossDomainFuncs.insert(initFunc);
  return crossDomainFuncs;
}

std::set<Function *> pdg::PDGUtils::computeTransitiveClosure(Function &F)
{
  std::set<Function *> transClosure;
  std::queue<Function *> funcQ;
  transClosure.insert(&F);
  funcQ.push(&F);

  while (!funcQ.empty())
  {
    Function *func = funcQ.front();
    funcQ.pop();
    auto callInstList = G_funcMap[func]->getCallInstList();
    for (auto ci : callInstList)
    {
      if (Function *calledF = dyn_cast<Function>(ci->getCalledValue()->stripPointerCasts()))
      {
        if (calledF->isDeclaration() || calledF->empty())
          continue;
        if (transClosure.find(calledF) != transClosure.end()) // skip if we already added the called function to queue.
          continue;
        transClosure.insert(calledF);
        funcQ.push(calledF);
      }
    }
  }

  return transClosure;
}

std::set<std::string> pdg::PDGUtils::computeDriverExportFuncPtrName()
{
  // compute a set of pointer pointer name provided by the driver side
  std::set<std::string> driverExportFuncPtrNames;
  std::ifstream driverExportFuncPtrs("static_funcptr.txt");
  for (std::string line; std::getline(driverExportFuncPtrs, line);)
  {
    driverExportFuncPtrNames.insert(line);
  }
  driverExportFuncPtrs.close();
  return driverExportFuncPtrNames;
}

std::map<std::string, std::string> pdg::PDGUtils::computeDriverExportFuncPtrNameMap()
{
  std::ifstream driverExportFuncPtrs("static_funcptr.txt");
  std::ifstream driverExportFuncs("static_func.txt");
  std::map<std::string, std::string> exportFuncPtrMap;
  // int s1 = std::count(std::istreambuf_iterator<char>(driverExportFuncPtrs), std::istreambuf_iterator<char>(), '\n');
  // int s2 = std::count(std::istreambuf_iterator<char>(driverExportFuncs), std::istreambuf_iterator<char>(), '\n');
  // assert(s1 == s2 && "driver export ptrs cannot be matched to a defined function.");
  for (std::string line1, line2; std::getline(driverExportFuncPtrs, line1), std::getline(driverExportFuncs, line2);)
  {
    exportFuncPtrMap[line2] = line1; // key: registered driver side function, value: the registered function pointer name
  }

  return exportFuncPtrMap;
}

// compute trasitive closure
std::set<Function *> pdg::PDGUtils::getTransitiveClosureInDomain(Function &F, std::set<Function *> searchDomain)
{
  std::set<Function *> transClosure;
  std::queue<Function *> funcQ;
  funcQ.push(&F);

  while (!funcQ.empty())
  {
    Function *func = funcQ.front();
    funcQ.pop();
    auto callInstList = G_funcMap[func]->getCallInstList();
    for (auto ci : callInstList)
    {
      if (ci->getCalledFunction() == nullptr) // indirect call
        continue;
      Function *calledF = ci->getCalledFunction();
      if (calledF->isDeclaration() || calledF->empty())
        continue;
      if (searchDomain.find(calledF) == searchDomain.end()) // skip if not in the receiver domain
        continue;
      if (transClosure.find(calledF) != transClosure.end()) // skip if we already added the called function to queue.
        continue;
      transClosure.insert(calledF);
      funcQ.push(calledF);
    }
  }

  return transClosure;
}
