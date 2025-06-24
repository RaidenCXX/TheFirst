#include "NavigationNodeManager.h"
#include "NavigationNode.h"

NavigationNodeManager::NavigationNodeManager()
  :m_nodesCount(1)
{
  m_navigationNodes.resize(2000);
}


NavigationNode& NavigationNodeManager::addNode(NavigationNode&& node)
{
  m_navigationNodes[node.id] = std::move(node);
  return m_navigationNodes.back();
}

std::vector<NavigationNode>& NavigationNodeManager::getNodes()
{
  return m_navigationNodes;
}
