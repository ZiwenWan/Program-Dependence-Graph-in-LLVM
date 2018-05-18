// dependency graph is in functionwrapper.h
#include "ProgramDependencies.h"
#include "llvm/Analysis/DOTGraphTraitsPass.h"


namespace llvm {

    template <>
    struct DOTGraphTraits<DepGraphNode *> : public DefaultDOTGraphTraits {
        DOTGraphTraits (bool isSimple = false): DefaultDOTGraphTraits(isSimple) {}

        std::string getNodeLabel(DepGraphNode *Node, DepGraphNode *Graph){

            const InstructionWrapper *instW = Node->getData();

            //TODO: why nullptr for Node->getData()?
            if(instW == nullptr || instW == NULL){
                errs() <<"instW " << instW << "\n";
                return "null instW";
            }
//            if (instW->getInstruction()) {
//                instW->getInstruction()->print(errs());
//            }
//            errs() << "\n";

            std::string Str;
            raw_string_ostream OS(Str);

            switch(instW->getType()) {
                case ENTRY:
                    return ("<<ENTRY>> " + instW->getFunctionName());

                case GLOBAL_VALUE:{
                    OS << *instW->getValue();
                    return ("GLOBAL_VALUE:" + OS.str());
                }

                case FORMAL_IN:{
                    OS << *instW->getArgument()->getType();
                    return ("FORMAL_IN:" + OS.str());
                }

                case ACTUAL_IN:{
                    OS << *instW->getArgument()->getType();
                    return ("ACTUAL_IN:" + OS.str() );
                }
                case FORMAL_OUT:{
                    OS << *instW->getArgument()->getType();
                    return ("FORMAL_OUT:" + OS.str());
                }

                case ACTUAL_OUT:{
                    OS << *instW->getArgument()->getType();
                    return ("ACTUAL_OUT:" + OS.str());
                }

                case PARAMETER_FIELD:{
                    OS << instW->getFieldId() << " " << *instW->getFieldType();
                    return OS.str();
                }

                    //for pointer node, add a "*" sign before real node content
                    //if multi level pointer, use a loop instead here
                case POINTER_RW:{
                    OS << *instW->getArgument()->getType();
                    return ("POINTER READ/WRITE : *" + OS.str());
                }

                default: {
                    break;
                }
            }

            const Instruction *inst = Node->getData()->getInstruction();

            if (isSimple() && !inst->getName().empty()) {
                return inst->getName().str();
            } else {
                std::string Str;
                raw_string_ostream OS(Str);
                OS << *inst;
                return OS.str();}
        }
    };

    template <>
    struct DOTGraphTraits<DepGraph *>
            : public DOTGraphTraits<DepGraphNode *>
    {
        DOTGraphTraits (bool isSimple = false)
                : DOTGraphTraits<DepGraphNode *>(isSimple) {}

        std::string getNodeLabel(DepGraphNode *Node, DepGraph *Graph) {
            return DOTGraphTraits<DepGraphNode *>::getNodeLabel(
                    Node, *(Graph->begin_children()));
        }
    };

    // data dependency graph

    template <>
    struct DOTGraphTraits<DataDependencyGraph *>
            : public DOTGraphTraits<DepGraph *> {
        DOTGraphTraits(bool isSimple = false)
                : DOTGraphTraits<DepGraph *>(isSimple) {}

        static std::string getGraphName(DataDependencyGraph *) {
            return "Data dependency graph";
        }

        std::string getNodeLabel(DepGraphNode *Node,
                                 DataDependencyGraph *Graph) {
            return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->DDG);
        }
    };

    // control dependency graph
    template <>
    struct DOTGraphTraits<ControlDependencyGraph *>
            : public DOTGraphTraits<DepGraph *> {
        DOTGraphTraits(bool isSimple = false)
                : DOTGraphTraits<DepGraph *>(isSimple) {}

        static std::string getGraphName(ControlDependencyGraph *) {
            return "Instruction-Level Control dependency graph";
        }

        std::string getNodeLabel(DepGraphNode *Node,
                                 ControlDependencyGraph *Graph) {
            return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->CDG);
        }

        static std::string
        getEdgeSourceLabel(DepGraphNode *Node,
                           DependencyLinkIterator<InstructionWrapper> EI) {
            //    errs() << "getEdgeSourceLabel(): type = " <<
            //    EI.getDependencyType() << "\n";
            switch (EI.getDependencyType()) {

                default:
                    return "";
            }
        }
    };

    template <>
    struct DOTGraphTraits<ProgramDependencyGraph *>
            : public DOTGraphTraits<DepGraph *> {
        DOTGraphTraits(bool isSimple = false)
                : DOTGraphTraits<DepGraph *>(isSimple) {}

        static std::string getGraphName(ProgramDependencyGraph *) {
            return "Program Dependency Graph";
        }

        std::string getNodeLabel(DepGraphNode *Node,
                                 ProgramDependencyGraph *Graph) {
            return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->PDG);
        }

        // return IW.getDependencyType() == DATA ?
        //"style=dotted" : "";

        // take care of the probable display error here
        std::string
        getEdgeAttributes(DepGraphNode *Node,
                          DependencyLinkIterator<InstructionWrapper> &IW,
                          ProgramDependencyGraph *PD) {

            switch (IW.getDependencyType()) {
                case CONTROL:
                    return "";
                case DATA_GENERAL:
                    return "style=dotted";
                case GLOBAL_VALUE:
                    return "style=dotted";
                case PARAMETER:
                    return "style=dashed,label= \"{para}\" ";
                case DATA_DEF_USE: {
                    Instruction *pFromInst = Node->getData()->getInstruction();
                    return "style=dotted,label = \"{DEF_USE}\" ";
                }
                case DATA_RAW: {
                    Instruction *pInstruction = IW.getDependencyNode()->getInstruction();
                    // pTo Node must be a LoadInst
                    std::string ret_str;
                    if (isa<LoadInst>(pInstruction)) {
                        LoadInst *LI = dyn_cast<LoadInst>(pInstruction);
                        Value *valLI = LI->getPointerOperand();
                        ret_str =
                                "style=dotted,label = \"{RAW} " + valLI->getName().str() + "\"";
                    } else if (isa<CallInst>(pInstruction)) {
                        ret_str = "style=dotted,label = \"{RAW}\"";
                    } else
                        errs() << "incorrect instruction for DATA_RAW node!"
                               << "\n";
                    return ret_str;
                }
                default:
                    return "style=dotted,label=\"{UNDEFINED}\"";
            }          // end switch
            //return ""; // default ret statement
        }            // end getEdgeAttr...
    };

} // namespace llvm

struct ControlDependencyViewer
        : public DOTGraphTraitsViewer<ControlDependencyGraph, false> {
    static char ID;
    ControlDependencyViewer()
            : DOTGraphTraitsViewer<ControlDependencyGraph, false>("cdgraph", ID) {
    }
};

char ControlDependencyViewer::ID = 0;

static RegisterPass<ControlDependencyViewer>
        CDGViewer("view-cdg", "View control dependency graph of function",
                  false, false);

struct ControlDependencyPrinter
        : public DOTGraphTraitsPrinter<ControlDependencyGraph, false> {
    static char ID;
    ControlDependencyPrinter()
            : DOTGraphTraitsPrinter<ControlDependencyGraph, false>("cdgragh",
                                                                   ID) {}
};

char ControlDependencyPrinter::ID = 0;
static RegisterPass<ControlDependencyPrinter>
        CDGPrinter("dot-cdg",
                   "Print control dependency graph of function to 'dot' file",
                   false, false);

// DataPrinter
struct DataDependencyViewer
        : public DOTGraphTraitsViewer<DataDependencyGraph, false> {
    static char ID;
    DataDependencyViewer()
            : DOTGraphTraitsViewer<DataDependencyGraph, false>("ddgraph", ID) {}
};

char DataDependencyViewer::ID = 0;
static RegisterPass<DataDependencyViewer>
        DdgViewer("view-ddg", "View data dependency graph of function", false,
                  false);

struct DataDependencyPrinter
        : public DOTGraphTraitsPrinter<DataDependencyGraph, false> {
    static char ID;
    DataDependencyPrinter()
            : DOTGraphTraitsPrinter<DataDependencyGraph, false>("ddgragh", ID) {}
};

char DataDependencyPrinter::ID = 0;
static RegisterPass<DataDependencyPrinter>
        DdGPrinter("dot-ddg",
                   "Print data dependency graph of function to 'dot' file",
                   false, false);

// Program Printer
// struct ProgramDependencyViewer
//     : public DOTGraphTraitsViewer<ProgramDependencyGraph, false> {
//   static char ID;
//   ProgramDependencyViewer()
//       : DOTGraphTraitsViewer<ProgramDependencyGraph, false>("pdgraph",
//       ID) {
//   }
// };

// char ProgramDependencyViewer::ID = 0;
// static RegisterPass<ProgramDependencyViewer>
//     PdgViewer("view-pdg", "View program dependency graph of function",
//               false, false);

struct ProgramDependencyPrinter
        : public DOTGraphTraitsPrinter<ProgramDependencyGraph, false> {
    static char ID;
    ProgramDependencyPrinter()
            : DOTGraphTraitsPrinter<ProgramDependencyGraph, false>("pdgragh",
                                                                   ID) {}
};

char ProgramDependencyPrinter::ID = 0;
static RegisterPass<ProgramDependencyPrinter>
        PDGPrinter("dot-pdg",
                   "Print instruction-level program dependency graph of "
                           "function to 'dot' file",
                   false, false);
