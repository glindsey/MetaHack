#pragma once

#include <SFML/Graphics.hpp>

#include "systems/SystemLighting.h"
#include "tilesheet/TileSheet.h"
#include "types/Grid2D.h"
#include "views/MapView.h"

// Forward declarations
class MapTileFancyAsciiView;

/// Class representing the pseudo-3D ASCII view of a Map object.
class MapFancyAsciiView : public MapView
{
  friend class FancyAsciiGraphicViews;

public:
  virtual bool renderMap(sf::RenderTexture& texture, int frame) override;

  virtual void updateTiles(EntityId viewer, Systems::Lighting& lighting) override;
  virtual void updateEntities(EntityId viewer, Systems::Lighting& lighting, int frame) override;

  virtual void drawHighlight(sf::RenderTarget& target,
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
  MapFancyAsciiView(metagui::Desktop& desktop,
                    std::string name, 
                    Map& map, 
                    UintVec2 size, 
                    FancyAsciiGraphicViews& views);

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
  std::unique_ptr< Grid2D< MapTileFancyAsciiView > > m_map_tile_views;

  /// Reference to parent views object.
  FancyAsciiGraphicViews& m_views;
};