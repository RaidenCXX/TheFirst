#pragma once

#include "Entity.h"
#include "Vec2.h"
#include "NavigationNode.h"

class Collision
{
public:

[[nodiscard]] static Vec2 getOverlap(Entity& entityFirst, Entity& entitySecond);
[[nodiscard]] static Vec2 getOverlap(Entity& entityA, const NavigationNode& NNode);
[[nodiscard]] static bool getWeaponOverlap(Entity& entityA, Entity& entityB, const Vec2& weaponBBox);
[[nodiscard]] static Vec2 getPreviousOverlap(Entity& entityA, Entity& entityB);
[[nodiscard]] static Vec2 getPreviousOverlap(Entity& entity, NavigationNode& NNode);
static void resolveCollision(Entity& entityA, Entity& entityB);
static unsigned short NNodeCollision(Entity& entity, NavigationNode& NNode);
[[nodiscard]] static Vec2 lineIntersect(Vec2 A, Vec2 B, Vec2 C, Vec2 D);
};
