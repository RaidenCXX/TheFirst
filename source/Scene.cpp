#include "Scene.h"
#include "Action.h"
#include "SFML/Window/Keyboard.hpp"
#include <vector>




Scene::Scene(GameEngine* gameEngine)
  :m_gameEngine(gameEngine)
{
  m_actionMapKeyboard.resize(sf::Keyboard::KeyCount, "None");
  m_actionMapJoystick.resize(21, "None");
}

void Scene::registerAction(const int inputKey, const std::string& actionKey, bool actionMap)
{
  if(actionMap)
  {
    m_actionMapKeyboard[inputKey] = actionKey; 
  }
  else if (!actionMap)
  { 
    m_actionMapJoystick[inputKey] = actionKey;
  }
}

GameEngine* Scene::getEngine() const
{
  return m_gameEngine;
}

std::vector<std::string>& Scene::getActionMap(bool actionMap) //If the value is true, the return ActionMap will be ActionMapKeyboard, if false ActionMapJoystick
{
  if(actionMap)
  {
    return m_actionMapKeyboard;
  }
  else
  {
    return m_actionMapJoystick; 
  }

}

void Scene::doAction(const Action& action)
{
  if(action.getName() == "None")
    return;

  sDoAction(action);
}
