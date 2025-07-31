#include "Scene_Prologue.h"
#include "Assets.h"
#include "EntityMemoryPool.h"
#include "Enums.h"
#include "JoystickEnum.h"
#include "NavigationNode.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Angle.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Scene.h"
#include "Entity.h"
#include "Action.h"
#include "Vec2.h"
#include "Collision.h"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
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
#define SLIDE_SPEED 4.0f
#define PLAYER_HP 200

//AnimationDepthLayer
#define MAX_DEPTH_LAYER 8

//Camera properties
#define CAMERA_ZOOM 0.3f

//Mushroom properties
#define M_PATROL_DISTANCE 100
#define M_SPEED 1.5f
#define M_VIEW_DISTANCE 216.f 
#define M_FOV 0.5f
#define M_FOLLOW_DISTANCE 32.f
#define M_HP 100
//EnemyAI System
#define THRESHOLD_DISTANCE 32.f

void  ScenePrologue::update(sf::Time deltaTime)
{
  sEnemyAI();
  sMovement();
  sAttack(deltaTime);
  sCollision();
  sLife(deltaTime);
}

void ScenePrologue::render(sf::Time deltaTime)
{
  sAnimation(deltaTime);
  sRender();
}

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
  registerAction(static_cast<int>(sf::Keyboard::Key::Space),  "Jump");
  registerAction(static_cast<int>(sf::Keyboard::Key::J),      "Attack");
  registerAction(static_cast<int>(sf::Keyboard::Key::K),      "Shoot");
  
  registerAction(static_cast<int>(sf::Keyboard::Key::B),       "Bound");

  //Joystick
  registerAction(static_cast<int>(Joystick::key::LJUp),       "Up",     false);
  registerAction(static_cast<int>(Joystick::key::LJDown),     "Down",   false);
  registerAction(static_cast<int>(Joystick::key::LJLeft),     "Left",   false);
  registerAction(static_cast<int>(Joystick::key::LJRight),    "Right",  false);
  registerAction(static_cast<int>(Joystick::key::A),          "Jump",   false);  
  registerAction(static_cast<int>(Joystick::key::X),          "Attack", false);  
  registerAction(static_cast<int>(Joystick::key::Y),          "Shoot",  false);  

  m_view = m_gameEngine->getWindow().getDefaultView();
  m_view.zoom(CAMERA_ZOOM);    
 
  size_t& counter = m_activeThreadCount;
  counter = 2;
  std::thread lLoader(&ScenePrologue::loadLevel, this, sceneConfigPath);
  std::thread nLoader(&ScenePrologue::loadNodeMesh, this, sceneConfigPath); 
  
  std::unique_lock<std::mutex> lock(m_loaderMutex);
  
  
  sf::RenderWindow& w = m_gameEngine->getWindow();
  Animation& anim = m_gameEngine->getAssets().getAnimation("Loading_Anim");
  anim.getSprite().setOrigin(sf::Vector2f{anim.getSize().x / 2, anim.getSize().y / 2});
  anim.getSprite().setPosition(sf::Vector2f{w.getSize().x * 0.9f, w.getSize().y * 0.9f});
  anim.getSprite().setScale(sf::Vector2f{5.f, 5.f});
  while (counter != 0)
  {
    m_cv.wait_for(lock, std::chrono::milliseconds(100));

    w.clear(sf::Color::Black);
    anim.update();
    w.draw(anim.getSprite());
    w.display();
  }

  m_hpPlayer.HpRamp = m_gameEngine->getAssets().getAnimation("HealthBarPanel_Anim");
  m_hpPlayer.HpBar = m_gameEngine->getAssets().getAnimation("HealthBar_Anim");
  m_hpPlayer.HpBar.getSprite().setOrigin(m_hpPlayer.HpBar.getSprite().getOrigin() - m_hpPlayer.HpBar.getSprite().getLocalBounds().size * 0.5f);
  
  lLoader.join();
  nLoader.join();

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
  
  else if (tag == "Mushroom")
    return Object::Mushroom;

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
      Vec2 pos, weaponBbox;
      std::string animName, weaponName;
      float animationDuration;
      size_t numbersAnimations;
      unsigned short weaponDamage;
      bool repeat;
 
      stream >> pos.x >> pos.y >> animName >> repeat >>
        weaponName >> animationDuration >> numbersAnimations >>
        weaponDamage >> weaponBbox.x >> weaponBbox.y;

      playerInit(pos, animName, repeat,
                 Weapon{weaponName ,sf::seconds(animationDuration),
                        numbersAnimations, weaponBbox, weaponDamage});
    }
    else if(convertToEnum(token) == Object::Mushroom)
    {
      Vec2 pos, weaponBbox;
      std::string animationName, weaponName;
      float animationDuration;
      size_t numbersAnimations;
      unsigned short weaponDamage;
      bool repeat;

      stream >> pos.x >> pos.y >> animationName >> repeat >>
        weaponName >> animationDuration >> numbersAnimations >>
         weaponDamage >> weaponBbox.x >> weaponBbox.y;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Mushroom);
      
      e.addComponent<CTransform>(newPos);
      CTransform& tr = e.getComponent<CTransform>();
      tr.prevPos = e.getComponent<CTransform>().pos;
      tr.scale = Vec2{1.0f, 1.0f};
      e.addComponent<CAnimation>(m_gameEngine->getAssets().
                                 getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().
        setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x,
                                e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>()
                                   .animation.getSize().x,
                                   e.getComponent<CAnimation>().
                                   animation.getSize().y);
      e.addComponent<CGravity>(GRAVITY, MAX_SPEED_FALL,
                               JUMP_VElOCITY,
                               FALL_MULTIPLIER);
      e.addComponent<CState>();
      e.addComponent<CEnemyAI>(EnemyState::Patrol, M_PATROL_DISTANCE);
      e.addComponent<CWeapon>(Weapon{weaponName,
                                    sf::seconds(animationDuration),
                                    numbersAnimations,weaponBbox ,
                                    weaponDamage});
      e.addComponent<CAttack>();
      e.addComponent<CHp>(M_HP);
      std::cout << "Mushroom added " << std::endl;
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
      e.getComponent<CTransform>().prevPos = e.getComponent<CTransform>().pos;
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
      std::cout << "TileBbox added " << std::endl;
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
        Vec2{SLIDE_SPEED, -SLIDE_SPEED} : Vec2{-SLIDE_SPEED, -SLIDE_SPEED};
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
      std::cout << "Slide Bbox added " << std::endl;
    }
    else if(convertToEnum(token) == Object::Bridge)
    {
      Vec2 pos;
      std::string animationName;
      
      stream >> pos.x >> pos.y >> animationName;
      
      Vec2 newPos = gridToMidPixel(pos.x, pos.y);
      
      Entity e = m_entityManager.addEntity(Object::Bridge);
 
      e.addComponent<CTransform>(newPos);
      e.getComponent<CTransform>().prevPos = e.getComponent<CTransform>().pos;
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      
      Animation& bbox = m_gameEngine->getAssets().getAnimation(animationName);
      e.addComponent<CBoundingBox>(bbox.getSize().x , bbox.getSize().y, Vec2{0.f, bbox.getSize().y / 2});
      std::cout << "Bridge added " << std::endl;
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
      std::cout << "Tile added " << std::endl;
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
      e.getComponent<CTransform>().prevPos = e.getComponent<CTransform>().pos;
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x * 0.2, e.getComponent<CAnimation>().animation.getSize().y);
      std::cout << "Ladder added " << std::endl;
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
      e.getComponent<CTransform>().prevPos = e.getComponent<CTransform>().pos;
      e.addComponent<CAnimation>(m_gameEngine->getAssets().getAnimation(animationName));
      e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});
      e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize().x,  e.getComponent<CAnimation>().animation.getSize().y);
      std::cout << "SmallPlatform added " << std::endl;
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
      std::cout << "Mushroom Background " << std::endl;
    }
    else if(convertToEnum(token) == Object::None)
    {
      continue;
    }
    else if(token == "End")
    {
      std::cerr << "End of file" << std::endl;
      stream.close();
    }
  }
  std::lock_guard<std::mutex> lock(m_loaderMutex);
  m_activeThreadCount--;
  m_cv.notify_one();
}

NNType converToNNT(std::string& type)
{
  if(type == "End")
    return NNType::End;
  
  else if(type == "Transitional")
    return NNType::Transitional;

  else
    return NNType::None;
}

void  ScenePrologue::loadNodeMesh(const std::string sceneConfigPath)
{
  std::ifstream stream(sceneConfigPath);
  std::string token;

  while (stream.good())
  {
    stream >> token;
    
    if(token == "NavigationNode")
    {
      std::string type;
      float x, y;
      unsigned short id, right, left, down, up;

      stream >> type >> x >> y >> id >> right >> left >> down >> up;

      Vec2 pos = gridToMidPixel(x, y);
      m_navigationManager.addNode(NavigationNode{converToNNT(type), pos, id, right, left, down, up});
    }
    else if(token == "End")
    {
      stream.close();
    }
  }
  

  std::lock_guard<std::mutex> lock(m_loaderMutex);
  m_activeThreadCount--;
  m_cv.notify_one();
}

Vec2  ScenePrologue::gridToMidPixel(float gridX, float gridY)
{
  return Vec2{(gridX * 16) - 8, (gridY * 16) - 8};
}


void ScenePrologue::playerInit(Vec2& pos, const std::string &animName, 
                               bool repeat, Weapon&& weapon)
{
  Entity player = m_entityManager.addEntity(Object::Player);
 
  player.addComponent<CTransform>(gridToMidPixel(pos.x, pos.y));
  player.getComponent<CTransform>().dynamic = true;
  player.getComponent<CTransform>().scale = Vec2{1.0f, 1.0f};
  player.addComponent<CAnimation>(m_gameEngine->getAssets().
                                  getAnimation(animName));
  player.addComponent<CInput>(true);
  player.addComponent<CBoundingBox>(20.f, 36.f);
  player.addComponent<CGravity>(GRAVITY, MAX_SPEED_FALL,
                                JUMP_VElOCITY, FALL_MULTIPLIER);
  player.addComponent<CState>(true);
  player.addComponent<CWeapon>(std::move(weapon));
  player.addComponent<CHp>(PLAYER_HP);
  std::cout << "Player is added\n";
}


ScenePrologue::ScenePrologue(GameEngine* gameEngine)
  :Scene(gameEngine), m_gridText(getEngine()->getAssets().getFont("Default")) 
{
  init(gameEngine->getPathToScene("ScenePrologue"));
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

  if (newCenter.x - halfWidth < 0)
    newCenter.x = halfWidth;


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
        else if(tag.tag == Object::Mushroom)
        {
          e.getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x, e.getComponent<CTransform>().pos.y});  
          e.getComponent<CAnimation>().animation.getSprite().setScale(sf::Vector2f{e.getComponent<CTransform>().scale.x, 1.f});
          m_gameEngine->getWindow().draw(e.getComponent<CAnimation>().animation.getSprite(), &m_gameEngine->getAssets().m_shaderSRGB);
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

  //Player HP bar
  CHp& playerHp = m_entityManager.getEntities()[m_entityManager.getPlayerIndex()].getComponent<CHp>();
  float Hp;
  playerHp.healthPoints <= 0 ? Hp = 0 : Hp  = (playerHp.healthPoints / 2.f) * 0.01;

  m_hpPlayer.HpRamp.getSprite().setPosition(m_view.getCenter() - m_view.getSize() * 0.43f);
  m_hpPlayer.HpBar.getSprite().setPosition(m_view.getCenter() - (m_view.getSize() * 0.43f) -
    (static_cast<sf::Vector2f>(m_hpPlayer.HpBar.getSprite().getTexture().getSize()) * 0.5f ));
 
  m_hpPlayer.HpBar.getSprite().setScale(sf::Vector2f{Hp , 1.f});
  m_gameEngine->getWindow().draw(m_hpPlayer.HpBar.getSprite());
  m_gameEngine->getWindow().draw(m_hpPlayer.HpRamp.getSprite());

  //Rendering bounding box 
  renderBoundingBox(m_drawCollision);

  m_gameEngine->getWindow().display();
}

void ScenePrologue::renderBoundingBox(bool active)
{
  if(active)
  {
    sf::RectangleShape rect;
    sf::CircleShape circl;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(-0.3f);
    rect.setOutlineColor(sf::Color::White);
    sf::CircleShape triangl(5.f , 3);
    triangl.setOrigin( sf::Vector2f{triangl.getPosition().x + (triangl.getRadius() * 0.5f),
                                    triangl.getPosition().y + (triangl.getRadius() * 0.5f)});
    triangl.setFillColor(sf::Color::Green);

    circl.setFillColor(sf::Color::Cyan);
    circl.setRadius(2.f);

    

    for (Entity& e : m_entityManager.getEntities())
    {
      CTransform& cTr = e.getComponent<CTransform>();
      CTag&       cTag = e.getComponent<CTag>();

      if(e.getComponent<CActive>().active && e.getComponent<CBoundingBox>().has)
      {
        rect.setSize(sf::Vector2f{e.getComponent<CBoundingBox>().size.x, e.getComponent<CBoundingBox>().size.y});
        rect.setOrigin(sf::Vector2f{e.getComponent<CBoundingBox>().halfSize.x, e.getComponent<CBoundingBox>().halfSize.y});
         
        if(e.getComponent<CTag>().tag == Object::Player)
        {
          circl.setOrigin(sf::Vector2f{circl.getRadius(), circl.getRadius()});
          circl.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x,
            e.getComponent<CTransform>().pos.y + e.getComponent<CBoundingBox>().halfSize.y});
        }
        else if(e.getComponent<CTag>().tag == Object::SlideRBbox)
        {
          circl.setOrigin(sf::Vector2f{circl.getRadius(), circl.getRadius()});
          circl.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x - e.getComponent<CBoundingBox>().halfSize.x,
            e.getComponent<CTransform>().pos.y - e.getComponent<CBoundingBox>().halfSize.y});
          m_gameEngine->getWindow().draw(circl);

          circl.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x + e.getComponent<CBoundingBox>().halfSize.x,
            e.getComponent<CTransform>().pos.y + e.getComponent<CBoundingBox>().halfSize.y});
        }
        else if(e.getComponent<CTag>().tag == Object::SlideLBbox)
        {
          circl.setOrigin(sf::Vector2f{circl.getRadius(), circl.getRadius()});
          circl.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x + e.getComponent<CBoundingBox>().halfSize.x,
            e.getComponent<CTransform>().pos.y - e.getComponent<CBoundingBox>().halfSize.y});
          m_gameEngine->getWindow().draw(circl);

          circl.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x - e.getComponent<CBoundingBox>().halfSize.x,
            e.getComponent<CTransform>().pos.y + e.getComponent<CBoundingBox>().halfSize.y});
        }

        if(cTag.tag == Object::Player ||
          cTag.tag   == Object::Mushroom)
        {
          triangl.setPosition(sf::Vector2f{cTr.pos.x, cTr.pos.y});
          if(cTr.scale.x > 0)
            triangl.setRotation(sf::degrees(90.f));
          else if(cTr.scale.x < 0)
            triangl.setRotation(sf::degrees(270.f));

          m_gameEngine->getWindow().draw(triangl);
        }

        rect.setPosition(sf::Vector2f{e.getComponent<CTransform>().pos.x + e.getComponent<CBoundingBox>().offset.x,
          e.getComponent<CTransform>().pos.y + e.getComponent<CBoundingBox>().offset.y});
        m_gameEngine->getWindow().draw(rect);
        m_gameEngine->getWindow().draw(circl);
      }
    }
    //NavigationNode display
   
    sf::Text t(m_gameEngine->getAssets().getFont());
    t.setFillColor(sf::Color::White);
    t.setScale(sf::Vector2f{t.getScale().x * 0.3f, t.getScale().y * 0.3f});
    sf::CircleShape c;
    c.setRadius(2.f);
    c.setOrigin(sf::Vector2f{c.getRadius(), c.getRadius()});
    c.setFillColor(sf::Color::Yellow);
    for (NavigationNode& node : m_navigationManager.getNodes())
    {
      c.setPosition(sf::Vector2f{node.pos.x, node.pos.y});
      m_gameEngine->getWindow().draw(c);
      t.setPosition(sf::Vector2f{node.pos.x, node.pos.y});
      t.setString(std::to_string(node.id));
      m_gameEngine->getWindow().draw(t);
    }

    //Enemy Hp bar
    sf::RectangleShape enemyBar;
    enemyBar.setFillColor(sf::Color::Red);

    for (Entity &e : m_entityManager.getEntities())
    {
      if(e.getComponent<CTag>().tag != Object::Mushroom) continue; 
      CTransform &eTr = e.getComponent<CTransform>();
      CBoundingBox &eBB = e.getComponent<CBoundingBox>();
      CHp &eHp = e.getComponent<CHp>();
      enemyBar.setPosition(sf::Vector2f{eTr.pos.x - eBB.halfSize.x, eTr.pos.y - eBB.halfSize.y});
      enemyBar.setSize(sf::Vector2f{eBB.size.x, 1.f});
      
      eHp.healthPoints < 0 ? eHp.healthPoints = 0 : 0 ;
      
      float bar = eHp.healthPoints * 0.01;

      enemyBar.setScale(sf::Vector2f{bar ,1.f});
      m_gameEngine->getWindow().draw(enemyBar);
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
    else if (action.getName() == "Bound")
    {
      m_drawCollision = !m_drawCollision;
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
  m_entityManager.getEntity(Object::Mushroom).getComponent<CAnimation>().animation.update(deltaTime);
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

      if(!cState.died)
      {
        // === ATTACK INPUT ===
        if (cInput.attack && !cState.slide)
        {
          // The attack is handled separately in sAttack
        }
        // === SHOOT INPUT ===
        else if (cInput.shoot && !cState.slide)
        {
          cInput.canShoot = true;
        }

        // === LADDER MOVEMENT ===
        if (cState.canClimb)
        {
          // Start of climbing up
          if (cInput.up)
          {
            cState.climp      = true;
            cState.onGround   = false;
            cTransform.vel.y  = -SPEED_VERTICAL;
            changeAnimation(e, "Ladder_Climb_Anim");
          }
          // Start of climbing down
          else if (cInput.down)
          {
            cState.climp      = true;
            cState.onGround   = false;
            cTransform.vel.y  = SPEED_VERTICAL;
            changeAnimation(e, "Ladder_Climb_Anim");
          }
          // If we are already climbing, keep the animation and speed along Y
          else if (cState.climp)
          {
            cTransform.vel.y = 0.0f;
            changeAnimation(e, "Ladder_Climb_Anim");
            e.getComponent<CAnimation>().animation.reset();
          }
          // If you are just near a ladder, you are not doing anything with vertical movement.
        }
        else if(!cState.canClimb)
        {
          cState.climp = false;
        }

        // === HORIZONTAL MOVE (always available, even when canClimb) ===
        if (!cAttack.attacking)
        {
          if (cInput.left && !cState.slide)
          {
            cTransform.vel.x -= SPEED;
            cTransform.scale.x = -1.0f;

            if (cState.onGround)
              changeAnimation(e, "Run_Anim");
          }
          else if (cInput.right && !cState.slide)
          {
            cTransform.vel.x += SPEED;
            cTransform.scale.x = 1.0f;

            if (cState.onGround && !cState.slide)
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
            cTransform.scale.x = (cTransform.prevPos.x - cTransform.pos.x) <= 0 ?
              1 : -1;
          }
        }

        if(cState.slide)
        {
          changeAnimation(e, "Slide_Anim", false);
        }

        // === JUMP ===
        if (cInput.jumpPressed && (cState.onGround || cState.slide))
        {
          cTransform.vel.y = -gravity.jumpVelocity;
          cState.onGround  = false;
          gravity.acceleration = 0.0f;
          changeAnimation(e, "Jump_Anim", false);
        }
      }
        // === GRAVITY (only works if we don't climb) ===
        if (gravity.has && !cState.climp)
        {
          gravity.acceleration += gravity.gravity;

          // Additional acceleration during a fall
          if (cTransform.vel.y > 0.0f)
          {
            cTransform.vel.y += gravity.gravity * (gravity.fallMultiplier - 1.0f);
          }

          cTransform.vel.y += gravity.acceleration;

          // Maximum fall speed limit
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

      // === RESET canClimb — will be updated with the collision of the next frame ===
      cState.canClimb = false;
      cState.slide = false;
    }
    else if (e.getComponent<CTag>().tag == Object::Mushroom)
    {
      CInput& cInput      = e.getComponent<CInput>();
      CGravity& gravity   = e.getComponent<CGravity>();
      CState& cState      = e.getComponent<CState>();
      CAttack& cAttack    = e.getComponent<CAttack>();
     if(!cState.died)
    {
      if(cInput.attack)
      { }

      if(!cAttack.attacking)
      {
        if (cInput.left)
        {
          cTransform.vel.x -= M_SPEED;
          cTransform.scale.x = -1.0f;

          if (cState.onGround)
            changeAnimation(e, "Mushroom_Run_Anim");
        }
        else if (cInput.right)
        {
          cTransform.vel.x += M_SPEED;
          cTransform.scale.x = 1.0f;

          if (cState.onGround && !cState.slide)
            changeAnimation(e, "Mushroom_Run_Anim");
        }
        else if (!cInput.left && !cInput.right)
        {
          if (cState.onGround)
            changeAnimation(e, "Mushroom_Idle_Anim"); 
        }
      }
    }
    // === GRAVITY (only works if we don't climb) ===
    if (gravity.has && !cState.climp)
    {
      gravity.acceleration += gravity.gravity;

      // Additional acceleration during a fall
      if (cTransform.vel.y > 0.0f)
      {
        cTransform.vel.y += gravity.gravity * (gravity.fallMultiplier - 1.0f);
      }

      cTransform.vel.y += gravity.acceleration;

      // Maximum fall speed limit
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
    }
  }
}

void ScenePrologue::sAttack(sf::Time deltaTime)
{
  for (Entity& e : m_entityManager.getEntities())
  {
    if (!e.getComponent<CActive>().active) continue;
    
    CTag& cTag              = e.getComponent<CTag>();
    if(!(e.getComponent<CTag>().tag == Object::Player ||
         e.getComponent<CTag>().tag == Object::Mushroom)) continue;
    
    CState&     cState      = e.getComponent<CState>();
    if(cState.died) continue;
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

        cAttack.damageOnThisAnim = false;
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
          cAttack.damageOnThisAnim = false;
        }
        else
        {
          // Combo ends
          cAttack.attacking = false;
          cAttack.comboStep = 0;
          cAttack.bufferedAttack = false;

          // return to Idle anim 
          switch (cTag.tag)
          {
          case Object::Player:
            cAnimation.animation = m_gameEngine->getAssets().getAnimation("Idle_Anim");
            cAnimation.animation.reset();
            break;
          case Object::Mushroom:
            cAnimation.animation = m_gameEngine->getAssets().getAnimation("Mushroom_Idle_Anim");
            cAnimation.animation.reset();
            break;
          default:
            break;
          }
        }
      }
        
      if(cAttack.comboTimer < firstWeapon.getComboTimer() * 0.6f &&
         cAttack.comboTimer > firstWeapon.getComboTimer() * 0.4f &&
         !cAttack.damageOnThisAnim)
      {
        for (Entity& eTarget : m_entityManager.getEntities())
        {
          if(&e == &eTarget) continue;
          CTag& tagTarget = eTarget.getComponent<CTag>();
          if(!(tagTarget.tag == Object::Player ||
            tagTarget.tag == Object::Mushroom))   continue;
         
          bool weponColl = Collision::getWeaponOverlap(e, eTarget, firstWeapon.getWeaponBBox());
          std::cout << weponColl << std::endl;
          if(!(cAttack.damageOnThisAnim) && weponColl)
          {
            eTarget.getComponent<CHp>().healthPoints -= firstWeapon.getDamage();
            std::cout <<"HP: " <<e.getComponent<CHp>().healthPoints << " Damage: " << firstWeapon.getDamage() << std::endl;
            cAttack.damageOnThisAnim = true;
          }
          
        }
      }

      // If combo timer is ends — reset attack
      if (cAttack.comboTimer <= sf::Time::Zero)
      {
        cAttack.attacking = false;
        cAttack.comboStep = 0;
        cAttack.bufferedAttack = false;
        
        switch (cTag.tag)
        {
        case Object::Player:
          cAnimation.animation = m_gameEngine->getAssets().getAnimation("Idle_Anim");
          cAnimation.animation.reset();
          break;
        case Object::Mushroom:
          cAnimation.animation = m_gameEngine->getAssets().getAnimation("Mushroom_Idle_Anim");
          cAnimation.animation.reset();
          break;
        default:
          break;
        }
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
        
        float distance = entity.getComponent<CTransform>().pos.distance(e.getComponent<CTransform>().pos);
        if(distance >  m_gameEngine->getWindow().getSize().x / 2.f) continue;

        if (e.getComponent<CTag>().tag != Object::TileBbox &&
            e.getComponent<CTag>().tag != Object::Bridge &&
            e.getComponent<CTag>().tag != Object::Ladder &&
            e.getComponent<CTag>().tag != Object::SmallPlatform &&
            e.getComponent<CTag>().tag != Object::SlideRBbox &&
            e.getComponent<CTag>().tag != Object::SlideLBbox) continue;
        
        Collision::resolveCollision(entity, e);
      }
    }
    else if (entity.getComponent<CTag>().tag == Object::Mushroom)
    {
      for (Entity& e : m_entityManager.getEntities())
      {
        if (!e.getComponent<CActive>().active) continue;

        float distance = entity.getComponent<CTransform>().pos.distance(e.getComponent<CTransform>().pos);
        if(distance >  m_gameEngine->getWindow().getSize().x / 2.f) continue;

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


void ScenePrologue::nodeScane(NavigationNode& node, CEnemyAI&           enemyAI,
                              std::vector<unsigned short>&              path,
                              std::unordered_set<unsigned short>&       visited,
                              std::vector<NavigationNode>&              nodes,
                              std::vector<std::vector<unsigned short>>& calculatedPathes)
{
  // If  already visited - exit
  if (visited.count(node.id)) return;

  // Mark as visited
  visited.insert(node.id);

  // Add to the path
  path.push_back(node.id);

  // If we find the end, we add the entire path
  if (node.nodeType == NNType::End && visited.size())
  {
    if(visited.size() > 1)
    {
      calculatedPathes.push_back(path);
      path.pop_back();
      return;
    }
  }
  // Depth-first neighbor search
  
  if (node.left != 0)
    nodeScane(nodes[node.left], enemyAI, path, visited, nodes, calculatedPathes);
  if (node.up != 0)
    nodeScane(nodes[node.up], enemyAI, path, visited, nodes, calculatedPathes);
  if (node.right != 0)
    nodeScane(nodes[node.right], enemyAI, path, visited, nodes, calculatedPathes);
  if (node.down != 0)
    nodeScane(nodes[node.down], enemyAI, path, visited, nodes, calculatedPathes);

  // We are going back along the path
  path.pop_back();
}

bool playerVisibility(Entity& entityA, Entity& entityB, std::vector<Entity> allEntity)
{
  Vec2 playerPos = entityA.getComponent<CTransform>().pos; 
  Vec2 enemyPos  = entityB.getComponent<CTransform>().pos;

  //If the player is too far away
  Vec2 dist = enemyPos - playerPos;
  if(dist.lenght() >= M_VIEW_DISTANCE) return false;

  //If the gaze is not directed towards the player
  Vec2 enemyViewDir {entityB.getComponent<CTransform>().scale.x * -1, 0.f};
  if(enemyViewDir.dot(dist.normalize()) < M_FOV) return false;

  //Is there no visual barrier in front of the enemy?
  for (Entity& e : allEntity)
  {
    if(!e.getComponent<CActive>().active) continue;

    CTag& tag = e.getComponent<CTag>();
    if(!(tag.tag == Object::TileBbox ||
       tag.tag == Object::SlideRBbox ||
       tag.tag == Object::SlideLBbox ||
       tag.tag == Object::Ladder)) continue;

    Vec2& pos = e.getComponent<CTransform>().pos;
    Vec2& hSize = e.getComponent<CBoundingBox>().halfSize;

    //Comparing with 4 sides of BoundingBox
    
    Vec2 intersectP = Collision::lineIntersect(playerPos, enemyPos,
                                      Vec2{pos.x, pos.y + hSize.y},
                                      Vec2{pos.x, pos.y - hSize.y});

    // if(intersectP.x != 0 && intersectP.y != 0)
      // std::cout <<"intersectP: " << intersectP.x << "\t" << intersectP.y << std::endl;
    if(intersectP != 0)
      return false;
  }
  return true;
}

void ScenePrologue::sEnemyAI()
{
  std::vector<NavigationNode>& nodes = m_navigationManager.getNodes();
  Entity& player = m_entityManager.getEntities()[m_entityManager.getPlayerIndex()];
  CTransform& playerTr = player.getComponent<CTransform>();
  
  unsigned short collidedPlayerNodeId = 0;
  for (NavigationNode& n : nodes)
  {
    if(Collision::NNodeCollision(player, n) != 0)
      collidedPlayerNodeId = n.id;
  }
  
  for (Entity& e : m_entityManager.getEntities())
  {
    if (!e.getComponent<CEnemyAI>().has) continue;
    
    CEnemyAI& enemyAI = e.getComponent<CEnemyAI>();
    CInput& eInput = e.getComponent<CInput>();
    CTransform& eTr = e.getComponent<CTransform>(); 
   
    // Vision of the enemy section
    bool EnemyCanSeePlayer   = playerVisibility(player, e, m_entityManager.getEntities());
    float playerDistance     = playerTr.pos.distance(eTr.pos);
    bool playerEnemyCollide  = Collision::getOverlap(player, e) != 0;
    // AI state switching
    switch (enemyAI.enemyState)
    {
      case EnemyState::Patrol:
        // If the enemy sees the player, switch to canAttack
        if (EnemyCanSeePlayer)
        {
          enemyAI.enemyState = EnemyState::canAttack;
        }
        break;
      case EnemyState::canAttack:
        // If already touching the player — start attacking
        if (playerEnemyCollide)
        {
          enemyAI.enemyState = EnemyState::Attack;
        }
        // If you lose sight of the player and don't touch it, return to patrolling
        else if (!EnemyCanSeePlayer)
        {
          enemyAI.enemyState = EnemyState::Patrol;
        }
        break;
      // If no longer touching the player — reduce aggression to canAttack
      case EnemyState::Attack:
        if (!playerEnemyCollide)
        {
          enemyAI.enemyState = EnemyState::canAttack;
        }
        break;
      case EnemyState::None:
        // Initial state, can be assigned to Patrol or canAttack as needed
        enemyAI.enemyState = EnemyState::Patrol;
        break;
    }

    
    if(enemyAI.prevEnemyState != enemyAI.enemyState)
    { 
      // std::cout << "Clear path ";
      enemyAI.hawePath = false;
      enemyAI.path.clear();
    }

    // if(eTr.scale.x == -1)
    // {
    //   std::cout << "-->";
    // }
    // else if(eTr.scale.x == 1)
    // {
    //   std::cout << "<--";
    // }

    enemyAI.prevEnemyState = enemyAI.enemyState;

    // if(enemyAI.enemyState == EnemyState::Patrol)
    // {
    //   std::cout << "State  Patrol ";
    // }
    // else if(enemyAI.enemyState == EnemyState::canAttack)
    // {
    //   std::cout << "State  canAttack ";
    // }
    // else if(enemyAI.enemyState == EnemyState::Attack)
    // {
    //   std::cout << "State  Attack ";
    // }
    //
    // for (auto& i : enemyAI.path)
    //   std::cout << i << " ";
    //
    // std::cout << " CoPlayerNodeId: " << collidedPlayerNodeId << ", ";
    // std::cout << " Path state: " << enemyAI.hawePath;
    // std::cout << std::endl;
    //
    unsigned short collidedEnemyNodeId = 0; 
    
    for (NavigationNode& n : nodes)
    {
      if(!n.active) continue;
      
      if(Collision::NNodeCollision(e, n) != 0 && collidedEnemyNodeId == 0)
        collidedEnemyNodeId = n.id;
      
      if (collidedEnemyNodeId == 0) continue;
 
      if(collidedPlayerNodeId != enemyAI.collidedPlayerNodeId &&
        enemyAI.enemyState == EnemyState::canAttack)
      {
        enemyAI.hawePath = false;
      }
      
      enemyAI.collidedPlayerNodeId = collidedPlayerNodeId;

      if(enemyAI.enemyState == EnemyState::Patrol)
      {
        if (!enemyAI.hawePath)
        {
          std::vector<std::vector<unsigned short>> calculatedPathes;
          std::unordered_set<unsigned short> visited;
          std::vector<unsigned short> path;
          nodeScane(n, enemyAI, path, visited, nodes, calculatedPathes);

          if (calculatedPathes.empty()) continue;
         
          if(calculatedPathes.size() > 1)
          {
            m_gameEngine->getRandGen().seed(
              std::chrono::steady_clock::now().time_since_epoch().count()
            );
            
            std::uniform_int_distribution<> dist(0, calculatedPathes.size()-1);
            enemyAI.path = calculatedPathes[dist(m_gameEngine->getRandGen())];
          }
          else
          {
            enemyAI.path = calculatedPathes[0];
          }
          

          std::reverse(enemyAI.path.begin(), enemyAI.path.end());
          enemyAI.hawePath = true;
          break;
        }
        if(enemyAI.hawePath)
        { 
          unsigned short  targetNodeId = enemyAI.path[enemyAI.path.size()-1];
          NavigationNode& targetNode  = nodes[targetNodeId];
          CTransform&     enemyT =  e.getComponent<CTransform>();
          
          float distance = targetNode.pos.distance(enemyT.pos);

          if(distance < THRESHOLD_DISTANCE)
          {
            enemyAI.path.pop_back();
            if(enemyAI.path.empty())
            {
              enemyAI.hawePath = false;
              eInput.right=eInput.left = false;
            }
            return;
          }

          
          if(targetNode.pos.x < enemyT.pos.x)
          {
            eInput.left = true;
          }
          else if(targetNode.pos.x > enemyT.pos.x)
          {
            eInput.right = true;
          }
        }
      }
      else if(enemyAI.enemyState == EnemyState::canAttack)
      {
        if(!enemyAI.hawePath)
        {
          std::vector<std::vector<unsigned short>> calculatedPathes;
          std::unordered_set<unsigned short> visited;
          std::vector<unsigned short> path;
          
          nodeScane(nodes[collidedEnemyNodeId], enemyAI, path, visited, nodes, calculatedPathes);

          if (calculatedPathes.empty()) continue;
          
          for (const auto& p : calculatedPathes)
          {
            if(std::find(p.begin(), p.end(), collidedPlayerNodeId) != p.end())
            {
              enemyAI.path = p;
              break;
            }
          }
          
          auto it = std::find(enemyAI.path.begin(), enemyAI.path.end(), collidedPlayerNodeId);
          if (it != enemyAI.path.end())
              enemyAI.path.erase(it+1, enemyAI.path.end());

          if (!enemyAI.path.empty())
          {
            std::reverse(enemyAI.path.begin(), enemyAI.path.end());
            enemyAI.hawePath = true;
            enemyAI.path.pop_back();
          }
          break;
        }
        if(enemyAI.hawePath)
        {
          if (enemyAI.path.empty())
          {
            enemyAI.hawePath = false;
            return;
          }

          unsigned short targetNodeId = enemyAI.path.back();
          Vec2           targetPos = nodes[targetNodeId].pos;
          CTransform&    enemyT = e.getComponent<CTransform>();
          
          float distance = targetPos.distance(enemyT.pos);

          if (distance < THRESHOLD_DISTANCE)
            enemyAI.path.pop_back();
         
          eInput.right=eInput.left=false;
          
          if (playerTr.pos.x < enemyT.pos.x )
            eInput.left = true;
          else if (playerTr.pos.x > enemyT.pos.x)
            eInput.right = true;
        }
      }
      else if(enemyAI.enemyState == EnemyState::Attack)
      {
        eInput.right=eInput.left=false;
        eInput.attack = true;
      }
    } 
  }
}

void  ScenePrologue::sLife(sf::Time deltaTime)
{
  for (Entity& e : m_entityManager.getEntities())
  {
    if(!e.getComponent<CActive>().active) continue;
    
    CHp& eHp = e.getComponent<CHp>();
    CState& eState = e.getComponent<CState>();
    CAnimation& eAnim = e.getComponent<CAnimation>();
    if(eHp.healthPoints <= 0 && !eState.died)
    {
      CTag& eTag = e.getComponent<CTag>();
      switch (eTag.tag)
      {
      case Object::Player:
        eAnim.animation = m_gameEngine->getAssets().getAnimation("Die_Anim");
        eState.died = true;
        break;
      case Object::Mushroom:
        eAnim.animation = m_gameEngine->getAssets().getAnimation("Mushroom_Die_Anim");
        eState.died = true;
        break;
      default:
        break;
      }
    }
    else if(eState.died && eAnim.animation.isFinished())
    {
      m_entityManager.destroyEntity(e);  
    }
  }
}
