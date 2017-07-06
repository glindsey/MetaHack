#include "stdafx.h"

#include "views/MapTileNullView.h"

std::string MapTileNullView::getViewName()
{
  return "null";
}

MapTileNullView::MapTileNullView(MapTile& map_tile)
  :
  MapTileView(map_tile)
{
}

bool MapTileNullView::onEvent(Event const & event)
{
  return false;
}
