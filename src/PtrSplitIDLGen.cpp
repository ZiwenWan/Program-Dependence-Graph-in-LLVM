#include "PtrSplitIDLGen.hpp"
using namespace llvm;

char pdg::PtrSplitIDLGen::ID = 0;
bool pdg::PtrSplitIDLGen::runOnModule(Module &M)
{
  // PDG = &getAnalysis<pdg::ProgramDependencyGraph>();
  // accInfoTracker = &getAnalysis<pdg::AccessInfoTracker>();
  idl_file.open("ptrSplitIdl.txt");
  for (Function &F : M)
  {
    if (F.isDeclaration())
      continue;
    // print return type name
    DIType *retDIType = DIUtils::getFuncRetDIType(F);
    if (retDIType == nullptr)
      idl_file << "void ";
    else
      idl_file << DIUtils::getDITypeName(retDIType) << " ";

    idl_file << F.getName().str() << " (";
    for (auto &arg : F.args())
    {
      // idl_file << getTypeNameByTypeID
      DIType *dt = DIUtils::getArgDIType(arg);
      idl_file << DIUtils::getDITypeName(dt) << " ";
      idl_file << DIUtils::getArgName(arg);
      if (arg.getArgNo() != F.arg_size() - 1)
        idl_file << ", ";
    }

    if (F.arg_size() == 0)
    {
      idl_file << "void";
    }
    idl_file << ")\n";
  }

  idl_file.close();
  return false;
}

void pdg::PtrSplitIDLGen::getAnalysisUsage(AnalysisUsage &AU) const
{
  // AU.addRequired<pdg::ProgramDependencyGraph>();
  // AU.addRequired<pdg::AccessInfoTracker>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::PtrSplitIDLGen>
    PtrSplitIDL("ptr-gen", "IDL generation for ptr split", false, true);