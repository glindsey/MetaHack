#pragma once

/// Forward declarations
namespace metagui
{
  class Desktop;
}
class EntityId;
class EntityView;
class Map;
class MapView;
class MapTile;
class MapTileView;

/// Interface for providing graphic views of game objects.
class IGraphicViews
{
public:
  virtual ~IGraphicViews();

  /// Return a Null Object that implements this interface.
  static IGraphicViews* getDefault();

  /// Return an Entity view.
  /// @warning The caller assumes ownership of the created view!
  virtual EntityView* createEntityView(EntityId entity) = 0;

  /// Return a MapTile view.
  /// @warning The caller assumes ownership of the created view!
  virtual MapTileView* createMapTileView(MapTile& map_tile) = 0;

  /// Return a Map view.
  /// @warning The caller assumes ownership of the created view!
  virtual MapView* createMapView(metagui::Desktop& desktop, 
                                 std::string name,
                                 Map& map,
                                 UintVec2 size) = 0;
  
  /// Load any required view resources associated with the specified category.
  virtual void loadViewResourcesFor(std::string category) = 0;

};