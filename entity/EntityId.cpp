#include "stdafx.h"

#include "game/GameState.h"
#include "entity/EntityId.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

Entity* EntityId::operator->()
{
  return &(GAME.get_entities().get(m_id));
}

Entity const* EntityId::operator->() const
{
  return &(GAME.get_entities().get(m_id));
}