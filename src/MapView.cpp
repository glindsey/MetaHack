#include "stdafx.h"

#include "MapView.h"

#include "GameState.h"

MapView::MapView(MapId map_id)
	:
	m_map_id(map_id)
{}

Map& MapView::get_map()
{
	return GAME.get_maps().get(m_map_id);
}