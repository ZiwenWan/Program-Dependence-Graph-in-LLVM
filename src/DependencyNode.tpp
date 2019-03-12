using namespace pdg;

template <typename NodeT>
void DependencyNode<NodeT>::addDependencyTo(DependencyNode<NodeT> *pNode, DependencyType type)
{
  if (pNode == this)
    return;
  DependencyLink link = DependencyLink(pNode, type);
  if (std::find(dependencyList.begin(), dependencyList.end(), link) == dependencyList.end())
  {
    dependencyList.push_back(link);
  }
}

template <typename NodeT>
bool DependencyNode<NodeT>::isDepends(const DependencyNode<NodeT> *dNode)
{
  if (dNode == nullptr)
  {
    throw DependencyNodeIsNullptrException("Dependency Node is nullptr.");
  }
  for (auto link : dependencyList)
  {
    if (link.first == dNode)
    {
      return true;
    }
  }
  return false;
}

template <typename NodeT>
DependencyType DependencyNode<NodeT>::getDependencyType(const DependencyNode<NodeT> *dNode) const
{
  if (dNode == nullptr)
  {
    throw DependencyNodeIsNullptrException("Dependency Node is nullptr.");
  }

  for (auto link : dependencyList)
  {
    if (link.first == dNode)
    {
      return link.second;
    }
  }

  return DependencyType::NO_DEPENDENCY;
}

template <typename NodeT>
typename DependencyNode<NodeT>::DependencyLinkList DependencyNode<NodeT>::getNodesWithDepType(DependencyType depType) const
{
  DependencyLinkList ret;
  for (auto pair : dependencyList)
  {
    if (pair.second == depType)
      ret.push_back(pair);
  }
  return ret;
}