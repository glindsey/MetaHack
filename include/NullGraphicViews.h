#pragma once

#include <deque>

#include "IGraphicViews.h"

#include "EntityNullView.h"
#include "MapNullView.h"
#include "MapTileNullView.h"
#include "New.h"

/// Implementation of the standard 2D (pseudo-3D) graphic views service.
class NullGraphicViews : public IGraphicViews
{
public:
  NullGraphicViews() {}

  virtual ~NullGraphicViews() {}

  /// Return an Entity view.
  virtual EntityView* createEntityView(Entity& entity) override
  {
    return NEW EntityNullView(entity);
  }

  /// Return a MapTile view.
  virtual MapTileView* createMapTileView(MapTile& map_tile) override
  {
    return NEW MapTileNullView(map_tile);
  }

  /// Return a Map view.
  virtual MapView* createMapView(Map& map) override
  {
    return NEW MapNullView(map);
  }

};