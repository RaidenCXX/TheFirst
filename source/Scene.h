#pragma once

#include "EntityManager.h"
#include "Action.h"
#include "SFML/System/Time.hpp"
#include <string>
#include <vector>

using ActionMapKeyboard = std::vector<std::string>;
using ActionMapJoystick = std::vector<std::string>;

class GameEngine;

class Scene
{
protected:
  GameEngine*         m_gameEngine    = nullptr;
  EntityManager       m_entityManager;
  ActionMapKeyboard   m_actionMapKeyboard;
  ActionMapJoystick   m_actionMapJoystick;
  bool                m_paused        = false;
  bool                m_hasEnded      = false;
  size_t              m_curentFrame   = 0;

public:
  Scene(GameEngine* gameEngine);
  
  virtual void                      init(const std::string sceneConfigPath) = 0;
  virtual void                      update(sf::Time deltaTime) = 0;
  virtual void                      render(sf::Time deltaTime) = 0;
  virtual void                      sDoAction(const Action& action) = 0;
  virtual void                      sRender() = 0;
  virtual void                      doAction(const Action& action);
  
  void                              registerAction(const int inputKey, const std::string& actionKey, bool actionMap = true);
  GameEngine*                       getEngine() const;
  std::vector<std::string>&         getActionMap(bool actionMap); //If the value is true, the return ActionMap will be ActionMapKeyboard, if false ActionMapJoystick
};
