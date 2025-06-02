#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "GameEngine.h"
#include "JoystickEnum.h"
#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Joystick.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Scene_Menu.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <fstream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <exception>
#include <vector>

GameEngine::GameEngine(const std::string &path)
{
  init(path);
}

void GameEngine::changeScene(const std::string sceneName, std::unique_ptr<Scene> scene, bool endCurrentScene)
{
  std::string tmpCurentScene = m_curentScene;
  if(scene)
  {
    if(m_sceneMap.find(sceneName) == m_sceneMap.end())
    {
      m_sceneMap.try_emplace(sceneName, std::move(scene));
      m_curentScene = sceneName;
    }
    else
      m_curentScene = sceneName;
  }
  else
  {
    if(m_sceneMap.find(sceneName) == m_sceneMap.end())
      std::cerr << "Warning: Scene " << sceneName << " does not exist" << std::endl;
  }

  if(endCurrentScene)
  {
    auto it = m_sceneMap.find(tmpCurentScene);
    if(it != m_sceneMap.end())
      m_sceneMap.erase(it);
  }
}


void GameEngine::init(const std::string& path)
{ 
  initConfig(path);

  //Joystick button state initialization
  m_prevJoystickButtonState.resize(sf::Joystick::getButtonCount(0), false);
  m_prevJoystickState.resize(20, 0);

  //Scene initialization
  changeScene("Menu", std::make_unique<SceneMenu>(this)); 
}

void GameEngine::initConfig(const std::string& path)  
{
  std::ifstream stream(path);
  std::string token;

  if(!stream.good())
  {
    std::cerr << "GameEngineameEngine::initConfig: stream good bit is false" << std::endl;
    exit(1); 
  }
  
  std::string   name, assetsConfigPath;
  int           width, hight, frameLimit;

  stream >> name >> width >> hight >> frameLimit >> assetsConfigPath; 
  
  m_window.create(sf::VideoMode(sf::Vector2u(width, hight)), name);
  m_window.setFramerateLimit(frameLimit);
  
  m_assets.loadFromFile(assetsConfigPath);
  stream >> token;
  while (stream.good() && token == "Scene")
  {
    std::string key, path;
    stream >> key >> path;
    if(key == "End")
    {
      break;
    }
    m_sceneConfig.try_emplace(key, path);
  }
  stream.close();
}


void GameEngine::userInput()
{

  while (const std::optional<sf::Event> event = m_window.pollEvent())
  {

    auto& Scene = m_sceneMap.find(m_curentScene)->second;


    if(event->is<sf::Event::Closed>())
    {
      quit();
    }
    else if (event->is<sf::Event::KeyPressed>())
    {
     
      const sf::Event::KeyPressed* key = event->getIf<sf::Event::KeyPressed>(); 

      if((Scene->getActionMap(true)[static_cast<int>(key->code)] == "None"))
      {
        continue;
      }

      Scene->doAction(Action{Scene->getActionMap(true)[static_cast<int>(key->code)], true});
    }
    else if (event->is<sf::Event::KeyReleased>())
    {

      const sf::Event::KeyReleased* key = event->getIf<sf::Event::KeyReleased>(); 
      
      if((Scene->getActionMap(true)[static_cast<int>(key->code)] == "None"))
      {
        continue;
      }

      Scene->doAction(Action{Scene->getActionMap(true)[static_cast<int>(key->code)], false});
    }
    
  //Joystick    
  
  m_joystickPos.lJoystickPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
  abs(m_joystickPos.lJoystickPosX) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.lJoystickPosX = 0 : m_joystickPos.lJoystickPosX;

  m_joystickPos.lJoystickPosY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
  abs(m_joystickPos.lJoystickPosY) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.lJoystickPosY = 0 : m_joystickPos.lJoystickPosY; 

  m_joystickPos.RJoystickPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U);
  abs(m_joystickPos.RJoystickPosX) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.RJoystickPosX = 0 : m_joystickPos.RJoystickPosX;  

  m_joystickPos.RJoystickPosY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V);
  abs(m_joystickPos.RJoystickPosY) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.RJoystickPosY = 0 : m_joystickPos.RJoystickPosY;  

  m_joystickPos.DPodX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
  abs(m_joystickPos.DPodX) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.DPodX = 0 : m_joystickPos.DPodX;  

  m_joystickPos.DPodY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);
  abs(m_joystickPos.DPodY) <= JOYSTICK_DEAD_ZONE ? m_joystickPos.DPodY = 0 : m_joystickPos.DPodY;  
  

  if( abs(m_joystickPos.lJoystickPosX) > 0 && !m_prevJoystickState[10]) // Left Joystick X Axis
  {
    if(m_joystickPos.lJoystickPosX > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJRight)], true});
      m_prevJoystickState[10] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJLeft)], true});
      m_prevJoystickState[10] = true; 
    }
  }
  else if(m_joystickPos.lJoystickPosX == 0 && m_prevJoystickState[10])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJRight)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJLeft)], false});
    m_prevJoystickState[10] = false;
  }

  else if( abs(m_joystickPos.lJoystickPosY) > 0 && !m_prevJoystickState[11]) // Left Joystick Y Axis
  {
    if(m_joystickPos.lJoystickPosY > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJDown)], true});
      m_prevJoystickState[11] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJUp)], true});
      m_prevJoystickState[11] = true;
    }
  }
  else if(m_joystickPos.lJoystickPosY == 0 && m_prevJoystickState[11])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJDown)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::LJUp)], false});
    m_prevJoystickState[11] = false;
  }

  
  else if( abs(m_joystickPos.RJoystickPosX) > 0 && !m_prevJoystickState[12]) //Right Joystick X Axis
  {
    if(m_joystickPos.RJoystickPosX > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJRight)], true});
      m_prevJoystickState[12] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJLeft)], true});
      m_prevJoystickState[12] = true; 
    }
  }
  else if(m_joystickPos.RJoystickPosX == 0 && m_prevJoystickState[12])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJLeft)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJRight)], false});
    m_prevJoystickState[12] = false;
  }
  
  else if( abs(m_joystickPos.RJoystickPosY) > 0 && !m_prevJoystickState[13]) //Right Joystick Y Axis
  {
    if(m_joystickPos.RJoystickPosY > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJRight)], true});
      m_prevJoystickState[13] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJLeft)], true});
      m_prevJoystickState[13] = true;
    }
  }
  else if(m_joystickPos.RJoystickPosY == 0 && m_prevJoystickState[13])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJLeft)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::RJRight)], false});
    m_prevJoystickState[13] = false;
  }

  else if( abs(m_joystickPos.DPodX) > 0 && !m_prevJoystickState[14]) //Right D-Pod X Axis
  {
    if(m_joystickPos.DPodX > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podR)], true});
      m_prevJoystickState[14] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podL)], true});
      m_prevJoystickState[14] = true;
    }
  }
  else if(m_joystickPos.DPodX == 0 && m_prevJoystickState[14])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podL)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podR)], false});
    m_prevJoystickState[14] = false;
  }

  else if( abs(m_joystickPos.DPodY) > 0 && !m_prevJoystickState[15]) //Right D-Pod Y Axis
  {
    if(m_joystickPos.DPodY > 0)
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podDown)], true});
      m_prevJoystickState[15] = true; 
    }
    else
    {
      Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podUp)], true});
      m_prevJoystickState[15] = true;
    }
  }
  else if(m_joystickPos.DPodY == 0 && m_prevJoystickState[15])
  {
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podDown)], false}); 
    Scene->doAction(Action{Scene->getActionMap(false)[static_cast<int>(Joystick::key::D_podUp)], false});
    m_prevJoystickState[15] = false;
  }


  for (size_t i = 0; i< m_prevJoystickButtonState.size(); i++)
  {
    bool curent = sf::Joystick::isButtonPressed(0, i);
    

    if(curent && !m_prevJoystickButtonState[i])
    {
      if(Scene->getActionMap(false)[i] == "None")  {continue;}
      
      Scene->doAction(Action(Scene->getActionMap(false)[i], true));
    }

    if(!curent && m_prevJoystickButtonState[i])
    {
      if(Scene->getActionMap(false)[i] == "None")  {continue;}
      
        Scene->doAction(Action(Scene->getActionMap(false)[i], false));
    }

    m_prevJoystickButtonState[i] = curent;
  }
  
  }
}


void GameEngine::run()
{
  

  try
  {
    while (isRunning())
      update();
  }

  catch (std::exception& e)
  {
    std::cerr <<"Exeption GameEngine::run: " << e.what() << std::endl;
  }
}

void GameEngine::update()
{
  sf::Time deltaTime = m_clock.restart();
  static sf::Time timeSinceLastRender = sf::Time::Zero;
  static sf::Time timeSinceLastUpdate = sf::Time::Zero;

  timeSinceLastRender += deltaTime;
  timeSinceLastUpdate += deltaTime;

  userInput();
  
  auto& scene = m_sceneMap.find(m_curentScene)->second;
  
  //Update Physics, Collisions
  while (timeSinceLastUpdate.asSeconds() >= UPDATE_TIME_PER_TICK)
  {
    scene->update(sf::seconds(UPDATE_TIME_PER_TICK));
    timeSinceLastUpdate -= sf::seconds(UPDATE_TIME_PER_TICK);
  }
  
  //Update Render, Animations
  if(timeSinceLastRender.asSeconds() >= RENDER_TIME_PER_TICK)
  {
    scene->render(deltaTime);
    timeSinceLastRender -= sf::seconds(RENDER_TIME_PER_TICK);
  }
}


void GameEngine::quit() 
{
  m_window.close();
}

bool GameEngine::isRunning()
{
	return m_running == m_window.isOpen();
}


const std::string&  GameEngine::getPathToScene(const std::string& sceneName)
{
  auto it = m_sceneConfig.find(sceneName);
  if(it != m_sceneConfig.end())
  {
    return it->second;
  }
  else
  {
    throw std::runtime_error{"For scene: " + sceneName + " a there is no path"};
  }
}


Assets& GameEngine::getAssets()
{
  return m_assets;
}


sf::RenderWindow& GameEngine::getWindow()
{
  return m_window;
}
