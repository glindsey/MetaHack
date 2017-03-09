#include "stdafx.h"

#include "MapTileNullView.h"

std::string MapTileNullView::getViewName()
{
  return "null";
}

MapTileNullView::MapTileNullView(MapTile& map_tile)
  :
  MapTileView(map_tile)
{
}
