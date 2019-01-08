
using namespace pdg;

template <typename NodeT>
DependencyGraph<NodeT>::~DependencyGraph()
{
  nodeSet.clear();
  DataToNodeMap.clear();
}

template <typename NodeT>
void DependencyGraph<NodeT>::addNode(const NodeT *dNode)
{
  DataToNodeMap[dNode] = new DependencyNode<NodeT>(dNode);
  nodeSet.push_back(DataToNodeMap[dNode]);
}

template <typename NodeT>
DependencyNode<NodeT> *DependencyGraph<NodeT>::getNodeByData(const NodeT *data)
{
  auto it = DataToNodeMap.find(data);
  if (it == DataToNodeMap.end())
  {
    addNode(data);
  }

  return DataToNodeMap[data];
}

template <typename NodeT>
typename DependencyNode<NodeT>::DependencyLinkList DependencyGraph<NodeT>::getNodeDepList(const NodeT *data) 
{
  DependencyNode<NodeT>* dNode = getNodeByData(data);
  return dNode->getDependencyList();
}


template <typename NodeT>
void DependencyGraph<NodeT>::addDependency(const NodeT *from, const NodeT *to, DependencyType depType)
{
  DependencyNode<NodeT> *fromNode = getNodeByData(from);
  DependencyNode<NodeT> *toNode = getNodeByData(to);
  fromNode->addDependencyTo(toNode, depType);
}

template <typename NodeT>
bool DependencyGraph<NodeT>::isDepends(const NodeT *data1, const NodeT *data2)
{
  bool hasDepends = false;
  try
  {
    hasDepends = DataToNodeMap[data1]->isDepends(DataToNodeMap[data2]);
  }
  catch (std::exception &e)
  {
    llvm::errs() << e.what() << "\n";
  }
  return hasDepends;
}

template <typename NodeT>
DependencyType DependencyGraph<NodeT>::getDepType(const NodeT *data1, const NodeT *data2)
{
  try
  {
    DependencyType depType = DataToNodeMap[data1]->getDependencyType(DataToNodeMap[data2]);
    return depType;
  }
  catch (std::exception &e)
  {
    llvm::errs() << e.what() << "\n";
  }
}