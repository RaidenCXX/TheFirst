#include "Scene_Prologue.h"
#include "EntityMemoryPool.h"
#include "Enums.h"
#include "JoystickEnum.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Scene.h"
#include "Entity.h"
#include "Action.h"
#include "Vec2.h"
#include "Collision.h"
#include <fstream>
#include <iostream>
#include <string>
#include "Weapon.h"
#include "math.h"


//Jump properties
#define GRAVITY  0.3f
#define JUMP_VElOCITY  12.0f
#define FALL_MULTIPLIER  2.5f
#define LOW_JUMP_MULTIPLIER  1.0f
#define MAX_SPEED_FALL  10.f

//Character properties
#define SPEED 5.f
#define SPEED_VERTICAL 1.f

//AnimationDepthLayer
#define MAX_DEPTH_LAYER 8

//Camera properties
#define CAMERA_ZOOM 0.3f

void ScenePrologue::init(const std::string sceneConfigPath)
{
  //Keyboard Key
  registerAction(static_cast<int>(sf::Keyboard::Key::Up),     "Up");
  registerAction(static_cast<int>(sf::Keyboard::Key::W),      "Up");
  registerAction(static_cast<int>(sf::Keyboard::Key::Down),   "Down");
  registerAction(static_cast<int>(sf::Keyboard::Key::S),      "Down");
  registerAction(static_cast<int>(sf::Keyboard::Key::Left),   "Left");
  registerAction(static_cast<int>(sf::Keyboard::Key::A),      "Left");
  registerAction(static_cast<int>(sf::Keyboard::Key::Right),  "Right");
  registerAction(static_cast<int>(sf::Keyboard::Key::D),      "Right");

  //Joystick
  registerAction(static_cast<int>(Joystick::key::LJUp),       "Up",     false);
  registerAction(static_cast<int>(Joystick::key::LJDown),     "Down",   false);
  registerAction(static_cast<int>(Joystick::key::LJLeft),     "Left",   false);
  registerAction(static_cast<int>(Joystick::key::LJRight),    "Right",  false);
  registerAction(static_cast<int>(Joystick::key::A),          "Jump",   false);  
  registerAction(static_cast<int>(Joystick::key::X),          "Attack", false);  
  registerAction(static_cast<int>(Joystick::key::Y),          "Shoot",  false);  

  loadLevel(sceneConfigPath); 

  m_view = m_gameEngine->getWindow().getDefaultView();
  m_view.zoom(CAMERA_ZOOM);    
}

std::string getFirstWord(const std::string& str)
{
  size_t pos = str.find("_");
  std::string firstWord = (pos != std::string::npos) ? str.substr(0, pos) : str;

  return firstWord;
}

Object convertToEnum(std::string& tag)
{
  if(tag == "Player")
    return Object::Player;

  else if (tag == "Tile")
    return Object::Tile;

  else if(tag == "TileBbox")
    return Object::TileBbox;

  else if(tag == "SlideRBbox")
    return Object::SlideRBbox;

  else if (tag == "SlideLBbox")
    return Object::SlideLBbox;

  else if (tag == "Bridge")
    return Object::Bridge;
  
  else if (tag == "Ladder")
    return Object::Ladder;
  
  else if (tag == "SmallPlatform")
    return Object::SmallPlatform;

  else if (tag == "Background")
    return Object::Background;

  else
    return Object::None;
}

void  ScenePrologue::loadLevel(const std::string sceneConfigPath)
{
  std::ifstream stream(sceneConfigPath);
  std::string token;
  
  

  while (stream.good())
  {
    stream >> token;    

    if(convertToEnum(token) == Object::Player)
    {
      Vec2 pos;
      std::string animName;
      bool repeat;
      
      stream >> pos.x >> pos.y >> animName >> repeat; 
      playerInit(pos);
    }
    else if(convertToEnum(token) == Object::TileBbox)
    {

      Vec2 pos;
      std::string animationName;
      bool repeat;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::TileBbox);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
    }
    else if( convertToEnum(token) == Object::SlideRBbox ||
             convertToEnum(token) == Object::SlideLBbox)
    {
      Vec2 pos;
      std::string animationName;
      Object type = convertToEnum(token);
      bool repeat;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(type);
 
      e.addComponent<CTransform>(newPos);
      e.getComponent<CTransform>().dir = type == Object::SlideRBbox? 
        Vec2{1.f, -1.f} : Vec2{-1.f, -1.f};
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
    }
    else if(convertToEnum(token) == Object::Bridge)
    {
      Vec2 pos;
      std::string animationName;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Bridge);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      
      Animation& bbox = m_gameEngine->getAssets().getAnimation(animationName);
      e.addComponent<CBoundingBox>(bbox.getSize().x , bbox.getSize().y/2, Vec2{0.f, bbox.getSize().y/4});
    }
    else if(convertToEnum(token) == Object::Tile)
    {
      Vec2 pos;
      std::string animationName;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Tile);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});  
    }
    else if (convertToEnum(token) == Object::Ladder)
    {
      Vec2 pos;
      std::string animationName;
      bool repeat;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Ladder);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x * 0.2, e.getComponent<CAnimation>().animation.getSize().y);
    }
    else if (convertToEnum(token) == Object::SmallPlatform)
    {
      Vec2 pos;
      std::string animationName;
      bool repeat;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::SmallPlatform);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
    }
    else if(convertToEnum(token) == Object::Background)
    {
      Vec2 pos;
      std::string animationName;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Background);
 
      e.addComponent<CTransform>(newPos);
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
    }
    else if(token == "End")
    {
      std::cerr << "End of file" << std::endl;
      stream.close();
    }
  } 
}

Vec2  ScenePrologue::gridToMidPixel(float gridX, float gridY)
{
  return Vec2{(gridX * 16) - 8, (gridY * 16) - 8};
}

void ScenePrologue::playerInit(Vec2& pos)
{

  Entity player = m_entityManager.addEntity(Object::Player);
 
  player.addComponent<CTransform>(gridToMidPixel(pos.x, pos.y));
  player.getComponent<CTransform>().dynamic = true;
  player.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation("Idle_Anim"));
  player.addComponent<CInput>(true);
  player.addComponent<CBoundingBox>(20.f, 36.f);
  player.addComponent<CGravity>(GRAVITY, MAX_SPEED_FALL, JUMP_VElOCITY, FALL_MULTIPLIER);
  player.addComponent<CState>(true);
  player.addComponent<CWeapon>(Weapon{"Sword", sf::seconds(0.5f), 3});
}


ScenePrologue::ScenePrologue(GameEngine* gameEngine)
  :Scene(gameEngine), m_gridText(getEngine()->getAssets().getFont("Default")) 
{
  init(gameEngine->getPathToScene("ScenePrologue"));
}


void  ScenePrologue::update(sf::Time deltaTime)
{
  sMovement();
  sAttack(deltaTime);
  sCollision();
}

void ScenePrologue::render(sf::Time deltaTime)
{
  sAnimation(deltaTime);
  sRender();
}

void  ScenePrologue::sRender()
{
  m_gameEngine->getWindow().clear(sf::Color{28, 165, 255});

  CTransform& transform = m_entityManager.getEntity(m_entityManager.getPlayerIndex()).getComponent<CTransform>();

  sf::Vector2f viewCenter = m_view.getCenter();
  sf::Vector2f viewSize   = m_view.getSize();

  float deadZoneWidth = viewSize.x * 0.1f;  
  float deadZoneHeight = viewSize.y * 0.1f; 

  float deadZoneLeft   = viewCenter.x - deadZoneWidth / 2.0f;
  float deadZoneRight  = viewCenter.x + deadZoneWidth / 2.0f;
  float deadZoneTop    = viewCenter.y - deadZoneHeight / 2.0f;
  float deadZoneBottom = viewCenter.y + deadZoneHeight / 2.0f;
 
  sf::Vector2f newCenter = viewCenter;

  if (transform.pos.x < deadZoneLeft)
    newCenter.x = transform.pos.x + deadZoneWidth / 2.0f;
  else if (transform.pos.x > deadZoneRight)
    newCenter.x = transform.pos.x - deadZoneWidth / 2.0f;

  if (transform.pos.y < deadZoneTop)
    newCenter.y = transform.pos.y + deadZoneHeight / 2.0f;
  else if (transform.pos.y > deadZoneBottom)
    newCenter.y = transform.pos.y - deadZoneHeight / 2.0f;
  
  float halfWidth = viewSize.x / 2.0f;
  // float halfHeight = viewSize.y / 2.0f;  limits for y axis

  if (newCenter.x - halfWidth < 0)
    newCenter.x = halfWidth;

  // if (newCenter.y - halfHeight < 0)  limit for y axis
  //   newCenter.y = halfHeight;

  m_view.setCenter(newCenter);
  m_gameEngine->getWindow().setView(m_view);
 

  //Rendering entities 
  for (int i = MAX_DEPTH_LAYER; i > 0; i--)
  {
    for (Entity& e : m_entityManager.getEntities())
    {
      if(e.getComponent<CActive>().active && e.getComponent<CAnimation>().has && e.getComponent<CAnimation>().animation.getLayer() == i)
      {
        CTag& tag = e.getComponent<CTag>();
        sf::Sprite& sprite = e.getComponent<CAnimation>().animation.getSprite();
        CTransform& transform = e.getComponent<CTransform>();
        float xViewPos = m_gameEngine->getWindow().getView().getCenter().x;

        if(tag.tag == Object::Tile ||
          tag.tag == Object::TileBbox ||
          tag.tag == Object::SlideRBbox ||
          tag.tag == Object::SlideLBbox)
        {
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
        }
        else if(tag.tag == Object::Bridge)
        {
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
        }
        else if(tag.tag == Object::SmallPlatform)
        {
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
        }
        else if(tag.tag == Object::Ladder)
        {
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
        }
        else if(tag.tag == Object::Background)
        {
          float factor = m_paralaxFactors[e.getComponent<CAnimation>().animation.getLayer()];
          sprite.setPosition(sf::Vector2f{transform.pos.x + (xViewPos * factor), transform.pos.y});

          m_gameEngine->getWindow().draw(sprite);
        }
        else if(tag.tag == Object::Player)
        {
          e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});  
          e.getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f{e.getComponent<CTransform>().scale.x, 1.f});
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
        }
      }
    }
  }
  
  //Rendering bounding box 
  renderBoundingBox(false);  

  m_gameEngine->getWindow().display();
}

void ScenePrologue::renderBoundingBox(bool active)
{
  if(active)
  {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(-0.3f);
    rect.setOutlineColor(sf::Color::White);
    for (Entity& e : m_entityManager.getEntities())
    {
      if(e.getComponent<CActive>().active && e.getComponent<CBoundingBox>().has)
      {
        rect.setSize(sf::Vector2f{e.getComponent<CBoundingBox>().size.x, e.getComponent<CBoundingBox>().size.y});
        rect.setOrigin(sf::Vector2f{e.getComponent<CBoundingBox>().halfSize.x, e.getComponent<CBoundingBox>().halfSize.y});
      
        rect.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x + e.getComponent<CBoundingBox>().offset.x,
          e.getComponent<CTransform>().pos.y + e.getComponent<CBoundingBox>().offset.y});
        m_gameEngine->getWindow().draw(rect);
      }
    }
  }
}


void ScenePrologue::sDoAction(const Action& action)
{
  CInput& input = m_entityManager.getEntity(m_entityManager.getPlayerIndex()).getComponent<CInput>();
  
  if(action.getType())                            //If the key is pressed
  {
    if(action.getName()       ==  "Left")
    {
      input.left        = true; 
    }
    else if (action.getName() ==  "Right")
    {
      input.right       = true;  
    }
    else if(action.getName()  ==  "Up")
    {
      input.up          = true;
    }
    else if (action.getName() ==  "Down")
    {
      input.down        = true;  
    }
    else if (action.getName() ==  "Jump")
    {
      input.jump        = true;
      input.jumpPressed = true;
    }
    else if(action.getName()  ==  "Shoot")
    {
      input.shoot       = true;   
    }
    else if(action.getName()  == "Attack")
    {
      input.attack = true;
    }
  }
  else if(!action.getType())                       //If the key is released
  {
    if(action.getName()       ==  "Left")
    {
      input.left      = false; 
    }
    else if (action.getName() ==  "Right")
    {
      input.right     = false;  
    }
    else if(action.getName()  ==  "Up")
    {
      input.up        = false;  
    }
    else if (action.getName() ==  "Down")
    {
      input.down      = false;  
    }
    else if (action.getName() ==  "Jump")
    {
      input.jump      = false;   
    }
    else if(action.getName()  ==  "Shoot")
    {
      input.shoot     = false;   
    }
    else if(action.getName()  == "Attack")
    {
      input.attack = false;
    }
  }
}

void  ScenePrologue::sAnimation(sf::Time deltaTime)
{
  m_entityManager.getEntity(Object::Player).getComponent<CAnimation>().animation.update(deltaTime);
}

void  ScenePrologue::changeAnimation(Entity& entity, std::string&& animationName, bool repeat)
{
  if(entity.getComponent<CAnimation>().animation.getName() != animationName)
  {
    Vec2& scale = entity.getComponent<CTransform>().scale;
    CAnimation& anim = entity.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
    anim.animation.getSprite().setScale(sf::Vector2f{scale.x, 1.0f});
  }
}

void ScenePrologue::sMovement()
{
  for (Entity& e : m_entityManager.getEntities())
  {
    if (!e.getComponent<CActive>().active) continue;

    CTransform& cTransform = e.getComponent<CTransform>();

    if (e.getComponent<CTag>().tag == Object::Player)
    {
      CInput& cInput      = e.getComponent<CInput>();
      CGravity& gravity   = e.getComponent<CGravity>();
      CState& cState      = e.getComponent<CState>();
      CAttack& cAttack    = e.getComponent<CAttack>();

      // === ATTACK INPUT ===
      if (cInput.attack)
      {
        // Атака обробляється окремо в sAttack
      }
      // === SHOOT INPUT ===
      else if (cInput.shoot)
      {
        cInput.canShoot = true;
      }

      // === LADDER MOVEMENT ===
      if (cState.canClimb)
      {
        // Початок лазіння вгору
        if (cInput.up)
        {
          cState.climp      = true;
          cState.onGround   = false;
          cTransform.vel.y  = -SPEED_VERTICAL;
          changeAnimation(e, "Ladder_Climb_Anim");
        }
        // Початок лазіння вниз
        else if (cInput.down)
        {
          cState.climp      = true;
          cState.onGround   = false;
          cTransform.vel.y  = SPEED_VERTICAL;
          changeAnimation(e, "Ladder_Climb_Anim");
        }
        // Якщо вже ліземо — тримати анімацію і швидкість по Y
        else if (cState.climp)
        {
          cTransform.vel.y = 0.0f;
          changeAnimation(e, "Ladder_Climb_Anim");
          e.getComponent<CAnimation>().animation.reset();
        }
        // Якщо просто біля драбини — нічого не робимо з вертикальним рухом
      }
      else if(!cState.canClimb)
      {
        cState.climp = false;
      }

      // === HORIZONTAL MOVE (доступний завжди, навіть коли canClimb) ===
      if (!cAttack.attacking)
      {
        if (cInput.left)
        {
          cTransform.vel.x -= SPEED;
          cTransform.scale.x = -1.0f;

          if (cState.onGround)
            changeAnimation(e, "Run_Anim");
        }
        else if (cInput.right)
        {
          cTransform.vel.x += SPEED;
          cTransform.scale.x = 1.0f;

          if (cState.onGround)
            changeAnimation(e, "Run_Anim");
        }
      }

      // === IDLE / IN-AIR ANIMATION ===
      if (!cInput.left && !cInput.right)
      {
        if (cState.onGround && !cAttack.attacking && !cState.climp)
        {
          changeAnimation(e, "Idle_Anim");
        }
        else if (!cState.onGround && !cState.climp)
        {
          changeAnimation(e, "Jump_Anim", false);
        }
      }

      // === JUMP ===
      if (cInput.jumpPressed && cState.onGround)
      {
        cTransform.vel.y = -gravity.jumpVelocity;
        cState.onGround  = false;
        gravity.acceleration = 0.0f;
        changeAnimation(e, "Jump_Anim", false);
      }

      // === GRAVITY (працює тільки якщо не лазимо) ===
      if (gravity.has && !cState.climp)
      {
        gravity.acceleration += gravity.gravity;

        // Додаткове прискорення під час падіння
        if (cTransform.vel.y > 0.0f)
        {
          cTransform.vel.y += gravity.gravity * (gravity.fallMultiplier - 1.0f);
        }

        cTransform.vel.y += gravity.acceleration;

        // Обмеження максимальної швидкості падіння
        if (cTransform.vel.y > gravity.maxSpeedFall)
        {
          cTransform.vel.y = gravity.maxSpeedFall;
        }
      }

      // === VELOCITY UPDATE ===
      cTransform.prevPos = cTransform.pos;
      cTransform.pos    += cTransform.vel;

      // === RESET HORIZONTAL VELOCITY ===
      cTransform.vel.x = 0.0f;

      // === RESET jumpPressed AFTER FRAME ===
      cInput.jumpPressed = false;

      // === RESET canClimb — оновиться колізією наступного кадру ===
      cState.canClimb = false;
    }
  }
}



void ScenePrologue::sAttack(sf::Time deltaTime)
{
  for (Entity& e : m_entityManager.getEntities())
  {
    if (!e.getComponent<CActive>().active) continue;
    if (e.getComponent<CTag>().tag != Object::Player) continue;

    CInput&     cInput      = e.getComponent<CInput>();
    CAttack&    cAttack     = e.getComponent<CAttack>();
    CAnimation& cAnimation  = e.getComponent<CAnimation>();
    CWeapon&    cWeapon     = e.getComponent<CWeapon>();
    Weapon&     firstWeapon = *cWeapon.firstWeapon;

    // If the player has no weapon
    if (firstWeapon.getWeaponName() == "None") continue;

    // If attack is pressed
    if (cInput.attack)
    {
      // If the player is already attacking - the animation is started
      if (cAttack.attacking)
      {
        cAttack.bufferedAttack = true;
      }
      // If the player has not yet attacked
      else
      {
        cAttack.attacking = true;
        cAttack.comboStep = 1;
        std::string animName = firstWeapon.getWeaponName() + "_Attack_" + std::to_string(cAttack.comboStep) + "_Anim";
        cAnimation.animation = m_gameEngine->getAssets().getAnimation(animName);
        cAnimation.animation.reset();

        cAttack.comboTimer = firstWeapon.getComboTimer();
      }
    }

    // If the player's attack is not the first
    if (cAttack.attacking)
    {
      cAttack.comboTimer -= deltaTime;

      // If animation is Finished
      if (cAnimation.animation.isFinished())
      {
        // If bufferedAttack is true and hawe combo steps
        if (cAttack.bufferedAttack && cAttack.comboStep < firstWeapon.getNumberCombo())
        {
          cAttack.comboStep++;
          std::string animName = firstWeapon.getWeaponName() + "_Attack_" + std::to_string(cAttack.comboStep) + "_Anim";
          cAnimation.animation = m_gameEngine->getAssets().getAnimation(animName);
          cAnimation.animation.reset();

          cAttack.comboTimer = firstWeapon.getComboTimer();
          cAttack.bufferedAttack = false;
        }
        else
        {
          // Combo ends
          cAttack.attacking = false;
          cAttack.comboStep = 0;
          cAttack.bufferedAttack = false;

          // return to Idle anim
          cAnimation.animation = m_gameEngine->getAssets().getAnimation("Idle_Anim");
          cAnimation.animation.reset();
        }
      }

      // If combo timer is ends — reset attack
      if (cAttack.comboTimer <= sf::Time::Zero)
      {
        cAttack.attacking = false;
        cAttack.comboStep = 0;
        cAttack.bufferedAttack = false;

        cAnimation.animation = m_gameEngine->getAssets().getAnimation("Idle_Anim");
        cAnimation.animation.reset();
      }
    }

    // Reset input attack state
    cInput.attack = false;
  }
}


void  ScenePrologue::sCollision()
{
  for (Entity& entity : m_entityManager.getEntities())
  {
    if (!entity.getComponent<CActive>().active) continue;

    if (entity.getComponent<CTag>().tag == Object::Player)
    {
      if (entity.hasComponent<CState>())
        entity.getComponent<CState>().onGround = false;

      for (Entity& e : m_entityManager.getEntities())
      {
        if (!e.getComponent<CActive>().active) continue;
        if (e.getComponent<CTag>().tag != Object::TileBbox &&
            e.getComponent<CTag>().tag != Object::Bridge &&
            e.getComponent<CTag>().tag != Object::Ladder &&
            e.getComponent<CTag>().tag != Object::SmallPlatform &&
            e.getComponent<CTag>().tag != Object::SlideRBbox &&
            e.getComponent<CTag>().tag != Object::SlideLBbox) continue;
        
        Collision::resolveCollision(entity, e);
      }
    }
  }
}
