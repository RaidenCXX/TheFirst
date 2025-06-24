#pragma once

#include "Enums.h"
#include "Vec2.h"

class NavigationNode
{
public:
  static constexpr float  size = 16.f;
  Vec2                    pos;
  NNType                  nodeType;
  unsigned short          id;
  unsigned short          right;
  unsigned short          left;
  unsigned short          down;
  unsigned short          up;
  bool                    active = false;

  NavigationNode();
  NavigationNode(NNType nodeType, Vec2& pos, int id, int right = 0, int left = 0, int down = 0, int up = 0);
  NavigationNode(NavigationNode&& node);
  void operator=(NavigationNode&& node);
};
