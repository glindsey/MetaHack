#include "stdafx.h"

#include "MapTileNullView.h"

MapTileNullView::MapTileNullView(MapTile& map_tile)
  :
  MapTileView(map_tile)
{
}

void MapTileNullView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
