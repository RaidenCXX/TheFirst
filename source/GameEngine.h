#pragma once

#include <map>
#include <memory>
#include <string>
#include "SFML/Graphics/RenderWindow.hpp"
#include "Assets.h"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Joystick.hpp"
#include "Scene.h"
#include <map>
#include <vector>


#define RENDER_TIME 60.0f
#define UPDATE_TIME 30.0f
#define RENDER_TIME_PER_TICK (1.0f / RENDER_TIME)
#define UPDATE_TIME_PER_TICK (1.0f / UPDATE_TIME)

#define JOYSTICK_DEAD_ZONE 40

using SceneMap = std::map<std::string, std::unique_ptr<Scene>>;
using ConfigScenePath = std::map<std::string, std::string>;

struct JoystickPos
{
  short lJoystickPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
  short lJoystickPosY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
  short RJoystickPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U);
  short RJoystickPosY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V);
  short DPodX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
  short DPodY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);

  bool addAllComponents ()
  {
    bool result;
    result += static_cast<bool>(lJoystickPosX);
    result += static_cast<bool>(lJoystickPosY);
    result += static_cast<bool>(RJoystickPosX);
    result += static_cast<bool>(RJoystickPosY);
    result += static_cast<bool>(DPodX);
    result += static_cast<bool>(DPodY);
    return result;
  }
};

class GameEngine
{
  private:
	sf::RenderWindow          m_window;
  sf::Clock                 m_clock;
	Assets                    m_assets;
  JoystickPos               m_joystickPos;
  ConfigScenePath           m_sceneConfig;
  std::vector<bool>         m_prevJoystickButtonState;
  std::vector<bool>         m_prevJoystickState;
	std::string               m_curentScene;
	SceneMap                  m_sceneMap;
	size_t                    m_simulationSpeed = 1;
	bool                      m_running = true;

	void init(const std::string &path);
  void initConfig(const std::string& path);
  
  void userInput();

  public:
	GameEngine(const std::string &path);
  
  void changeScene(const std::string sceneName, std::unique_ptr<Scene> scene, bool endCurrentScene = false);

	void run();
	bool isRunning();
  void update();
  void quit();

  const   std::string&  getPathToScene(const std::string& sceneName);
          Assets&       getAssets();
  sf::RenderWindow&     getWindow();
  
};
