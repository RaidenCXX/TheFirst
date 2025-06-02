#include "Scene_Menu.h"
#include "SFML/System/Time.hpp"
#include "Scene_Prologue.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Scene.h"
#include <cmath>
#include <exception>
#include <fstream>
#include <memory>
#include <string>
#include <iostream>
#include "JoystickEnum.h"

SceneMenu::SceneMenu(GameEngine* gameEngine)
  :Scene(gameEngine), m_menuText(getEngine()->getAssets().getFont("Default")), m_beckground(m_gameEngine->getAssets().getAnimation("MenuStatic"))
{
  try
  {
  init(gameEngine->getPathToScene("Menu"));

  //Keyboard
  registerAction(static_cast<int>(sf::Keyboard::Key::Up),   "Up",     true);
  registerAction(static_cast<int>(sf::Keyboard::Key::Down), "Down",   true);
  registerAction(static_cast<int>(sf::Keyboard::Key::Enter),"Select", true);
 
  //Joystick
  registerAction(static_cast<int>(Joystick::key::A),        "Select", false);
  registerAction(static_cast<int>(Joystick::key::LJDown),   "Down",   false);
  registerAction(static_cast<int>(Joystick::key::LJUp),     "Up",     false);
  registerAction(static_cast<int>(Joystick::key::RJDown),   "Down",   false);
  registerAction(static_cast<int>(Joystick::key::RJUp),     "Up",     false);
  registerAction(static_cast<int>(Joystick::key::D_podDown),"Down",   false);
  registerAction(static_cast<int>(Joystick::key::D_podUp),  "Up",     false);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception Scene_Menu constructor: " << e.what() << std::endl;
  }


  m_beckground.getSprite().setPosition(sf::Vector2f{static_cast<float>( std::floor(m_gameEngine->getWindow().getSize().x / 2)), static_cast<float>( std::floor(m_gameEngine->getWindow().getSize().y / 2))});
  
  float backSizeX = m_gameEngine->getWindow().getSize().x/m_beckground.getSize().x;
  float backSizeY = m_gameEngine->getWindow().getSize().y/m_beckground.getSize().y;
  m_beckground.getSprite().setScale(sf::Vector2f{backSizeX, backSizeY});


}


void SceneMenu::init(const std::string sceneConfigPath)
{

  std::ifstream stream(sceneConfigPath);
  std::string token;

  while (stream.good())
  {
    stream >> token;

    if(token == "Title")
    {
      stream >> m_title; 
    }
    else if (token == "MenuString")
    {
      m_menuString.resize(4);
      stream >> m_menuString[0] >> m_menuString[1] >> m_menuString[2] >> m_menuString[3];
    }
    else if (token == "Font")
    {
      std::string fontName;
    stream >> fontName;
      
      m_menuText.setFont(this->getEngine()->getAssets().getFont(fontName));
    }
    else if(token == "End")
    {
      stream.close();
      break;
    }
  }
}

void SceneMenu::update(sf::Time deltaTime)
{

}

void SceneMenu::render(sf::Time deltaTime)
{
  sRender(); 
}

void SceneMenu::sRender()
{
  getEngine()->getWindow().clear(sf::Color(50,50,100));
  

  m_gameEngine->getWindow().draw(m_beckground.getSprite());
  
  m_menuText.setString(m_title);
  m_menuText.setFillColor(sf::Color{243, 73, 76});
  m_menuText.setPosition(sf::Vector2f{static_cast<float>(getEngine()->getWindow().getSize().x / 1.5f), static_cast<float>(getEngine()->getWindow().getSize().y / 13.f)});
  m_menuText.setScale(sf::Vector2f{3.f, 3.0f});
  m_gameEngine->getWindow().draw(m_menuText);

  sf::Vector2f windowSize {};
  float sizeX = getEngine()->getWindow().getSize().x;
  float sizeY = getEngine()->getWindow().getSize().y;


  getEngine()->getWindow().draw(m_menuText);

  int count = 0;
  for (std::string& e : m_menuString)
  {
    m_menuText.setString(e);
    if(count == m_selectedMenuIndex)
    {
      m_menuText.setFillColor(sf::Color{243, 73, 76});
      m_menuText.setScale(sf::Vector2f{1.2f, 1.2f});
    }
    else
    {
      m_menuText.setFillColor(sf::Color::White);
      m_menuText.setScale(sf::Vector2f{1.f, 1.f});
    }
    m_menuText.setPosition(sf::Vector2f{static_cast<float>(getEngine()->getWindow().getSize().x / 20.f), (static_cast<float>(getEngine()->getWindow().getSize().y / 2.f) + (count * 40)) });
    getEngine()->getWindow().draw(m_menuText);
    count++;
  }

  getEngine()->getWindow().display();
  getEngine()->getWindow().clear();
}


void SceneMenu::sDoAction(const Action& action)
{
  if(action.getType())
  {
    if(action.getName() == "Select")
    {
      switch (m_selectedMenuIndex)
      {
      case 0:
        getEngine()->changeScene("Prologue", std::make_unique<ScenePrologue>(m_gameEngine));
        std::cout << "Select case: " << m_selectedMenuIndex <<" New_Game" << std::endl;
        break;
      case 1:
        //getEngine()->
        std::cout << "Select case: " << m_selectedMenuIndex <<" Cont" << std::endl;
        break;
      case 2:
        std::cout << "Select case: " << m_selectedMenuIndex << " Settings"<< std::endl;
        break;
      case 3:
        std::cout << "Select case: " << m_selectedMenuIndex <<" Exit" << std::endl;
        getEngine()->quit();
        break;
      } 
    }
    else if(action.getName() == "Up")
    {
      m_selectedMenuIndex == 0 ? m_selectedMenuIndex = m_menuString.size() - 1 : m_selectedMenuIndex--;
    }
    else if(action.getName() == "Down")
    {
      m_selectedMenuIndex == m_menuString.size() - 1 ? m_selectedMenuIndex = 0 : m_selectedMenuIndex++; 
    }
  }
}
