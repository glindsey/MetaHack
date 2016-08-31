#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "MapView.h"

// Forward declarations

/// Class representing the standard 2D (pseudo-3D) view of a Map object.
class MapStandard2DView : public MapView
{
public:
  /// Constructor.
  MapStandard2DView();

  /// Constructor.
  /// @param map	ID of Map object to associate with this view.
  MapStandard2DView(MapId map_id);

  virtual void update_tiles(ThingId viewer) override;
  virtual void update_things(ThingId viewer, int frame) override;
  
  virtual bool render(sf::RenderTexture& texture, int frame) override;

protected:
  /// Reinitialize cached map render data.
  virtual void reset_cached_render_data() override;

  /// Add the vertices for the maptile located at the coordinates specified.
  void add_tile_vertices(ThingId viewer, Vec2i coords);

  /// Add the floor vertices for the maptile located at the coordinates specified.
  void add_tile_floor_vertices(Vec2i coords);

  /// Add the wall vertices for the maptile located at the coordinates specified.
  void add_tile_wall_vertices(Vec2i coords);

  /// Add the floor vertices for the thing specified.
  void add_thing_floor_vertices(ThingId thing, bool use_lighting, int frame);

private:

  /// "Seen" map vertex array.
  sf::VertexArray m_map_seen_vertices;

  /// "Memory" map vertex array.
  sf::VertexArray m_map_memory_vertices;

  /// Thing vertex array.
  sf::VertexArray m_thing_vertices;
};