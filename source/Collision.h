#pragma once

#include "Entity.h"
#include "Vec2.h"
#include "NavigationNode.h"

class Collision
{
public:

[[nodiscard]] static Vec2 getOverlap(Entity& entityFirst, Entity& entitySecond);
[[nodiscard]] static Vec2 getOverlap(Entity& entityA, NavigationNode& NNode);
[[nodiscard]] static Vec2 getPreviousOverlap(Entity& entityA, Entity& entityB);
[[nodiscard]] static Vec2 getPreviousOverlap(Entity& entity, NavigationNode& NNode);
static void resolveCollision(Entity& entityA, Entity& entityB);
static bool NNodeCollision(Entity& entity, NavigationNode& NNode);
};
