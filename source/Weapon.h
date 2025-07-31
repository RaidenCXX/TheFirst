#pragma once

#include "SFML/System/Time.hpp"
#include "Vec2.h"
#include <string>


class Weapon
{
  Vec2            m_weaponBBox;
  std::string     m_name;
  sf::Time        m_comboTimer;
  unsigned short  m_damage;
  size_t          m_comboNumber;

public:

  Weapon();
  Weapon(const std::string& weaponName, sf::Time&& comboTimer,
         size_t comboNumber, Vec2& weaponBBox, unsigned short damage);
  Weapon(const Weapon& otherWeapon);
  Weapon(Weapon&& otherWeapon);
  
  Weapon& operator=(const Weapon& otherWeapon);
  Weapon& operator=(Weapon&& otherWeapon);

  const std::string     getWeaponName()   const;
  const sf::Time&       getComboTimer()  const;
  const size_t          getNumberCombo()  const;
  const Vec2&           getWeaponBBox()   const;
  const unsigned short  getDamage()       const;
};
