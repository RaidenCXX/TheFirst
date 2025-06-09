#include "NavigationNode.h"
#include <utility>


NavigationNode::NavigationNode()
  :id(0), right(0), left(0), down(0), up(0),
    upRight(0), upLeft(0)  
{}

NavigationNode::NavigationNode(Vec2&& pos, int id, int right, int left,
                               int down,int up, int upRight, int upLeft)
  :pos(std::move(pos)), id(id), right(right), left(left), down(down),
    up(up), upRight(upRight), upLeft(upLeft)
{}
