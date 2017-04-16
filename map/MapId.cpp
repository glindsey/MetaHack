#include "stdafx.h"

#include "map/MapId.h"

#include "game/GameState.h"
#include "map/Map.h"
#include "map/MapFactory.h"

Map* MapId::operator->()
{
  return &(GAME.maps().get(m_id));
}

Map const* MapId::operator->() const
{
  return &(GAME.maps().get(m_id));
}

void to_json(json& j, MapId const& id)
{
  j = id.m_id;
}

void from_json(json const& j, MapId& id)
{
  id.m_id = j.get<uint32_t>();
}