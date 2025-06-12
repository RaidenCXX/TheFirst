#include "NavigationNode.h"
#include <utility>


NavigationNode::NavigationNode()
  :id(0), right(0), left(0), down(0), up(0)
{}

NavigationNode::NavigationNode(Vec2&& pos, int id, int right, int left, int down,int up)
  :pos(std::move(pos)), id(id), right(right), left(left), down(down)
{}

NavigationNode::NavigationNode(NavigationNode&& node)
  :pos(std::move(node.pos)), id(node.id), right(node.right), left(node.left)
{}
