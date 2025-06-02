#include "Weapon.h"
#include <utility>

Weapon::Weapon() 
  :m_name("Nane"), m_comboTimer( sf::seconds(0.0f)), m_comboNumber(0) {}
  


Weapon::Weapon(const std::string& weaponName, sf::Time&& comboTimer, size_t comboNumber)
  :m_name(weaponName), m_comboTimer(comboTimer), m_comboNumber(comboNumber) {}

Weapon::Weapon(const Weapon& otherWeapon)
  :m_name(otherWeapon.m_name), m_comboTimer(otherWeapon.m_comboTimer),
  m_comboNumber(otherWeapon.m_comboNumber) {}

Weapon::Weapon(Weapon&& otherWeapon)
{
  m_name = std::move(otherWeapon.m_name);
  m_comboTimer = std::move(otherWeapon.m_comboTimer);
  m_comboNumber = otherWeapon.m_comboNumber;
}

Weapon& Weapon::operator=(const Weapon& otherWeapon)
{
  if(this != &otherWeapon)
  {
    m_name = otherWeapon.m_name;
    m_comboTimer = otherWeapon.m_comboTimer;
    m_comboNumber = otherWeapon.m_comboNumber;
  }
  return *this;
}

Weapon& Weapon::operator=(Weapon&& otherWeapon)
{
  if(this != &otherWeapon)
  {
    m_name = std::move(otherWeapon.m_name);
    m_comboTimer = std::move(otherWeapon.m_comboTimer);
    m_comboNumber = otherWeapon.m_comboNumber;
  }
  return *this;
}

const std::string Weapon::getWeaponName() const
{
  return m_name;
}

const sf::Time& Weapon::getComboTimer()   const
{
  return m_comboTimer;
}

const size_t Weapon::getNumberCombo()  const
{
  return m_comboNumber;
}
