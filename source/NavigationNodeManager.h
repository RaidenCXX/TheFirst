#pragma once
#include <vector>
#include "NavigationNode.h"

#include <vector>
class NavigationNodeManager
{
  std::vector<NavigationNode>  m_navigationNodes;
  unsigned short               m_nodesCount;

public:
  NavigationNodeManager();
  NavigationNode&              addNode(NavigationNode&& node);
  std::vector<NavigationNode>& getNodes();
};
