#include "stdafx.h"

#include "MapTileNullView.h"

std::string MapTileNullView::get_view_name()
{
  return "null";
}

MapTileNullView::MapTileNullView(MapTile& map_tile)
  :
  MapTileView(map_tile)
{
}

void MapTileNullView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
