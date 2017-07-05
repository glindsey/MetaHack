#pragma once

#include <deque>

#include "services/IGraphicViews.h"

#include "entity/EntityNullView.h"
#include "map/MapNullView.h"
#include "maptile/MapTileNullView.h"
#include "utilities/New.h"

/// Implementation of the standard 2D (pseudo-3D) graphic views service.
class NullGraphicViews : public IGraphicViews
{
public:
  NullGraphicViews() {}

  virtual ~NullGraphicViews() {}

  /// Return an Entity view.
  virtual EntityView* createEntityView(EntityId entity) override
  {
    return NEW EntityNullView(entity);
  }

  /// Return a MapTile view.
  virtual MapTileView* createMapTileView(MapTile& map_tile) override
  {
    return NEW MapTileNullView(map_tile);
  }

  /// Return a Map view.
  virtual MapView* createMapView(metagui::Desktop& desktop, std::string name, Map& map, UintVec2 size) override
  {
    return NEW MapNullView(desktop, name, map, size);
  }

  /// Load any required view resources associated with the specified category.
  virtual void loadViewResourcesFor(std::string category) override
  {
  }

};