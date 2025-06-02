#include "EntityManager.h"
#include "EntityMemoryPool.h"
#include "Enums.h"
#include <vector>


EntityManager::EntityManager()
  :m_numEntities(0), m_player(-1)
{
  m_entities.resize(MAX_ENTITIES);

  size_t count = 0;
  for (Entity& e : getEntities())
  {
    e.m_id = count;
    count++;
  }
}

Entity& EntityManager::addEntity(Object tag)
{
  size_t index = EntityMemoryPool::Instance().addEntity(tag);
  
  m_entities[index].m_id = index;


  if (EntityMemoryPool::Instance().getComponent<CTag>(index).tag == Object::Player)
  {
    m_player = index;
  }
  ++m_numEntities;


  return m_entities[index];
}

Entity& EntityManager::getEntity(Object tag)
{
  size_t index = EntityMemoryPool::Instance().getEntity(tag);
  return m_entities[index];
}

Entity& EntityManager::getEntity(const size_t index)
{
  return m_entities[index];
}


std::vector<Entity>& EntityManager::getEntities()
{
  return m_entities;
}


short EntityManager::getPlayerIndex()
{
  return m_player;
}

size_t  EntityManager::getNumActiveEntities()
{
  return m_numEntities;
}


void   EntityManager::destroyEntity(Entity& entity)
{
  entity.getComponent<CActive>().active = false;
}
