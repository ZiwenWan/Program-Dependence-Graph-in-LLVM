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
  std::set<Function*> ret;
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
  std::ifstream importedFuncs("imported_func.txt");
  for (std::string line; std::getline(importedFuncs, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (!f)
      continue;
    if (f->isDeclaration() || f->empty() || blackListFuncs.find(f->getName()) != blackListFuncs.end())
      continue;
    crossDomainFuncs.insert(f);
  }
  importedFuncs.close();

  // driver side functions
  // cross-domain function from kernel to driver
  std::ifstream static_func("static_func.txt");
  for (std::string line; std::getline(static_func, line);)
  {
    Function *f = M.getFunction(StringRef(line));
    if (!f)
      continue;
    if (f->isDeclaration() || f->empty() || blackListFuncs.find(f->getName()) != blackListFuncs.end())
      continue;
    crossDomainFuncs.insert(f);
  }
  static_func.close();

  return crossDomainFuncs;
}

std::set<Function *> pdg::PDGUtils::computeTransitiveClosure(Function &F)
{
  std::set<Function*> transClosure;
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
std::set<Function *> pdg::PDGUtils::getTransitiveClosureInDomain(Function &F, std::set<Function*> searchDomain)
{
  std::set<Function*> transClosure;
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
