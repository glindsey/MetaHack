#include "stdafx.h"

#include "MapTileView.h"

#include "App.h"
#include "GameState.h"
#include "IConfigSettings.h"
#include "Service.h"

MapTileView::MapTileView(MapTile& map_tile)
  :
  m_map_tile(map_tile)
{
  startObserving(map_tile);
}

MapTileView::~MapTileView()
{}

MapTile& MapTileView::get_map_tile()
{
  return m_map_tile;
}

MapTile const& MapTileView::get_map_tile() const
{
  return m_map_tile;
}