#include "ProgramDependencyGraph.hpp"
#include "llvm/Analysis/DOTGraphTraitsPass.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/Support/TypeName.h"

namespace llvm
{
template <>
struct GraphTraits<pdg::DependencyNode<pdg::InstructionWrapper> *>
{
  using NodeRef = pdg::DependencyNode<pdg::InstructionWrapper> *;
  using ChildIteratorType = pdg::DependencyNode<InstructionWrapper>::iterator;
  using nodes_iterator = pdg::DependencyNode<InstructionWrapper>::iterator;

  static NodeRef getEntryNode(pdg::DependencyNode<pdg::InstructionWrapper> *N) { return N; }
  static inline ChildIteratorType child_begin(pdg::DependencyNode<InstructionWrapper> *N) { return ChildIteratorType(N->getDependencyList().begin()); }
  static inline ChildIteratorType child_end(pdg::DependencyNode<InstructionWrapper> *N) { return ChildIteratorType(N->getDependencyList().end()); }
  static nodes_iterator nodes_begin(pdg::DependencyNode<InstructionWrapper> *N) { return nodes_iterator(N->getDependencyList().begin()); }
  static nodes_iterator nodes_end(pdg::DependencyNode<InstructionWrapper> *N) { return nodes_iterator(N->getDependencyList().end()); }
};

template <>
struct GraphTraits<pdg::DependencyGraph<pdg::InstructionWrapper> *> : public GraphTraits<pdg::DependencyNode<pdg::InstructionWrapper> *>
{
  static NodeRef getEntryNode(pdg::DependencyGraph<InstructionWrapper> *N) { return *(N->getNodeSet().begin()); }
  using nodes_iterator = DependencyGraph<InstructionWrapper>::nodes_iterator;
  static nodes_iterator nodes_begin(pdg::DependencyGraph<InstructionWrapper> *N) { return nodes_iterator(N->getNodeSet().begin()); }
  static nodes_iterator nodes_end(pdg::DependencyGraph<InstructionWrapper> *N) { return nodes_iterator(N->getNodeSet().end()); }
};

// PDG
template <>
struct GraphTraits<pdg::ProgramDependencyGraph *> : public GraphTraits<pdg::DependencyGraph<InstructionWrapper> *>
{
  static NodeRef getEntryNode(pdg::ProgramDependencyGraph *PG)
  {
    return *(PG->_getPDG()->begin_child());
  }

  static nodes_iterator nodes_begin(pdg::ProgramDependencyGraph *PG)
  {
    return PG->_getPDG()->begin_child();
  }

  static nodes_iterator nodes_end(pdg::ProgramDependencyGraph *PG)
  {
    return PG->_getPDG()->end_child();
  }
};

template <>
struct DOTGraphTraits<pdg::DependencyNode<pdg::InstructionWrapper> *> : public DefaultDOTGraphTraits
{
  DOTGraphTraits(bool isSimple = false) : DefaultDOTGraphTraits(isSimple) {}

  std::string getNodeLabel(pdg::DependencyNode<pdg::InstructionWrapper> *Node, pdg::DependencyNode<pdg::InstructionWrapper> *Graph)
  {
    using namespace pdg;
    const InstructionWrapper *instW = Node->getData();
    if (instW == nullptr)
    {
      errs() << "instW " << instW << "\n";
      return "null instW";
    }
    

    std::string Str;
    raw_string_ostream OS(Str);

    switch (instW->getGraphNodeType())
    {
    case GraphNodeType::ENTRY:
    {
      return ("<<ENTRY>> " + instW->getFunction()->getName().str());
    }
    case GraphNodeType::GLOBAL_VALUE:
    {
      OS << *instW->getValue();
      return ("GLOBAL_VALUE:" + OS.str());
    }
    case GraphNodeType::FORMAL_IN:
    {
      llvm::Argument *arg = instW->getArgument();
      int arg_pos = arg->getArgNo();
      OS << *instW->getArgument()->getType();
      return ("FORMAL_IN: " + std::to_string(arg_pos) + " " + OS.str());
    }
    case GraphNodeType::ACTUAL_IN:
    {
      llvm::Argument *arg = instW->getArgument();
      int arg_pos = arg->getArgNo();
      OS << *instW->getArgument()->getType();
      return ("ACTUAL_IN: " + std::to_string(arg_pos) + " " + OS.str());
    }
    case GraphNodeType::FORMAL_OUT:
    {
      llvm::Argument *arg = instW->getArgument();
      int arg_pos = arg->getArgNo();
      OS << *instW->getArgument()->getType();
      return ("FORMAL_OUT: " + std::to_string(arg_pos) + " " + OS.str());
    }

    case GraphNodeType::ACTUAL_OUT:
    {
      llvm::Argument *arg = instW->getArgument();
      int arg_pos = arg->getArgNo();
      OS << *instW->getArgument()->getType();
      return ("ACTUAL_OUT: " + std::to_string(arg_pos) + " " + OS.str());
    }
    case GraphNodeType::PARAMETER_FIELD:
    {
      llvm::Argument *arg = instW->getArgument();
      int arg_pos = arg->getArgNo();
      OS << *instW->getTreeNodeType() << " arg_pos: " << arg_pos << " - "
         << "f_id: " << instW->getNodeOffset();
      return OS.str();
    }
    case GraphNodeType::POINTER_RW:
    {
      OS << *instW->getArgument()->getType();
      return ("POINTER READ/WRITE : *" + OS.str());
    }
      //for pointer node, add a "*" sign before real node content
      //if multi level pointer, use a loop instead here

    case GraphNodeType::STRUCT_FIELD:
    {
      llvm::Instruction *inst = instW->getInstruction();
      llvm::AllocaInst *allocaInst = dyn_cast<AllocaInst>(inst);
      // processing differently when get a struct pointer
      llvm::StringRef struct_name = "";
      if (allocaInst->getAllocatedType()->isPointerTy())
      {
        llvm::PointerType *pt = dyn_cast<llvm::PointerType>(allocaInst->getAllocatedType());
        struct_name = pt->getElementType()->getStructName();
      }
      else
      {
        struct_name = allocaInst->getAllocatedType()->getStructName();
      }

      std::string struct_string = struct_name.str();
      std::vector<std::string> TYPE_NAMES = {
          "VoidTy",      ///<  0: type with no size
          "HalfTy",      ///<  1: 16-bit floating point type
          "FloatTy",     ///<  2: 32-bit floating point type
          "DoubleTy",    ///<  3: 64-bit floating point type
          "X86_FP80Ty",  ///<  4: 80-bit floating point type (X87)
          "FP128Ty",     ///<  5: 128-bit floating point type (112-bit mantissa)
          "PPC_FP128Ty", ///<  6: 128-bit floating point type (two 64-bits, PowerPC)
          "LabelTy",     ///<  7: Labels
          "MetadataTy",  ///<  8: Metadata
          "X86_MMXTy",   ///<  9: MMX vectors (64 bits, X86 specific)
          "TokenTy",     ///< 10: Tokens

          // Derived types... see DerivedTypes.h file.
          // Make sure FirstDerivedTyID stays up to date!
          "IntegerTy",  ///< 11: Arbitrary bit width integers
          "FunctionTy", ///< 12: Functions
          "StructTy",   ///< 13: Structures
          "ArrayTy",    ///< 14: Arrays
          "PointerTy",  ///< 15: Pointers
          "VectorTy"};
      llvm::Type *field_type = instW->getTreeNodeType();
      std::string type_name = TYPE_NAMES.at(field_type->getTypeID());

      std::string ret_string = "";
      std::string field_pos = std::to_string(instW->getNodeOffset());
      ret_string = struct_string + " (" + type_name + ") : " + std::to_string(instW->getNodeOffset());
      return (ret_string);
    }
    default:
      break;
    }

    //llvm::Instruction *inst = Node->getData()->getInstruction();
    llvm::Instruction *inst = instW->getInstruction();

    if (inst == nullptr)
      return OS.str();
    if (isSimple() && !inst->getName().empty())
      return inst->getName().str();
    else
    {
      std::string Str;
      raw_string_ostream OS(Str);
      OS << *inst;
      return OS.str();
    }
  }
};

template <>
struct DOTGraphTraits<pdg::DependencyGraph<InstructionWrapper> *> : public DOTGraphTraits<pdg::DependencyNode<InstructionWrapper> *>
{
  DOTGraphTraits(bool isSimple = false) : DOTGraphTraits<pdg::DependencyNode<InstructionWrapper> *>(isSimple) {}

  std::string getNodeLabel(pdg::DependencyNode<InstructionWrapper> *Node, pdg::DependencyGraph<InstructionWrapper> *Graph)
  {
    return DOTGraphTraits<pdg::DependencyNode<InstructionWrapper> *>::getNodeLabel(Node, *(Graph->begin_child()));
  }
};

template <>
struct DOTGraphTraits<pdg::ProgramDependencyGraph *> : public DOTGraphTraits<pdg::DependencyGraph<InstructionWrapper> *>
{
  //using nodes_itr = std::vector<std::pair<pdg::DependencyNode<pdg::InstructionWrapper> *, pdg::DependencyType>>::iterator;
  DOTGraphTraits(bool isSimple = false) : DOTGraphTraits<pdg::DependencyGraph<pdg::InstructionWrapper> *>(isSimple) {}

  static std::string getGraphName(pdg::ProgramDependencyGraph *)
  {
    return "Program Dependency Graph";
  }

  std::string getNodeLabel(pdg::DependencyNode<InstructionWrapper> *Node, pdg::ProgramDependencyGraph *Graph)
  {
    return DOTGraphTraits<pdg::DependencyGraph<InstructionWrapper> *>::getNodeLabel(Node, Graph->_getPDG());
  }

  std::string
  getEdgeAttributes(pdg::DependencyNode<InstructionWrapper> *Node,
                    pdg::DependencyLinkIterator<pdg::InstructionWrapper> &IW,
                    pdg::ProgramDependencyGraph *PD)
  {
    using namespace pdg;
    switch (IW.getDependencyType())
    {
    case DependencyType::CONTROL:
      return "";
    case DependencyType::DATA_GENERAL:
      return "style=dotted, label = \"{data_g}\"";
    case DependencyType::GLOBAL_DEP:
      return "style=dotted";
    case DependencyType::PARAMETER:
      return "style=dashed, color=\"blue\"";
    case DependencyType::DATA_DEF_USE:
    {
      Instruction *pFromInst = Node->getData()->getInstruction();
      return "style=dotted,label = \"{DEF_USE}\" ";
    }
    case DependencyType::DATA_RAW:
    {
      Instruction *pInstruction = (*IW)->getData()->getInstruction();
      // pTo Node must be a LoadInst
      std::string ret_str;
      if (isa<LoadInst>(pInstruction))
      {
        LoadInst *LI = dyn_cast<LoadInst>(pInstruction);
        Value *valLI = LI->getPointerOperand();
        ret_str = "style=dotted,label = \"{RAW} " + valLI->getName().str() + "\"";
      }
      else if (isa<CallInst>(pInstruction))
      {
        ret_str = "style=dotted,label = \"{RAW}\"";
      }
      else
        errs() << "incorrect instruction for DATA_RAW node!"
               << "\n";
      return ret_str;
    }
    case DependencyType::DATA_CALL_PARA:
    {
      return "style=dotted, label=\"{CALL_PARA}\", color=\"yellow\", penwidth=\"2.0\"";
    }
    case DependencyType::VAL_DEP:
    {
      return "style=dotted, label=\"{S_FIELD}\", color=\"red\", penwidth=\"1.0\"";
    }
    case DependencyType::DATA_ALIAS:
      return "style=dotted, label=\"{alias}\", color=\"green\", penwidth=\"1.0\"";
    default:
      return "";
      //return "style=dotted,label=\"{UNDEFINED}\"";
    }
  }

  std::string getNodeAttributes(pdg::DependencyNode<InstructionWrapper> *Node, pdg::ProgramDependencyGraph *Graph)
  {
    using namespace pdg;

    auto instW = Node->getData();

    if (instW == nullptr)
    {
      //errs() << "instW " << instW << "\n";
      return "null instW";
    }

    switch (instW->getGraphNodeType())
    {
    case GraphNodeType::ENTRY:
      return "";
    case GraphNodeType::GLOBAL_VALUE:
      return "";
    case GraphNodeType::FORMAL_IN:
      return "color=\"blue\"";
    case GraphNodeType::ACTUAL_IN:
      return "color=\"blue\"";
    case GraphNodeType::FORMAL_OUT:
      return "color=\"blue\"";
    case GraphNodeType::ACTUAL_OUT:
      return "color=\"blue\"";
    case GraphNodeType::PARAMETER_FIELD:
      return "color=\"blue\"";
    case GraphNodeType::POINTER_RW:
      return "color=\"red\"";
    case GraphNodeType::STRUCT_FIELD:
      return "";
    default:
      return "";
    }
  }

  std::string getGraphProperties(pdg::ProgramDependencyGraph *Graph)
  {
    return "graph [ splines=true ]";
  }
};
} // namespace llvm

struct ProgramDependencyPrinter : public llvm::DOTGraphTraitsPrinter<pdg::ProgramDependencyGraph, false>
{
  static char ID;
  ProgramDependencyPrinter() : llvm::DOTGraphTraitsPrinter<pdg::ProgramDependencyGraph, false>("pdgragh", ID) {}
};

char ProgramDependencyPrinter::ID = 0;
static llvm::RegisterPass<ProgramDependencyPrinter>
    PDGPrinter("dot-pdg",
               "Print instruction-level program dependency graph of "
               "function to 'dot' file",
               false, false);