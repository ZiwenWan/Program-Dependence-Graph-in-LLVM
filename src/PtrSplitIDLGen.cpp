#include "PtrSplitIDLGen.hpp"

using namespace llvm;

char pdg::PtrSplitIDLGen::ID = 0;

std::string getTypeNameByTypeID(Type* ty)
{
  std::vector<std::string> TYPE_NAMES = {
      "void",      ///<  0: type with no size
      "Half",      ///<  1: 16-bit floating point type
      "float",     ///<  2: 32-bit floating point type
      "double",    ///<  3: 64-bit floating point type
      "X86_FP80Ty",  ///<  4: 80-bit floating point type (X87)
      "FP128Ty",     ///<  5: 128-bit floating point type (112-bit mantissa)
      "PPC_FP128Ty", ///<  6: 128-bit floating point type (two 64-bits, PowerPC)
      "LabelTy",     ///<  7: Labels
      "MetadataTy",  ///<  8: Metadata
      "X86_MMXTy",   ///<  9: MMX vectors (64 bits, X86 specific)
      "TokenTy",     ///< 10: Tokens
      // Derived types... see DerivedTypes.h file.
      // Make sure FirstDerivedTyID stays up to date!
      "int",  ///< 11: Arbitrary bit width integers
      "function", ///< 12: Functions
      //"struct",   ///< 13: Structures
      "opaque", // use opaque for struct first
      "array",    ///< 14: Arrays
      "pointer",  ///< 15: Pointers
      "vector"};

  Type::TypeID tid = ty->getTypeID();
  if (ty->isPointerTy())
    return TYPE_NAMES[ty->getPointerElementType()->getTypeID()] + " *"; 

  return TYPE_NAMES[tid];
}


bool pdg::PtrSplitIDLGen::runOnModule(Module &M)
{
  acctracker = &getAnalysis<pdg::AccessInfoTracker>();
  idl_file.open("ptrSplitIdl.txt"); 

  for (Function &F : M)
  {
    if (F.isDeclaration())
      continue;
    idl_file << F.getName().str() << "\n";
    idl_file << getTypeNameByTypeID(F.getReturnType()) << " ( ";
    for (auto &arg : F.args()) {
      idl_file << getTypeNameByTypeID(arg.getType());
      if (arg.getArgNo() != F.arg_size()-1) {
        idl_file << ", ";
      }
    }
    idl_file << " )";

    idl_file << " ( ";
    for (auto &arg : F.args())
    {
      idl_file << acctracker->getArgAccessInfo(arg);
      if (arg.getArgNo() != F.arg_size()-1) {
        idl_file << ", ";
      }
    }
    idl_file << " ) \n";
  }

  idl_file.close();
  return false;
}

void pdg::PtrSplitIDLGen::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<pdg::AccessInfoTracker>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::PtrSplitIDLGen>
    PtrSplitIDL("ptr-gen", "IDL generation for ptr split", false, true);