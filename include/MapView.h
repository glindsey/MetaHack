#pragma once

#include "stdafx.h"

#include "Map.h"
#include "Renderable.h"

// Forward declarations
class ThingId;

/// Abstract cLass representing a view of a Map object.
class MapView : public RenderableToTexture
{
public:
  /// Constructor.
  MapView();

  /// Constructor.
  /// @param map	ID of Map object to associate with this view.
  explicit MapView(MapId map_id);

  /// Destructor.
  virtual ~MapView();

  /// Sets the current map ID.
  /// If new map ID is different from the current map ID,
  /// this method may reinitialize cached render data.
  /// @param map_id ID of the map to use.
  void set_map_id(MapId map_id);

  /// Gets the current map ID.
  /// @return The ID of the map being viewed.
  MapId get_map_id();

  /// Sets map view information.
  /// @param center The place to center the rendered map on.
  /// @param zoom_level The zoom level.
  /// @todo Not sure this actually belongs here. It seems more specific to
  ///       AppStateGameMode itself.
  void set_view(sf::RenderTarget& target,
                sf::Vector2f center,
                float zoom_level);

  /// Update any cached render data associated with map tiles.
  /// @param thing	ID of the thing that is percieving the map.
  virtual void update_tiles(ThingId thing) = 0;

  /// Update any cached render data associated with map things.
  /// @param thing	ID of the thing that is percieving the map.
  /// @param frame	Current animation frame number.
  virtual void update_things(ThingId thing, int frame) = 0;

protected:
  /// Get reference to Map associated with this view.
  Map& get_map();

  /// Reinitialize cached map render data.
  virtual void reset_cached_render_data() = 0;

private:
  /// Map ID associated with this view.
  MapId m_map_id;
};