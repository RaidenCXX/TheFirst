#pragma once
#include "Entity.h"
#include "GameEngine.h"
#include "NavigationNodeManager.h"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Time.hpp"
#include "Scene.h"
#include "Vec2.h"
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>


class ScenePrologue : public Scene 
{
  std::string levelPath;
  
  sf::View                      m_view;
  NavigationNodeManager         m_navigationManager;
  Vec2                          m_gridSize {16, 16};
  bool									        m_drawTexture = true;
	bool									        m_drawCollision = false;
	bool									        m_drawGrid = false;
  const std::vector<float>      m_paralaxFactors = {0.f, 0.0f, 0.0f, 0.2f, 0.2f, 0.6f, 0.7f, 0.8};
  sf::Text                      m_gridText;
  std::condition_variable       m_cv;
  std::mutex                    m_loaderMutex;
  size_t                        m_activeThreadCount = 0;
  

  
  void                playerInit(Vec2& pos);
  void                loadLevel(const std::string sceneConfigPath);
  void                loadNodeMesh(const std::string sceneConfigPath);
  Vec2                gridToMidPixel(float gridX, float gridY);
  void                renderBoundingBox(bool active);
  void                changeAnimation(Entity& entity, std::string&& animationName, bool repeat = true);
public:

  ScenePrologue(GameEngine* gameEngine);
  
  virtual void        init(const std::string sceneConfigPath) override;
  virtual void        update(sf::Time deltaTime)              override;
  virtual void        render(sf::Time deltaTime)              override;
  virtual void        sDoAction(const Action& action)         override;
  virtual void        sRender()                               override;
          void        sAnimation(sf::Time deltaTime);
          void        sMovement();
          void        sAttack(sf::Time deltaTime);
          void        sCollision();
};
