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
  friend class Standard2DGraphicViews;

public:
  virtual void update_tiles(EntityId viewer) override;
  virtual void update_things(EntityId viewer, int frame) override;
  
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual void draw_highlight(sf::RenderTarget& target,
                              Vec2f location,
                              sf::Color fgColor,
                              sf::Color bgColor,
                              int frame) override;

protected:
  /// Constructor.
  /// Private because map views should be obtained via the IGraphicViews service.
  /// @param map	    Reference to Map object to associate with this view.
  /// @param tile_sheet Reference to tilesheet containing graphics to display.
  MapStandard2DView(Map& map, TileSheet& tile_sheet);

  /// Reinitialize cached map render data.
  void reset_cached_render_data();

  virtual void notifyOfEvent_(Observable& observed, Event event) override;


private:

  /// "Seen" map vertex array.
  sf::VertexArray m_map_seen_vertices;

  /// "Memory" map vertex array.
  sf::VertexArray m_map_memory_vertices;

  /// Entity vertex array.
  sf::VertexArray m_thing_vertices;

  /// Grid of tile views.
  std::unique_ptr< Grid2D< MapTileStandard2DView > > m_map_tile_views;

  /// Reference to tilesheet containing tiles/entities.
  TileSheet& m_tile_sheet;
};