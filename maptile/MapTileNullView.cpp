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

EventResult MapTileNullView::onEvent_NVI(Event const & event)
{
  return{ EventHandled::Yes, ContinueBroadcasting::Yes };
}
