#pragma once
#include <cstddef>
#include <utility>
#include "EntityMemoryPool.h"

class Entity
{
public:
  size_t   m_id;

  Entity() = default;
  Entity(size_t entityID);

  template<typename T>
  T& getComponent()
  {
    return EntityMemoryPool::Instance().getComponent<T>(m_id);
  }

  template<typename T>
  bool hasComponent() 
  {
    return EntityMemoryPool::Instance().getComponent<T>(m_id).has;
  }
  template<typename T, typename... TArgs>
  T& addComponent(TArgs&&... mArgs)
  {
    T& component = EntityMemoryPool::Instance().addComponent<T>(m_id, std::forward<TArgs>(mArgs)...);
    return component;
  }
};
