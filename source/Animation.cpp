#include "Animation.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"
#include <cmath>
#include <cstddef>
  

#define ONE_FRAME 1

Animation::Animation()
  :m_emptyTexture(sf::Vector2u{1,1}), m_sprite(m_emptyTexture)
{}

Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed, int layer, bool repeat)
:m_name(name), m_sprite(texture), m_frameCount(frameCount), m_speed(speed), m_layer(layer), m_repeat(repeat), m_finished(false)
{
  m_size.x = std::floor(m_sprite.getTexture().getSize().x / frameCount);
  m_size.y = m_sprite.getTexture().getSize().y;
  m_sprite.setTextureRect(sf::IntRect{sf::Vector2<int>{0,0}, sf::Vector2<int>{static_cast<int>(m_size.x), static_cast<int>(m_size.y)}});
  m_sprite.setOrigin(sf::Vector2<float>(m_size.x / 2, m_size.y / 2));

  m_frameDuration = sf::seconds(1.0f / static_cast<float>(speed));

  m_elapsedTime = sf::Time::Zero;
  m_animFrame = 0;
}


Animation::Animation(const Animation& animation)
  :m_sprite(animation.m_sprite), m_name(animation.m_name), m_size(animation.m_size), m_speed(animation.m_speed),
   m_frameCount(animation.m_frameCount), m_curentFrame(animation.m_curentFrame), m_layer(animation.m_layer),
   m_repeat(animation.m_repeat), m_frameDuration(animation.m_frameDuration), m_elapsedTime(animation.m_elapsedTime),
   m_animFrame(animation.m_animFrame), m_finished(false)
 
{}

Animation& Animation::operator=(const Animation& animation)
{
  m_sprite      = animation.m_sprite;
  m_name        = animation.m_name;
  m_size        = animation.m_size;
  m_speed       = animation.m_speed;
  m_frameCount  = animation.m_frameCount;
  m_curentFrame = animation.m_curentFrame;
  m_layer       = animation.m_layer;
  m_repeat      = animation.m_repeat;
  m_frameDuration = animation.m_frameDuration;
  m_elapsedTime   = animation.m_elapsedTime;
  m_animFrame     = animation.m_animFrame;
  m_finished      = false;
  return *this;
}

void Animation::update(sf::Time deltaTime)
{
  if (m_frameCount == 1) return;

  m_elapsedTime += deltaTime;

  while (m_elapsedTime >= m_frameDuration)
  {
    m_elapsedTime -= m_frameDuration;

    m_animFrame++;

    if (m_animFrame >= m_frameCount)
    {
      if (m_repeat)
        m_animFrame = 0;
      else
      {
        m_animFrame = m_frameCount - 1;
        m_finished = true;
      } 
    }

    sf::IntRect rect(
      sf::Vector2i(static_cast<int>(m_animFrame * m_size.x), 0),
      sf::Vector2i(static_cast<int>(m_size.x), static_cast<int>(m_size.y))
    );

    m_sprite.setTextureRect(rect);

  }
}

void Animation::update()
{
  if (m_frameCount == 1) return;

    m_animFrame++;

    if (m_animFrame >= m_frameCount)
    {
        m_animFrame = 1;
    }

    sf::IntRect rect(
      sf::Vector2i(static_cast<int>(m_animFrame * m_size.x), 0),
      sf::Vector2i(static_cast<int>(m_size.x), static_cast<int>(m_size.y))
    );

    m_sprite.setTextureRect(rect);

}
void Animation::reset()
{
  m_animFrame = 0;
  m_elapsedTime = sf::Time::Zero;
  m_finished = false;

  sf::IntRect rect(
    sf::Vector2i(0, 0),
    sf::Vector2i(static_cast<int>(m_size.x), static_cast<int>(m_size.y))
  );

  m_sprite.setTextureRect(rect);
}


const bool Animation::isFinished() const
{
  return m_finished;
}



const std::string&  Animation::getName()   const
{
  return m_name;
}

sf::Sprite&   Animation::getSprite()
{
  return m_sprite;
}

const Vec2&         Animation::getSize()   const
{
  return m_size;
}



unsigned short Animation::getLayer()
{
  return m_layer;
}
