#include "stdafx.h"

#include "New.h"
#include "Standard2DGraphicViews.h"

#include "EntityStandard2DView.h"
#include "MapTileStandard2DView.h"
#include "MapStandard2DView.h"

Standard2DGraphicViews::Standard2DGraphicViews()
{}

Standard2DGraphicViews::~Standard2DGraphicViews()
{}

EntityView* Standard2DGraphicViews::createEntityView(Entity& entity)
{
  return NEW EntityStandard2DView(entity);
}

MapTileView* Standard2DGraphicViews::createMapTileView(MapTile& map_tile)
{
  return NEW MapTileStandard2DView(map_tile);
}

MapView* Standard2DGraphicViews::createMapView(Map& map)
{
  return NEW MapStandard2DView(map);
}
