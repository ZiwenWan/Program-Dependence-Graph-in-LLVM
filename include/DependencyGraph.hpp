#ifndef DEPENDENCYGRAPH_H_
#define DEPENDENCYGRAPH_H_
#include "DependencyNode.hpp"
#include "PDGUtils.hpp"
#include <map>
#include <set>
#include <exception>

namespace pdg
{
template <typename NodeT>
class DependencyGraph
{
public:
  using nodes_iterator = typename std::vector<DependencyNode<InstructionWrapper> *>::iterator;
  using const_nodes_iterator = typename std::vector<DependencyNode<InstructionWrapper> *>::const_iterator;
  DependencyGraph() = default;
  ~DependencyGraph();
  std::vector<DependencyNode<NodeT> *> getNodeSet() const { return nodeSet; }
  void addNode(const NodeT *dNode);
  DependencyNode<NodeT> *getNodeByData(const NodeT *data);
  typename DependencyNode<NodeT>::DependencyLinkList getNodeDepList(const NodeT *data);
  void addDependency(const NodeT *from, const NodeT *to, DependencyType depType);
  bool isDepends(const NodeT *data1, const NodeT *data2);
  DependencyType getDepType(const NodeT *data1, const NodeT *data2);
  nodes_iterator begin_child() { return nodes_iterator(nodeSet.begin()); }
  nodes_iterator end_child() { return nodes_iterator(nodeSet.end()); }

private:
  std::vector<DependencyNode<NodeT> *> nodeSet;
  std::map<const NodeT *, DependencyNode<NodeT> *> DataToNodeMap;
};
} // namespace pdg

#include "DependencyGraph.tpp"
#endif