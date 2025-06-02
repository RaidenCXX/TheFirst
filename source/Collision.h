#pragma once

#include "Entity.h"
#include "Vec2.h"
class Collision
{
public:

[[nodiscard]] static Vec2 getOverlap(Entity& entityFirst, Entity& entitySecond);
[[nodiscard]] static Vec2 getPreviousOverlap(Entity& entityA, Entity& entityB);
static void resolveCollision(Entity& entityA, Entity& entityB);
};
