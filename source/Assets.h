#pragma once

#include <SFML/Graphics/Font.hpp>
#include <map>
#include <string>

#include "Animation.h"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/Texture.hpp"

class Assets
{
  private:
  std::map<std::string, Animation>      m_animationMap;
  std::map<std::string, sf::Texture >   m_textureMap;
  std::map<std::string, sf::Font>       m_fontMap;
  
  void addTexture(const std::string& name, const std::string& path, bool smooth);
  void addFont(const std::string& name, const std::string& path);
  void addAnimation(const std::string& name, const std::string& texture,
    const size_t frameCount, size_t speed = 1, int layer = 0, bool repeat = false);
  
  public: 
  sf::Shader m_shaderSRGB;
  
  void loadFromFile(const std::string& pathToAssets);

  const sf::Texture&  getTexture(const std::string& textureName)     const;
  Animation&          getAnimation(const std::string& animationName) ;
  const sf::Font&     getFont(const std::string& fontName)           const;
};
