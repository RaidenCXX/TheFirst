#pragma once

#include "SFML/System/Time.hpp"
#include <string>


class Weapon
{
  std::string m_name;
  sf::Time    m_comboTimer;
  size_t      m_comboNumber;

public:

  Weapon();
  Weapon(const std::string& weaponName, sf::Time&& comboTimer, size_t comboNumber);
  Weapon(const Weapon& otherWeapon);
  Weapon(Weapon&& otherWeapon);
  
  Weapon& operator=(const Weapon& otherWeapon);
  Weapon& operator=(Weapon&& otherWeapon);

  const std::string getWeaponName()   const;
  const sf::Time&    getComboTimer()  const;
  const size_t      getNumberCombo()  const;
};
