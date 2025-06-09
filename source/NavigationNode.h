#pragma once

#include "Vec2.h"

class NavigationNode
{
  Vec2            pos;
  unsigned short  id;
  unsigned short  right;
  unsigned short  left;
  unsigned short  down;
  unsigned short  up;
  unsigned short  upRight;
  unsigned short  upLeft;

  NavigationNode();
  NavigationNode(Vec2&& pos, int id, int right = 0, int left = 0, int down = 0,
                                  int up = 0, int upRight = 0, int upLeft = 0);
};
