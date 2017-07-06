#pragma once

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "services/IGraphicViews.h"

/// Forward declarations
class TileSheet;

/// Using declarations
using TileCoordsMap = std::unordered_map<std::string, UintVec2>;

/// Implementation of the fancy ASCII graphic views service.
class FancyAsciiGraphicViews : public IGraphicViews
{
  friend class EntityFancyAsciiView;
  friend class MapTileFancyAsciiView;
  friend class MapFancyAsciiView;

public:
  FancyAsciiGraphicViews();

  virtual ~FancyAsciiGraphicViews();

  virtual EntityView* createEntityView(EntityId entity) override;

  virtual MapTileView* createMapTileView(MapTile& map_tile) override;

  virtual MapView* createMapView(metagui::Desktop& desktop, 
                                 std::string name, 
                                 Map& map, 
                                 UintVec2 size) override;

  virtual void loadViewResourcesFor(std::string category) override;

protected:
  TileSheet& getTileSheet();
  bool hasTilesFor(std::string category);
  UintVec2 const& getTileSheetCoords(std::string category);

private:
  /// A map associating category names with tilesheet coordinates.
  std::unordered_map<std::string, UintVec2> m_tileCoords;

  /// A set indicating which categories we've already tried to load.
  std::unordered_set<std::string> m_triedToLoad;

  /// The tilesheet that these views are drawn from.
  std::unique_ptr<TileSheet> m_tileSheet;
};