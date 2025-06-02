#pragma once

#include <vector>
#include "Entity.h"
#include "Enums.h"

class EntityManager
{

  std::vector<Entity> m_entities;
  size_t              m_numEntities;
  short               m_player = -1;
  

public:
  EntityManager();
  
  Entity&               addEntity(Object tag);
  Entity&               getEntity(Object tag);
  Entity&               getEntity(const size_t index);
  std::vector<Entity>&  getEntities();
  short                 getPlayerIndex();
  size_t                getNumActiveEntities();
  void                  destroyEntity(Entity& entity);
  

};
