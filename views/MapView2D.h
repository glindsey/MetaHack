#pragma once

#include <SFML/Graphics.hpp>

#include "systems/SystemLighting.h"
#include "tilesheet/TileSheet.h"
#include "types/Grid2D.h"
#include "views/MapView.h"

// Forward declarations
class MapTileView2D;

/// Class representing the standard 2D (pseudo-3D) view of a Map object.
class MapView2D : public MapView
{
public:
  /// Constructor.
  /// @param name     Name of this view.
  /// @param map      Reference to Map object to associate with this view.
  MapView2D(std::string name,
            Map& map);

  virtual ~MapView2D();

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
  /// Called before rendering the object's children.
  /// Default behavior is to do nothing.
  virtual void drawPreChildren_(sf::RenderTexture& texture, int frame);

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
  std::unique_ptr< Grid2D< MapTileView2D > > m_map_tile_views;
};
