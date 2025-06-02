#pragma once

#include "Assets.h"
#include "GameEngine.h"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Time.hpp"
#include "Scene.h"
#include <cstddef>
#include <string>
#include <vector>




class SceneMenu :public Scene
{
  
  std::vector<std::string>  m_menuString;
  std::vector<std::string>  m_levelPath;
  sf::Text                  m_menuText;
  Animation&                m_beckground;
  std::string               m_title;
  size_t                    m_selectedMenuIndex = 0;

public:
  SceneMenu(GameEngine* gameEngine);
  
  virtual void init(const std::string sceneConfigPath) override;

  virtual void update(sf::Time deltaTime) override;
  virtual void render(sf::Time deltaTime) override;
  virtual void sDoAction(const Action& action) override;
  virtual void sRender() override;
};
