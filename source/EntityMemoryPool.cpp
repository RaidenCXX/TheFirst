#include "EntityMemoryPool.h"
#include "Enums.h"

EntityMemoryPool::EntityMemoryPool(size_t maxEntities)
{
  std::get<0>(m_pool).resize(maxEntities);  //Transform
  std::get<1>(m_pool).resize(maxEntities);  //CAnimation
  std::get<2>(m_pool).resize(maxEntities);  //CActive
  std::get<3>(m_pool).resize(maxEntities);  //CTag
  std::get<4>(m_pool).resize(maxEntities);  //CInput
  std::get<5>(m_pool).resize(maxEntities);  //CGravity
  std::get<6>(m_pool).resize(maxEntities);  //CBoundingBox 
  std::get<7>(m_pool).resize(maxEntities);  //CState 
  std::get<8>(m_pool).resize(maxEntities);  //CAttack 
  std::get<9>(m_pool).resize(maxEntities);  //CWeapon
}


size_t EntityMemoryPool::addEntity(Object tag)
{
  size_t index = getFreeEntityIndex();
  
  getComponent<CTag>(index).tag = tag;
  getComponent<CActive>(index).active = true;

  return index;
}

size_t EntityMemoryPool::getFreeEntityIndex()
{

  for(size_t i = 0; i < std::get<2>(m_pool).size(); i++)
  {
    if(std::get<2>(m_pool)[i].active == false)
    {
      return i;
    }
  }
  return std::get<2>(m_pool).size();
}


size_t  EntityMemoryPool::getEntity(Object tag)
{
  size_t i = 0;
  for (auto& t : std::get<3>(m_pool))
  {
    if(tag == t.tag)
    {
      break;
    }
    i++;
  }
  return i;
}
