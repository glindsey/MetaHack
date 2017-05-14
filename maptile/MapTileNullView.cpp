#include "stdafx.h"

#include "maptile/MapTileNullView.h"

std::string MapTileNullView::getViewName()
{
  return "null";
}

MapTileNullView::MapTileNullView(MapTile& map_tile)
  :
  MapTileView(map_tile)
{
}

bool MapTileNullView::onEvent_NVI(Event const & event)
{
  return false;
}
