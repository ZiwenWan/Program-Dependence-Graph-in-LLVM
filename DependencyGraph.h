#ifndef DEPENDENCYGRAPH_H_
#define DEPENDENCYGRAPH_H_
#define DEBUG_TYPE "pdg"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"

#include "llvm/ADT/GraphTraits.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include "tree.hh"
#include <iterator>
#include <map>
#include <set>

// static llvm::AliasAnalysis Global_AA_value; //shared by all classes, both

// DataDependenceGraph and FDA  static llvm::AliasAnalysis Global_AA;
using namespace llvm;

namespace pdg{

    enum DependencyType {
        CALL = 0,
        CONTROL,
        DATA_GENERAL,
        DATA_DEF_USE,
        DATA_RAW,
        PARAMETER,
        STRUCT_FIELDS
    };

    enum ControlType {
        TRUE = 0x4, FALSE = 0x8, OTHER = 0xc
    };

    enum InstWrapperType {
        INST = 0,
        FORMAL_IN,
        FORMAL_OUT,
        ACTUAL_IN,
        ACTUAL_OUT,
        RETURN,
        POINTER_RW,
        PARAMETER_FIELD,
        ENTRY,
        GLOBAL_VALUE,
        STRUCT_FIELD
    };

    class InstructionWrapper {
    private:
        int node_id; // intialized after the set "nodes" setted up
        llvm::Instruction *Inst;
        llvm::BasicBlock *BB;
        llvm::Function *Func;
        llvm::Value *value;
        llvm::Argument *arg;
        llvm::Type *field_type;
        llvm::Type *parent_type;
        int field_id;
        InstWrapperType type;
        bool flag;   // for separation algorithm coloring
        bool isVisited; // avoid multiple access by for iteration in PDG construction
        tree<InstructionWrapper *> containedNodes;

    public:
        InstructionWrapper(llvm::Instruction *Inst, llvm::Function *Func,
                           llvm::Value *value, InstWrapperType type) {

            this->value = value;
            this->type = type;
            this->Func = Func;
            this->BB = nullptr;
            this->arg = nullptr;
            this->Inst = Inst;
            this->field_id = -1;
            this->field_type = nullptr;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        // for function argument node(formal in/out)

        InstructionWrapper(llvm::Function *Func, llvm::Argument *arg,
                           llvm::Type *field_type, llvm::Type *parent_type, int field_id,
                           InstWrapperType type) {
            this->arg = arg;
            this->type = type;
            this->Func = Func;
            this->field_type = field_type;
            this->field_id = field_id;
            this->parent_type = parent_type;

            this->BB = nullptr;
            this->Inst = nullptr;
            // changed by shen
            this->value = arg;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Function *Func, llvm::Instruction *inst, llvm::Argument *arg,
                           llvm::Type *field_type, int field_id,
                           InstWrapperType type) {
            this->arg = arg;
            this->type = type;
            this->Func = Func;
            this->field_type = field_type;
            this->field_id = field_id;
            this->BB = nullptr;
            this->parent_type = parent_type;
            this->Inst = inst;
            // changed by shen
            this->value = arg;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Function *Func, llvm::Argument *arg,
                           InstWrapperType type) {
            this->arg = arg;
            this->type = type;
            this->Func = Func;

            this->BB = nullptr;
            this->Inst = nullptr;
            this->value = arg; // changed 12.14
            this->field_id = -1;
            this->field_type = nullptr;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Function *Func, InstWrapperType type) {
            this->Func = Func;
            this->type = type;
            this->BB = nullptr;
            this->Inst = nullptr;
            this->value = nullptr;
            this->field_type = nullptr;
            this->parent_type = nullptr;
            this->arg = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Instruction *Inst, llvm::BasicBlock *BB,
                           llvm::Function *Func) {
            this->Func = Func;
            this->BB = BB;
            this->Inst = Inst;
            this->type = INST;
            this->value = Inst; // changed from nullptr to Inst, 12.14
            this->arg = nullptr;
            this->field_id = -1;
            this->field_type = nullptr;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Instruction *Inst, llvm::Function *Func,
                           InstWrapperType type) {
            this->Func = Func;
            this->type = type;
            this->BB = nullptr;
            this->Inst = Inst;
            this->value = Inst; // changed from nullptr to Inst, 12.14
            this->arg = nullptr;
            this->field_id = -1;
            this->field_type = nullptr;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Instruction *Inst, llvm::Function *Func, int field_id,
                           InstWrapperType type, Type *field_type) {
            this->Func = Func;
            this->type = type;
            this->BB = nullptr;
            this->Inst = Inst;
            this->value = Inst; // changed from nullptr to Inst, 12.14
            this->arg = nullptr;
            this->field_id = field_id;
            this->field_type = field_type;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }

        InstructionWrapper(llvm::Function *Func, int field_id,
                           InstWrapperType type, Type *field_type) {
            this->Func = Func;
            this->type = type;
            this->BB = nullptr;
            this->Inst = Inst;
            this->value = Inst; // changed from nullptr to Inst, 12.14
            this->arg = nullptr;
            this->field_id = field_id;
            this->field_type = field_type;
            this->parent_type = nullptr;
            this->flag = false;
            this->isVisited = false;
        }



        std::string getFunctionName() const { return (this->Func->getName()).str(); }

        llvm::BasicBlock *getBasicBlock() const { return BB; }

        llvm::Instruction *getInstruction() const { return Inst; }

        llvm::Function *getFunction() const { return Func; }

        llvm::Value *getValue() const { return value; }

        llvm::Argument *getArgument() const { return arg; }

        llvm::Type *getFieldType() const { return field_type; }

        llvm::Type *getParentType() const { return parent_type; }

        int getFieldId() const { return field_id; }

        void setNodeId(const int id) { node_id = id; }

        int getNodeId() const { return node_id; }

        int getType() const { return type; }

        bool getFlag() const { return flag; }

        void setFlag(const bool _flag) { flag = _flag; }

        bool getVisited() const { return isVisited; }

        void setVisited(const bool _isVisited) { isVisited = _isVisited; }
    };


    template<class NodeT>
    class DependencyLinkIterator;

    template<class NodeT>
    class DependencyNode {
    public:
        //    typedef std::pair<DependencyNode<NodeT> *, DependencyType>
        //    DependencyLink;

        typedef std::pair<DependencyNode<NodeT> *, int> DependencyLink;
        typedef std::vector<DependencyLink> DependencyLinkList;
        typedef DependencyLinkIterator<NodeT> iterator;
        typedef DependencyLinkIterator<NodeT> const_iterator;

        iterator begin() {
            return DependencyLinkIterator<NodeT>(mDependencies.begin());
        }

        iterator end() { return DependencyLinkIterator<NodeT>(mDependencies.end()); }

        const_iterator begin() const {
            return DependencyLinkIterator<NodeT>(mDependencies.begin());
        }

        const_iterator end() const {
            return DependencyLinkIterator<NodeT>(mDependencies.end());
        }

        DependencyNode(const NodeT *pData) : mpData(pData) {}

        void addDependencyTo(DependencyNode<NodeT> *pNode, int type) {
            // Avoid self-loops.
            if (pNode == this)
                return;

            DependencyLink link = DependencyLink(pNode, type);

            // Avoid double links.
            if (std::find(mDependencies.begin(), mDependencies.end(), link) ==
                mDependencies.end()) {
                mDependencies.push_back(link);
            }
        }

        const NodeT *getData() const { return mpData; }

        const DependencyLinkList &getDependencyList() const { return mDependencies; }

        bool dependsFrom(const DependencyNode<NodeT> *pNode) const {
            if (pNode == nullptr)
                return false;

            for (typename DependencyLinkList::const_iterator it = mDependencies.begin();
                 it != mDependencies.end(); ++it) {
                if (it->first == pNode)
                    return true;
            }
            return false;
        }

        int getDataDependencyType(const DependencyNode<NodeT> *pNode) const {
            for (typename DependencyLinkList::const_iterator it = mDependencies.begin();
                 it != mDependencies.end(); ++it) {
                if (it->first == pNode) {
                    return it->second;
                }
            }
            return DATA_GENERAL;
        }

        int getControlType(const DependencyNode<NodeT> *pNode) const {
            for (typename DependencyLinkList::const_iterator it = mDependencies.begin();
                 it != mDependencies.end(); ++it) {
                if (it->first == pNode) {
                    return it->second;
                }
            }
            return -1;
        }

        void printDepdendencies() {
            for (typename DependencyLinkList::const_iterator it = mDependencies.begin(); it != mDependencies.end(); ++it) {
               errs() << "Dependency Type:" << it->second << "\n";
            }
        }

    private:
        const NodeT *mpData;
        DependencyLinkList mDependencies;
    };

//  typedef DependencyNode<BasicBlockWrapper> DepGraphNode;
    using DepGraphNode = DependencyNode<InstructionWrapper>;

    template<class NodeT>
    class DependencyLinkIterator
            : public std::iterator<std::input_iterator_tag, NodeT> {
        typedef typename DependencyNode<NodeT>::DependencyLinkList::const_iterator
                InnerIterator;

    public:
        DependencyLinkIterator() : itr() {}

        DependencyLinkIterator(const InnerIterator &r) : itr(r) {}

        DependencyLinkIterator<NodeT> &operator++() {
            ++itr;
            return *this;
        }

        DependencyLinkIterator<NodeT> operator++(int) {
            DependencyLinkIterator<NodeT> old = *this;
            ++itr;
            return old;
        }

        DependencyNode<NodeT> *operator->() {
            typename DependencyNode<NodeT>::DependencyLink L = *itr;
            return L.first;
        }

        DependencyNode<NodeT> *operator*() {
            typename DependencyNode<NodeT>::DependencyLink L = *itr;
            return L.first;
        }

        bool operator!=(const DependencyLinkIterator &r) const {
            return itr != r.itr;
        }

        bool operator==(const DependencyLinkIterator &r) const {
            return !(operator!=(r));
        }

        int getDependencyType() const {
            typename DependencyNode<NodeT>::DependencyLink L = *itr;
            return L.second;
        }

        const NodeT *getDependencyNode() const {
            typename DependencyNode<NodeT>::DependencyLink L = *itr;
            return L.first->getData();
        }

    private:
        InnerIterator itr;
    };

/////////////////////////////////////////////////////////////////////////////

    template<class NodeT>
    class DependencyGraph {
    public:
        typedef typename std::vector<DependencyNode<NodeT> *>::iterator nodes_iterator;
        typedef typename std::vector<DependencyNode<NodeT> *>::const_iterator
                const_nodes_iterator;

        DependencyNode<NodeT> *getRootNode() const { return RootNode; }

        std::vector<DependencyNode<NodeT> *> getNodeSet() { return mNodes; }

        DependencyNode<NodeT> *getNodeByData(const NodeT *pData) {
            typename DataToNodeMap::iterator it = mDataToNode.find(pData);
            if (it == mDataToNode.end()) {
                it = mDataToNode.insert(it, typename DataToNodeMap::value_type(
                        pData, new DependencyNode<NodeT>(pData)));
                mNodes.push_back(it->second);
                if (!RootNode) {
                    RootNode = it->second;
                }
            }
            return it->second;
        }

        // const DependencyNode<NodeT> *getNodeByData(const NodeT *pData) const {
        //   typename DataToNodeMap::const_iterator it = mDataToNode.find(pData);
        //   if (it == mDataToNode.end()) {
        //     return 0;
        //   }
        //   return it->second;
        // }

        void addDependency(const NodeT *from, const NodeT *to, int type) {
            DependencyNode<NodeT> *pFrom = getNodeByData(from);
            DependencyNode<NodeT> *pTo = getNodeByData(to);
            pFrom->addDependencyTo(pTo, type);
        }

        bool depends(const NodeT *pNode1, const NodeT *pNode2) {
            DependencyNode<NodeT> *pFrom = getNodeByData(pNode1);
            DependencyNode<NodeT> *pTo = getNodeByData(pNode2);

            //      if(pFrom == nullptr)
            //	llvm::errs() << "pFrom == nullptr!\n";
            if (pFrom != nullptr && pTo != nullptr) {
                return pFrom->dependsFrom(pTo);
            } else
                return false;
        }

        int getControlType(const NodeT *pNode1, const NodeT *pNode2) const {
            const DependencyNode<NodeT> *pFrom = getNodeByData(pNode1);
            const DependencyNode<NodeT> *pTo = getNodeByData(pNode2);
            return pFrom->getControlType(pTo);
        }

        int getDataType(NodeT *pNode1, NodeT *pNode2) {
            DependencyNode<NodeT> *pFrom = getNodeByData(pNode1);
            DependencyNode<NodeT> *pTo = getNodeByData(pNode2);
            return pFrom->getDataDependencyType(pTo);
        }

        nodes_iterator begin_children() { return nodes_iterator(mNodes.begin()); }

        nodes_iterator end_children() { return nodes_iterator(mNodes.end()); }

        const_nodes_iterator begin_children() const {
            return const_nodes_iterator(mNodes.begin());
        }

        const_nodes_iterator end_children() const {
            return const_nodes_iterator(mNodes.end());
        }

        void printDependencies(NodeT *pNode) {
            DependencyNode<NodeT> *data = this->getNodeByData(pNode);
            data->printDepdendencies();
        }

        //    void print(llvm::raw_ostream &OS, const char *PN) const
        void print(llvm::raw_ostream &OS, const char *PN) const {
            OS << "=============================--------------------------------\n";
            OS << PN << ": \n";
            const_nodes_iterator firstNode = begin_children();
            if (firstNode != end_children())
                PrintDependencyTree(OS, this);
        }


        /// Zhiyuan added
        ~DependencyGraph() {
            mNodes.clear();
            mDataToNode.clear();
        }

    private:
        typedef std::vector<DependencyNode<NodeT> *> NodeSet;
        typedef std::map<const NodeT *, DependencyNode<NodeT> *> DataToNodeMap;
        DependencyNode<NodeT> *RootNode;
        NodeSet mNodes;
        DataToNodeMap mDataToNode;
    };

//  typedef DependencyGraph<BasicBlockWrapper> DepGraph;
    typedef DependencyGraph<InstructionWrapper> DepGraph;

/*!
 * Overloaded operator that pretty print a DependencyNode
 */
    template<class NodeT>
    static llvm::raw_ostream &operator<<(llvm::raw_ostream &o,
                                         const DependencyNode<NodeT> *N) {
        const NodeT *block = N->getData();
        if (block) {
            //      WriteAsOperand(o, block, false);
        } else {
            o << "<<EntryNode>>";
        }
        o << " { ";
        typename DependencyNode<NodeT>::const_iterator I = N->begin();
        typename DependencyNode<NodeT>::const_iterator E = N->end();
        for (; I != E; ++I) {
            //      WriteAsOperand(o, (*I)->getData(), false);
            o << ":" << I.getDependencyType() << " ";
        }
        o << "}";
        return o << "\n";
    }

/*!
 * Print function.
 */
    template<class NodeT>
    static void PrintDependencyTree(llvm::raw_ostream &o,
                                    const DependencyGraph<NodeT> *G) {
        for (typename DependencyGraph<NodeT>::const_nodes_iterator
                     I = G->begin_children(),
                     E = G->end_children();
             I != E; ++I) {
            o.indent(4);
            o << *I;
        }
    }
}

namespace pdg {
    extern std::set<InstructionWrapper *> instnodes;
    extern std::set<InstructionWrapper *> globalList;
    extern std::map<const llvm::Instruction *, InstructionWrapper *> instMap;
    extern std::map<const llvm::Function *, std::set<InstructionWrapper *>> funcInstWList;
}

namespace llvm {
    template <> struct GraphTraits<pdg::DepGraphNode *> {
        // typedef DepGraphNode NodeRef;
        using NodeRef = pdg::DepGraphNode *;
        using ChildIteratorType = pdg::DepGraphNode::iterator;
        // typedef NodeRef::iterator  ChildIteratorType;

        static NodeRef getEntryNode(pdg::DepGraphNode *N) { return N; }
        static inline ChildIteratorType child_begin(pdg::DepGraphNode *N) {
            return N->begin();
        }
        static inline ChildIteratorType child_end(pdg::DepGraphNode *N) {
            return N->end();
        }

        using nodes_iterator = pdg::DepGraphNode::iterator;

        static nodes_iterator nodes_begin(pdg::DepGraphNode *N) { return N->begin(); }

        static nodes_iterator nodes_end(pdg::DepGraphNode *N) { return N->end(); }
    };

    template <>
    struct GraphTraits<pdg::DepGraph *> : public GraphTraits<pdg::DepGraphNode *> {

        static NodeRef getEntryNode(pdg::DepGraph *N) { return *(N->begin_children()); }

        typedef pdg::DepGraph::const_nodes_iterator nodes_iterator;

        static nodes_iterator nodes_begin(pdg::DepGraph *N) { return N->begin_children(); }

        static nodes_iterator nodes_end(pdg::DepGraph *N) { return N->end_children(); }
    };

} // namespace llvm

#endif // DEPENDENCYGRAPH_H_
