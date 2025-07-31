#include "Weapon.h"
#include <algorithm>
#include <utility>

Weapon::Weapon() 
  :m_name("Nane"), m_comboTimer( sf::seconds(0.0f)), m_comboNumber(0),
   m_damage(0) {}
  


Weapon::Weapon(const std::string& weaponName, sf::Time&& comboTimer,
               size_t comboNumber, Vec2& weaponBBox, unsigned short damage)
  :m_name(weaponName), m_comboTimer(comboTimer),
  m_comboNumber(comboNumber), m_weaponBBox(weaponBBox),
  m_damage(damage)  
{}

Weapon::Weapon(const Weapon& otherWeapon)
  :m_name(otherWeapon.m_name), m_comboTimer(otherWeapon.m_comboTimer),
  m_comboNumber(otherWeapon.m_comboNumber), m_weaponBBox(otherWeapon.m_weaponBBox),
  m_damage(otherWeapon.m_damage) {}

Weapon::Weapon(Weapon&& otherWeapon)
{
  m_name = std::move(otherWeapon.m_name);
  m_comboTimer = std::move(otherWeapon.m_comboTimer);
  m_comboNumber = otherWeapon.m_comboNumber;
  m_weaponBBox = std::move(otherWeapon.m_weaponBBox);
  m_damage = otherWeapon.m_damage; 
}

Weapon& Weapon::operator=(const Weapon& otherWeapon)
{
  if(this != &otherWeapon)
  {
    m_name = otherWeapon.m_name;
    m_comboTimer = otherWeapon.m_comboTimer;
    m_comboNumber = otherWeapon.m_comboNumber;
    m_weaponBBox = otherWeapon.m_weaponBBox;
    m_damage = otherWeapon.m_damage; 
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
    m_weaponBBox = std::move(otherWeapon.m_weaponBBox);
    m_damage = otherWeapon.m_damage; 
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
const Vec2&  Weapon::getWeaponBBox() const
{
  return m_weaponBBox;
}

const unsigned short Weapon::getDamage()   const
{
  return m_damage;
}
