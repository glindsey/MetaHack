#include "stdafx.h"

#include "GameState.h"
#include "EntityId.h"
#include "Entity.h"
#include "EntityPool.h"

Entity* EntityId::operator->()
{
  return &(GAME.get_things().get(m_id));
}

Entity const* EntityId::operator->() const
{
  return &(GAME.get_things().get(m_id));
}