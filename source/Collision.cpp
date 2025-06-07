#include "Collision.h"
#include "Entity.h"
#include "EntityMemoryPool.h"
#include "Enums.h"
#include "Vec2.h"
#include <cmath>


[[nodiscard]] Vec2 Collision::getOverlap(Entity& entityA, Entity& entityB)
{
  constexpr float epsilon = 0.001f;

  CBoundingBox& bboxA       = entityA.getComponent<CBoundingBox>();
  CTransform&   transformA  = entityA.getComponent<CTransform>();

  CBoundingBox& bboxB       = entityB.getComponent<CBoundingBox>();
  CTransform&   transformB  = entityB.getComponent<CTransform>();

  float oX = (bboxA.halfSize.x + bboxB.halfSize.x) - std::fabsf(transformA.pos.x - (transformB.pos.x + bboxB.offset.x));
  float oY = (bboxA.halfSize.y + bboxB.halfSize.y) - std::fabsf(transformA.pos.y - (transformB.pos.y + bboxB.offset.y));

  Vec2 overlap{0, 0};

  if(oX > epsilon && oY > epsilon)
  {
    overlap.x = oX;
    overlap.y = oY; 
  }

  return overlap;
}


Vec2 Collision::getPreviousOverlap(Entity& entityA, Entity& entityB)
{   
  CBoundingBox& bboxA       = entityA.getComponent<CBoundingBox>();
  CTransform&   transformA  = entityA.getComponent<CTransform>();

  CBoundingBox& bboxB       = entityB.getComponent<CBoundingBox>();
  CTransform&   transformB  = entityB.getComponent<CTransform>();

  float oX = (bboxA.halfSize.x + bboxB.halfSize.x) - 
    std::fabsf(transformA.prevPos.x - (transformB.prevPos.x + bboxB.offset.x ));
  float oY = (bboxA.halfSize.y + bboxB.halfSize.y) - 
    std::fabsf(transformA.prevPos.y - (transformB.prevPos.y + bboxB.offset.y ));
  
  Vec2 prevOverlap{0, 0};
  prevOverlap.y = oY > 0 ? oY : 0.0f;
  prevOverlap.x = oX > 0 ? oX : 0.0f;
  
  return prevOverlap;
}
 
bool isPointUnderLine(const Vec2& playerCornerPoint,const  Vec2& entityPointA,const Vec2& entityPointB)
{
  constexpr float epsilon = 0.0001f;

  if(std::abs(entityPointA.x - entityPointB.x) < epsilon)
  {
    return playerCornerPoint.x < entityPointA.x;
  }

  float slope = (entityPointA.y - entityPointB.y) / (entityPointA.x - entityPointB.x);
  float b = entityPointA.y - slope * entityPointA.x;

  float yLine = slope * playerCornerPoint.x + b;

  return playerCornerPoint.y > yLine;
}

void slide(Entity& entityA, Entity& entityB, Vec2& linesIntersection)
{
  CTransform& trPlayer  = entityA.getComponent<CTransform>();
  CState& sPlayer       = entityA.getComponent<CState>();
  CBoundingBox& bPlayer = entityA.getComponent<CBoundingBox>();

  CTransform& trObject   = entityB.getComponent<CTransform>();
  
  trPlayer.pos = linesIntersection;
  
  trPlayer.pos += trObject.dir;

  sPlayer.onGround = false;
  sPlayer.slide = true;
}

Vec2 linesIntersection(const Vec2& pA1, const Vec2& pA2, const Vec2& pB1, const Vec2& pB2)
{
  float denominator = (pA1.x - pA2.x) * (pB1.y - pB2.y) - (pA1.y - pA2.y) * (pB1.x - pB2.x);

  if (denominator == 0.0f)
      return Vec2{0.0f, 0.0f}; 

  float xNum = (pA1.x * pA2.y - pA1.y * pA2.x) * (pB1.x - pB2.x)
             - (pA1.x - pA2.x) * (pB1.x * pB2.y - pB1.y * pB2.x);

  float yNum = (pA1.x * pA2.y - pA1.y * pA2.x) * (pB1.y - pB2.y)
             - (pA1.y - pA2.y) * (pB1.x * pB2.y - pB1.y * pB2.x);

  float x = xNum / denominator;
  float y = yNum / denominator;

  return Vec2{x, y};
}

void Collision::resolveCollision(Entity& entityA, Entity& entityB)
{
  Vec2 overlap = getOverlap(entityA, entityB);
  if (overlap.x == 0 || overlap.y == 0)
    return;

  CTransform& transformA = entityA.getComponent<CTransform>();
  CTransform& transformB = entityB.getComponent<CTransform>();

  CTag& tagA = entityA.getComponent<CTag>();
  CTag& tagB = entityB.getComponent<CTag>();

  CBoundingBox& bboxA = entityA.getComponent<CBoundingBox>();
  CBoundingBox& bboxB = entityB.getComponent<CBoundingBox>();


  if (tagA.tag == Object::Player &&
      (tagB.tag == Object::TileBbox || tagB.tag == Object::Bridge || tagB.tag == Object::SmallPlatform))
  {
    
    Vec2 prevOverlap = getPreviousOverlap(entityA, entityB);
    
    if (prevOverlap.x > 0) // Vertical collision |
    {
  
      if (transformA.prevPos.y < transformB.prevPos.y)
      {
        // Down → Up
        transformA.pos.y -= overlap.y;
        transformA.vel.y  = 0;
        if (entityA.hasComponent<CGravity>())
            entityA.getComponent<CGravity>().acceleration = 0.0f;
        if (entityA.hasComponent<CState>())
            entityA.getComponent<CState>().onGround = true;
      }
      else if(transformA.prevPos.y > transformB.prevPos.y) 
      {
        // Up → Down
        transformA.pos.y += overlap.y;
        transformA.vel.y  = 0;
      }
    }
    if(prevOverlap.y > 0) // Horizontal collision <->
    {
      if (transformA.prevPos.x < transformB.prevPos.x)
      {
        // Left → Right
        transformA.pos.x -= overlap.x;
      }
      else if (transformA.prevPos.x > transformB.prevPos.x)
      {
        // Right → Left
        transformA.pos.x += overlap.x;
      }
      transformA.vel.x = 0;
    }
  }
  if (tagA.tag == Object::Player && 
      (tagB.tag == Object::SlideRBbox || tagB.tag == Object::SlideLBbox))
  {
      Vec2 playerPoint = transformA.pos;
      playerPoint.y += bboxA.halfSize.y / 2;

      Vec2 playerPointPrev = transformA.prevPos;
      playerPointPrev.y += bboxA.halfSize.y / 2;

      Vec2 pointA = transformB.pos - bboxB.halfSize;
      Vec2 pointB = transformB.pos + bboxB.halfSize;

      if (isPointUnderLine(playerPoint, pointA, pointB))
      {
          Vec2 intersectP = linesIntersection(playerPointPrev, playerPoint, pointA, pointB);
          intersectP.y -= bboxA.halfSize.y / 2;

          slide(entityA, entityB, intersectP);
          return;
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
