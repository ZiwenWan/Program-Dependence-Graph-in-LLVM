#include "PtrSplitIDLGen.hpp"
using namespace llvm;

char pdg::PtrSplitIDLGen::ID = 0;
bool pdg::PtrSplitIDLGen::runOnModule(Module &M)
{
  // PDG = &getAnalysis<pdg::ProgramDependencyGraph>();
  std::vector<std::string> accStrs = {
    "N", "R", "W", "T"
  };
  auto &pdgUtils = PDGUtils::getInstance();
  // accInfoTracker = &getAnalysis<pdg::AccessInfoTracker>();
  idl_file.open("ptrSplitIdl.txt");
  for (Function &F : M)
  {
    if (F.isDeclaration())
      continue;
    // print return type name
    errs() << "Generatring for " << F.getName() << "\n";
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

    // auto funcMap = pdgUtils.getFuncMap()[&F];
    // idl_file << "( ";
    // for (ArgumentWrapper *argW : funcMap->getArgWList())
    // {
    //   AccessType accType = (*argW->tree_begin(TreeType::FORMAL_IN_TREE))->getAccessType();
    //   int accTypeNum = static_cast<int>(accType);
    //   if (isOnlyUsedinProcessCall(argW))
    //   {
    //     accTypeNum = 3;
    //     errs() << "Only used in pc.."
    //            << "\n";
    //   }

    //   idl_file << accStrs[accTypeNum];
    //   if (argW->getArg()->getArgNo() != F.arg_size() - 1)
    //     idl_file << ", ";
    // }
    // if (F.arg_size() == 0)
    //   idl_file << "void";

    // idl_file << " )\n";
  }

  idl_file.close();
  return false;
}

// bool pdg::PtrSplitIDLGen::isOnlyUsedinProcessCall(ArgumentWrapper *argW)
// {
//   std::set<InstructionWrapper *> argAliasLoadInsts = accInfoTracker->getAliasLoadInstsForArg(argW);
//   for (InstructionWrapper* instW : argAliasLoadInsts)
//   {
//     auto dataDList = accInfoTracker->_getPDG()->getNodeDepList(instW->getInstruction());
//     for (auto dependencyPair : dataDList)
//     {
//       DependencyType depType = dependencyPair.second;
//       InstructionWrapper *DNodeW2 = const_cast<InstructionWrapper *>(dependencyPair.first->getData());
//       if (depType == DependencyType::DATA_READ)
//         return false;
//       if (depType == DependencyType::DATA_DEF_USE)
//       {
//         if (StoreInst *st = dyn_cast<StoreInst>(DNodeW2->getInstruction()))
//         {
//           if (st->getPointerOperand() == instW->getInstruction())
//             return false;
//         }
//       }
//     }
//   }

//   return true;
// }

void pdg::PtrSplitIDLGen::getAnalysisUsage(AnalysisUsage &AU) const
{
  // AU.addRequired<pdg::ProgramDependencyGraph>();
  // AU.addRequired<pdg::AccessInfoTracker>();
  AU.setPreservesAll();
}

static RegisterPass<pdg::PtrSplitIDLGen>
    PtrSplitIDL("ptr-gen", "IDL generation for ptr split", false, true);