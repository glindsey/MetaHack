#pragma once

#include <deque>

#include "IGraphicViews.h"

/// Implementation of the standard 2D graphic views service.
class Standard2DGraphicViews : public IGraphicViews
{
public:
  Standard2DGraphicViews();

  virtual ~Standard2DGraphicViews();

  virtual EntityView* createEntityView(Entity& entity) override;

  virtual MapTileView* createMapTileView(MapTile& map_tile) override;

  virtual MapView* createMapView(Map& map) override;

};