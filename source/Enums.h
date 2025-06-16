#pragma once

enum class Object
{
  None      = 0,
  Player,
  Mushroom,
  TileBbox,
  Tile,
  SlideRBbox,
  SlideLBbox,
  Bridge,
  Ladder,
  SmallPlatform,
  Background,
};


enum class Wepon
{

};

enum class EnemyState
{
  None    = 0,
  Patrol,
  Attack
};

enum class NNType //Navigation Node Type
{
  None = 0,
  Transitional,
  End
};
