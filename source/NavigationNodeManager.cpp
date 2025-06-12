#include "NavigationNodeManager.h"
#include "NavigationNode.h"

NavigationNodeManager::NavigationNodeManager()
  :m_nodesCount(1)
{
  m_navigationNodes.reserve(2000);
}


NavigationNode& NavigationNodeManager::addNode(NavigationNode&& node)
{
  node.id = m_nodesCount;
  m_nodesCount++;
  m_navigationNodes.emplace_back(std::move(node));
  return m_navigationNodes.back();
}

std::vector<NavigationNode>& NavigationNodeManager::getNodes()
{
  return m_navigationNodes;
}
