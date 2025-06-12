#include "SFML/Graphics/Sprite.hpp"

#include <string>
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Time.hpp"
#include "Vec2.h"

class Animation
{
 
  sf::Sprite        m_sprite; 
  std::string       m_name        =   {"None"};
  Vec2              m_size        =   {0,0}; 
  size_t            m_speed       =   1;
  size_t            m_frameCount  =   1;
  size_t            m_curentFrame =   0;
  size_t            m_animFrame   =   0;
  short             m_layer       =   -1;
  bool              m_repeat      =   false;
  bool              m_finished    =   false;
  sf::Texture       m_emptyTexture;
  sf::Time          m_elapsedTime;
  sf::Time          m_frameDuration;

  public:
  Animation();
  Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed = 1, int layer = -1, bool repeat = false);
  
  Animation(const Animation& animation);
  Animation& operator=(const Animation& animation);

  void update(sf::Time deltaTime);
  void update();
  void reset();
  const bool isFinished() const;

  const std::string&  getName() const;
  sf::Sprite&         getSprite();
  const Vec2&         getSize() const;
  unsigned short      getLayer();
};
