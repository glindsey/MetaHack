#include "stdafx.h"

/// Forward declarations
class Entity;
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
  static IGraphicViews* getNull();

  /// Return an Entity view.
  /// @warning The caller assumes ownership of the created view!
  virtual EntityView* createEntityView(Entity& entity) = 0;

  /// Return a MapTile view.
  /// @warning The caller assumes ownership of the created view!
  virtual MapTileView* createMapTileView(MapTile& map_tile) = 0;

  /// Return a Map view.
  /// @warning The caller assumes ownership of the created view!
  virtual MapView* createMapView(Map& map) = 0;
};