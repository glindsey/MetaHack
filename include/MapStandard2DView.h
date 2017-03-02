#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "Grid2D.h"
#include "MapView.h"

// Forward declarations
class MapTileStandard2DView;

/// Class representing the standard 2D (pseudo-3D) view of a Map object.
class MapStandard2DView : public MapView
{
public:
  /// Constructor.
  /// @param map	Reference to Map object to associate with this view.
  MapStandard2DView(Map& map);

  virtual void update_tiles(ThingId viewer) override;
  virtual void update_things(ThingId viewer, int frame) override;
  
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual void draw_highlight(sf::RenderTarget& target,
                              Vec2f location,
                              sf::Color fgColor,
                              sf::Color bgColor,
                              int frame) override;
protected:
  /// Reinitialize cached map render data.
  virtual void reset_cached_render_data() override;

  virtual void notifyOfEvent_(Observable& observed, Event event) override;


private:

  /// "Seen" map vertex array.
  sf::VertexArray m_map_seen_vertices;

  /// "Memory" map vertex array.
  sf::VertexArray m_map_memory_vertices;

  /// Thing vertex array.
  sf::VertexArray m_thing_vertices;

  /// Grid of tile views.
  std::unique_ptr< Grid2D< MapTileStandard2DView > > m_map_tile_views;
};