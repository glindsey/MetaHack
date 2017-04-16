#include "stdafx.h"

#include "entity/EntityId.h"

#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "game/GameState.h"

Entity* EntityId::operator->()
{
  return &(GAME.getEntities().get(m_id));
}

Entity const* EntityId::operator->() const
{
  return &(GAME.getEntities().get(m_id));
}

void to_json(json& j, EntityId const& id)
{
  j = id.m_id;
}

void from_json(json const& j, EntityId& id)
{
  id.m_id = j.get<uint64_t>();
}