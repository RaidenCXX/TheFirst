
#include "Assets.h"
#include "SFML/System/Time.hpp"
#include "Vec2.h"
#include "Weapon.h"
#include <vector>
#include "Enums.h"

#define INVENTORY_SIZE 5

class Component
{
public:
  bool has = false;

  Component() = default;
  Component(bool has) :has(has)  {}
};

class CTransform :public Component
{
public:
  Vec2  pos;
  Vec2  prevPos;
  Vec2  scale;
  Vec2  vel;
  Vec2  dir;
  float angle;
  bool  dynamic = false;

  CTransform() :pos(0.0f, 0.0f), prevPos(0.0f, 0.0f), scale(0.0f, 0.0f), vel(0.0f, 0.0f), angle(0.0f) {}
  CTransform(float posX, float posY) :Component(true), pos(posX, posY), prevPos(0.0f, 0.0f), scale(0.0f, 0.0f), vel(0.0f, 0.0f), angle(0.0f) {}
  CTransform(Vec2&& posVec) :Component(true), pos(posVec.x, posVec.y), prevPos(0.0f, 0.0f), scale(0.0f, 0.0f), vel(0.0f, 0.0f), angle(0.0f) {}
  CTransform(Vec2& posVec) :Component(true), pos(posVec.x, posVec.y), prevPos(0.0f, 0.0f), scale(0.0f, 0.0f), vel(0.0f, 0.0f), angle(0.0f) {}
  CTransform(float posX, float posY, float prevPosX, float prevPosY, float scaleX, float scaleY, float velX, float velY, float angle)
    :Component(true), pos(posX, posY), prevPos(prevPosX, prevPosY), scale(scaleX, scaleY), vel(scaleX, scaleY), angle(angle) {}
};

class CAnimation :public Component
{
public:
  Animation animation;
  
  CAnimation() {}
  CAnimation(Animation& animation) :Component(true), animation(animation) {}
};

class CActive : public Component
{
public:
  bool active = false;

  CActive() :Component(true), active(false) {}
  CActive(bool active) :Component(true), active(active) {}
};

class CTag :public Component
{
public:
  static constexpr Object defaultTag = Object::None;
  Object tag = defaultTag;

  CTag() :Component(true), tag(defaultTag)  {}
  CTag(const Object tag) :Component(true), tag(tag) {}
};

class CInput :public Component
{
public:
  bool left           = false;
  bool right          = false;
  bool up             = false;
  bool down           = false;
  bool jump           = false;
  bool jumpPressed    = false;
  bool canJump        = true;
  bool shoot          = false;
  bool canShoot       = true;
  bool attack         = false;
  bool canAttack      = true;

  CInput() = default;
  CInput(bool hasComponent) :Component(hasComponent) {}
};

class CGravity :public Component
{
public:
  float gravity;
  float acceleration = 0;
  float fallMultiplier = 1.f;   
  float maxSpeedFall = 1;
  float jumpVelocity = 0;
  float lowJumpMultiplier = 1.f;  


  CGravity() = default;
  CGravity(float gravity) :Component(true), gravity(gravity) {}
  CGravity(float gravity, float maxSpeedFall) 
    :Component(true), gravity(gravity), maxSpeedFall(maxSpeedFall) {}
  CGravity(float gravity, float maxSpeedFall, float jumpVelocity) 
    :Component(true), gravity(gravity), maxSpeedFall(maxSpeedFall), jumpVelocity(jumpVelocity) {}
  CGravity(float gravity, float maxSpeedFall, float jumpVelocity, float fallMultiplier)
    :Component(true), gravity(gravity), maxSpeedFall(maxSpeedFall),
    jumpVelocity(jumpVelocity), fallMultiplier(fallMultiplier) {}
};

class CBoundingBox :public Component
{
public:
  Vec2 size;
  Vec2 halfSize;
  Vec2 offset = Vec2(0.0f, 0.0f);

  CBoundingBox() = default;
  CBoundingBox(float height, float width) 
    :Component(true), size(height, width), halfSize(height/2, width/2) {}
  CBoundingBox(float height, float width, Vec2 offset)
    :Component(true), size(height, width), halfSize(height/2, width/2), offset(offset) {}
};

class CState :public Component
{
public:
  bool onGround = false;
  bool canClimb = false;
  bool climp = false;
  

  CState () = default;
  CState (bool activate) :Component(true) { }
};

class CAttack :public Component
{
public:
  size_t    comboStep = 0;
  sf::Time  comboTimer;
  bool      attacking = false;
  bool      bufferedAttack = false;
  
  CAttack () {}
  CAttack (sf::Time&& comboTimer)
    :Component(true), comboTimer(comboTimer) {}
};

class CWeapon :public Component
{
public:
  std::vector<Weapon> weaponInventory;
  Weapon* firstWeapon = nullptr;
  Weapon* secondWeapon = nullptr;

  CWeapon () 
    :weaponInventory(), firstWeapon(), secondWeapon()
    {
      weaponInventory.resize(INVENTORY_SIZE);
      firstWeapon = &weaponInventory[0];
      secondWeapon = &weaponInventory[1];
    }

  CWeapon (Weapon&& firstWeapon) 
    :Component(true), weaponInventory(), firstWeapon(), secondWeapon()
    {
      weaponInventory.resize(INVENTORY_SIZE);
      weaponInventory[0] = std::move(firstWeapon);
      this->firstWeapon = &weaponInventory[0];
    }
};
