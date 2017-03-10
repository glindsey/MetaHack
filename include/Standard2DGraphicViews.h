#pragma once

#include <deque>

#include "IGraphicViews.h"

/// Forward declarations
class TileSheet;

/// Implementation of the standard 2D graphic views service.
class Standard2DGraphicViews : public IGraphicViews
{
  friend class EntityStandard2DView;
  friend class MapTileStandard2DView;
  friend class MapStandard2DView;

public:
  Standard2DGraphicViews();

  virtual ~Standard2DGraphicViews();

  virtual EntityView* createEntityView(Entity& entity) override;

  virtual MapTileView* createMapTileView(MapTile& map_tile) override;

  virtual MapView* createMapView(std::string name, Map& map, Vec2u size) override;

  virtual void loadViewResourcesFor(Metadata& metadata) override;

protected:
  TileSheet& getTileSheet();

private:
  /// The tilesheet that these views are drawn from.
  std::unique_ptr<TileSheet> m_tile_sheet;
};