#pragma once

#include <cstddef>
#include <vector>
#include "Component.h"
#include "Enums.h"

class Entity;

#define MAX_ENTITIES 10000

using EntityComponentVectorTuple = std::tuple<std::vector<CTransform>,
  std::vector<CAnimation>,std::vector<CActive>, std::vector<CTag>,
  std::vector<CInput>, std::vector<CGravity>,std::vector<CBoundingBox>,
  std::vector<CState>, std::vector<CAttack>, std::vector<CWeapon>>;

class EntityMemoryPool
{
  EntityComponentVectorTuple  m_pool;

  EntityMemoryPool(size_t maxEntities);
public:

  static  EntityMemoryPool& Instance()
  {
    static EntityMemoryPool pool(MAX_ENTITIES);
    return pool;
  }
  
 template<typename T>
  T& getComponent(size_t entityID)
  {
    return std::get<std::vector<T>>(m_pool)[entityID];
  }

  
  template<typename T, typename... TArgs>
  T& addComponent(size_t entityID, TArgs&&... mArgs)
  {
    auto& component = EntityMemoryPool::Instance().getComponent<T>(entityID);
    component = T(std::forward<TArgs>(mArgs)...);
    component.has = true;
    return component;
  }


  size_t                    addEntity(Object tag);
  size_t                    getEntity(Object tag); 
  size_t                    getFreeEntityIndex();
};
