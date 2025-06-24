#include "NavigationNode.h"
#include <utility>


NavigationNode::NavigationNode()
  :nodeType(NNType::None), id(0), right(0), left(0), down(0), up(0), active(false) 
{}

NavigationNode::NavigationNode(NNType nodeType, Vec2& pos, int id, int right, int left, int down,int up)
  :nodeType(nodeType), pos(std::move(pos)), id(id), right(right), left(left), down(down), active(true)
{}

NavigationNode::NavigationNode(NavigationNode&& node)
  :nodeType(node.nodeType), pos(std::move(node.pos)), id(node.id), 
    right(node.right),left(node.left), up(node.up), down(node.down), active(node.active)
{}

void NavigationNode::operator= (NavigationNode&& node)
{
  this->id    = node.id;
  this->left  = node.left;
  this->right = node.right;
  this->up    = node.up;
  this->down  = node.down;
  this->nodeType = node.nodeType;
  this->pos = node.pos;
  this->active = node.active;
}
