#include "Assets.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void Assets::loadFromFile(const std::string& pathToAssets)
{
  std::ifstream stream(pathToAssets);
  std::string   token;

  if(!stream.good())
  {
    std::cerr << "Assets::loadFromFile: stream good bit is false" << std::endl; 
  }


  while (stream.good())
  {
   stream >> token;

  try
    {
      if(token == "Texture")
      {
        std::string name, path;
        bool        smooth;

        stream >> name >> path >> smooth;
        addTexture(name, path, smooth);      
      }
      else if (token == "Animation")
      {
        std::string name, textureName;
        size_t  frameCount, speed;
        int layer;
        bool repeat;

        stream >> name >> textureName >> frameCount >> speed >> layer >> repeat;
        addAnimation(name, textureName, frameCount, speed, layer, repeat);
      }
      else if(token == "Font")
      { 
        std::string name, path;
        stream >> name >> path;

        addFont(name, path);
      }
      else if(token == "Shader")
      {
        std::string path;
        stream >> path;
        
        if(!sf::Shader::isAvailable())
        {
          throw std::runtime_error{"Shaders are not avaible on your system"};
        }

        if(!m_shaderSRGB.loadFromFile(path, sf::Shader::Type::Fragment))
        {
          throw std::runtime_error(path);
        }
      }
      else if(token == "End")
      {
        stream.close();
      }
      
    }
    catch(std::exception& e)
    {
      std::cout << e.what() << std::endl;
    }
  }
}


void Assets::addTexture(const std::string& name,const std::string& path, bool smooth)
{ 
  sf::Texture texture;
  
  if(!texture.loadFromFile(path))
  {
    throw std::runtime_error("Texture does not load: " + name);
  }
  texture.setSmooth(smooth);

  m_textureMap.try_emplace(name, std::move(texture));
  
  std::cout << "Texture was successfully loaded: " + name << std::endl;  
}

void Assets::addFont(const std::string& name, const std::string& path)
{
  sf::Font font(path);
  m_fontMap.try_emplace(name, std::move(font));
  std::cout << "Font was successfully loaded: " + name << std::endl;
}

void Assets::addAnimation(const std::string& name, const std::string& textureName,
  const size_t frameCount, size_t speed, int layer, bool repeat)
{ 
  m_animationMap.try_emplace(name ,name, m_textureMap.at(textureName), frameCount, speed, layer, repeat);
  std::cout << "Animation was successfully loaded: " + name << std::endl;
  //std::cout << "Layer: " << m_animationMap.find(name)->second.getLayer() << std::endl;
}

const sf::Texture& Assets::getTexture(const std::string& textureName)  const
{
  return m_textureMap.at(textureName);
}

Animation&  Assets::getAnimation(const std::string& animationName)
{
  return m_animationMap.at(animationName);
}

const sf::Font&  Assets::getFont(const std::string& fontName)  const
{
  auto it = m_fontMap.find(fontName);
  if(it != m_fontMap.end())
  {
    return m_fontMap.at(fontName);
  }
  throw std::runtime_error{"Font not found: "+fontName};
}
