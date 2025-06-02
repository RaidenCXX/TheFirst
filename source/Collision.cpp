#include "Collision.h"
#include "EntityMemoryPool.h"
#include "Enums.h"
#include "Vec2.h"
#include <cmath>


[[nodiscard]] Vec2 Collision::getOverlap(Entity& entityA, Entity& entityB)
{
  
  CBoundingBox& bboxA       = entityA.getComponent<CBoundingBox>();
  CTransform&   transformA  = entityA.getComponent<CTransform>();

  CBoundingBox& bboxB       = entityB.getComponent<CBoundingBox>();
  CTransform&   transformB  = entityB.getComponent<CTransform>();

  float oX = (bboxA.halfSize.x + bboxB.halfSize.x) - std::fabsf((transformA.pos.x + bboxA.offset.x) - (transformB.pos.x + bboxB.offset.x));
  float oY = (bboxA.halfSize.y + bboxB.halfSize.y) - std::fabsf((transformA.pos.y + bboxA.offset.y) - (transformB.pos.y + bboxB.offset.y));

  Vec2 overlap {0, 0}; 

  if (oX> 0 && oY > 0)
  {
    overlap.x = oX;
    overlap.y = oY;
  }
  
  return overlap;
}
 
bool calculatePointOnLine(Vec2& playerCornerPoint, Vec2& entityPointA, Vec2& entityPointB)
{
  float slope = (entityPointA.y - entityPointB.y) / (entityPointA.x - entityPointB.x);
  float b = entityPointA.y - slope * entityPointA.x;

  float yLine = slope * playerCornerPoint.x + b;

  return playerCornerPoint.y < yLine;
}


void Collision::resolveCollision(Entity& entityA, Entity& entityB)
{
  
  Vec2 overlap = getOverlap(entityA, entityB);
  if (overlap.x <= 0 || overlap.y <= 0)
    return;

  CTransform& transformA = entityA.getComponent<CTransform>();
  CTransform& transformB = entityB.getComponent<CTransform>();

  CBoundingBox& bboxA    = entityA.getComponent<CBoundingBox>();
  CBoundingBox& bboxB    = entityB.getComponent<CBoundingBox>();
  
  CTag& tagA = entityA.getComponent<CTag>();
  CTag& tagB = entityB.getComponent<CTag>();
 
  if(tagA.tag == Object::Player && 
    (tagB.tag == Object::TileBbox ||
    tagB.tag == Object::Bridge ||
    tagB.tag == Object::SmallPlatform))
  {
    if (overlap.x < overlap.y)
    {
      if (transformA.pos.x < transformB.pos.x)
      {
        transformA.pos.x -= overlap.x;
      }
      else
      {
        transformA.pos.x += overlap.x;
      }
      transformA.vel.x = 0;
    }
    else
    {
      if (transformA.pos.y < transformB.pos.y)
      {
        // Down up 
        transformA.pos.y -= overlap.y;
        transformA.vel.y = 0;

        if (entityA.hasComponent<CGravity>())
          entityA.getComponent<CGravity>().acceleration = 0.0f;

        if (entityA.hasComponent<CState>())
          entityA.getComponent<CState>().onGround = true;
      }
      else
      {
        // Up down
        transformA.pos.y += overlap.y;
        transformA.vel.y = 0;
      }
    }
  }
  else if(tagA.tag == Object::Player && 
    (tagB.tag == Object::SlideRBbox ||
    tagB.tag == Object::SlideLBbox))
  {
    //std::cerr << "Tag A: "<< static_cast<int>(tagA.tag) << "Tag B:" << static_cast<int>(tagB.tag) << std::endl;
    if (overlap.x < overlap.y)
    {
      //Left to right
      if (transformA.pos.x < transformB.pos.x)
      {
        transformA.pos.x -= overlap.x;
      }
      //Right to left
      else
      {
        transformA.pos.x += overlap.x;
      }
      transformA.vel.x = 0;
    }
    else
    {
      if (transformA.pos.y < transformB.pos.y)
      {
        // Down up 
        transformA.vel.y -= overlap.y;
        transformA.vel.y = 0;

        if (entityA.hasComponent<CGravity>())
          entityA.getComponent<CGravity>().acceleration = 0.0f;

        if (entityA.hasComponent<CState>())
          entityA.getComponent<CState>().onGround = true;
      }
      else
      {
        // Up down
        if(tagB.tag == Object::SlideRBbox)
        {
          Vec2 playerLeftDownPoint = transformA.pos + bboxA.size.y;
          Vec2 pointA = transformB.pos;
          Vec2 pointB = transformB.pos + bboxB.size;
          /*Is the lower left corner of the player above the
          perpendicular line which is located at 45 degrees?
          If true left corner of player is above! */ 
          bool state = calculatePointOnLine(playerLeftDownPoint, pointA, pointB);
          if(state)
          {

          }
          else
          {
            transformA.pos = transformA.prevPos;
            transformA.vel = transformB.dir;
          }
          
        }
        else if (tagB.tag == Object::SlideLBbox)
        {
        
        }
      }
    }
  }
  if (tagA.tag == Object::Player &&
    tagB.tag == Object::Ladder)
  {
    //std::cout << "Ladder collision" << std::endl;

    if (overlap.x < overlap.y)
    { 
      if(transformA.pos.x < transformB.pos.x)
        entityA.getComponent<CState>().canClimb = true;
      else
        entityA.getComponent<CState>().canClimb = true;
    }
    else if (overlap.x > overlap.y)
    {
      if (transformA.pos.y < transformB.pos.y)
      {
        // Down up 
        entityA.getComponent<CState>().canClimb = true;
        
      }
      else
      {
        // Up down
        entityA.getComponent<CState>().canClimb = true;
      }
    }
  }
}
