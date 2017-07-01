#pragma once

#include <SFML/Graphics.hpp>

#include "types/Grid2D.h"
#include "map/MapView.h"
#include "systems/SystemLighting.h"
#include "tilesheet/TileSheet.h"

// Forward declarations
class MapTileStandard2DView;

/// Class representing the standard 2D (pseudo-3D) view of a Map object.
class MapStandard2DView : public MapView
{
  friend class Standard2DGraphicViews;

public:
  virtual bool render_map(sf::RenderTexture& texture, int frame) override;

  virtual void updateTiles(EntityId viewer, Systems::Lighting& lighting) override;
  virtual void updateEntities(EntityId viewer, Systems::Lighting& lighting, int frame) override;

  virtual void draw_highlight(sf::RenderTarget& target,
                              RealVec2 location,
                              Color fgColor,
                              Color bgColor,
                              int frame) override;

  virtual std::string getViewName() override;

protected:
  /// Constructor.
  /// Private because map views should be obtained via the IGraphicViews service.
  /// @param name       Name of this view.
  /// @param map	    Reference to Map object to associate with this view.
  /// @param size       Size of the view to create.
  /// @param tile_sheet Reference to tilesheet containing graphics to display.
  MapStandard2DView(std::string name, 
                    Map& map, 
                    UintVec2 size, 
                    Standard2DGraphicViews& views);

  /// Called before rendering the object's children.
  /// Default behavior is to do nothing.
  virtual void drawPreChildren_(sf::RenderTexture& texture, int frame) override;

  /// Reinitialize cached map render data.
  void resetCachedRenderData();

private:

  /// "Horizontal" (floor/ceiling) map vertex array.
  sf::VertexArray m_mapHorizVertices;

  /// "Vertical" (wall) map vertex array.
  sf::VertexArray m_mapVertVertices;

  /// "Memory" map vertex array.
  sf::VertexArray m_mapMemoryVertices;

  /// Entity vertex array.
  sf::VertexArray m_entityVertices;

  /// Grid of tile views.
  std::unique_ptr< Grid2D< MapTileStandard2DView > > m_map_tile_views;

  /// Reference to parent views object.
  Standard2DGraphicViews& m_views;
};