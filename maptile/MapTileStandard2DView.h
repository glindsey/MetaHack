#pragma once



#include <SFML/Graphics.hpp>

#include "maptile/MapTileView.h"

#include "entity/EntityStandard2DView.h"
#include "tilesheet/TileSheet.h"

// Forward declarations
namespace Systems
{
  class Lighting;
}

/// Class representing the standard 2D (pseudo-3D) view of a MapTile object.
class MapTileStandard2DView : public MapTileView
{
  friend class MapStandard2DView;
  friend class Standard2DGraphicViews;

public:

  virtual std::string getViewName() override;

protected:
  /// Constructor.
  MapTileStandard2DView(MapTile& mapTile, 
                        Standard2DGraphicViews& views);

  /// Return the coordinates of the tile's floor entity on the tilesheet.
  UintVec2 getFloorTileSheetCoords() const;

  /// Return the coordinates of the tile's space entity on the tilesheet.
  UintVec2 getSpaceTileSheetCoords() const;

  UintVec2 getEntityTileSheetCoords(EntityId entity, int frame) const;

  /// Add the vertices for the maptile to the vertex arrays.
  /// @param viewer Entity that is viewing this tile.
  /// @param horizVertices Array to add horizontal surface vertices to.
  /// @param vertVertices Array to add vertical surface vertices to.
  /// @param memoryVertices Array to add memory vertices to.
  void addTileVertices(EntityId viewer, 
                       sf::VertexArray& horizVertices,
                       sf::VertexArray& vertVertices,
                       sf::VertexArray& memoryVertices,
                       Systems::Lighting& lighting);

  /// Add the vertices from the viewer's memory.
  /// @param vertices Array to add vertices to.
  /// @param viewer Entity that is remembering this tile.
  void addMemoryVerticesTo(sf::VertexArray& vertices,
                           EntityId viewer);

  /// Add the horizontal surface vertices for the MapTile to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  void addHorizontalSurfaceVerticesTo(sf::VertexArray& vertices,
                                      EntityId viewer,
                                      Systems::Lighting& lighting);

  /// Add the vertices for the entities on this MapTile to a VertexArray to be drawn.
  void addEntitiesVertices(EntityId viewer,
                           sf::VertexArray& vertices,
                           Systems::Lighting* lighting,
                           int frame);

  /// Add the vertices for the entity specified.
  /// @todo Move into a Entity view.
  void addEntityVertices(EntityId entity, 
                         sf::VertexArray& vertices,
                         Systems::Lighting* lighting,
                         int frame);

  /// Add the vertical surface vertices for this MapTile to a VertexArray to be drawn.
  /// @param vertices     Array to add vertices to.
  /// @param viewer       Viewer of this tile.
  /// @param lighting     Pointer to lighting system, or nullptr if
  ///                     no lighting is to be used.
  void addVerticalSurfaceVerticesTo(sf::VertexArray& vertices,
                                    EntityId viewer,
                                    Systems::Lighting* lighting);

  virtual bool onEvent(Event const& event) override;

private:
  /// Random tile offset.
  int m_tileOffset;

  /// Reference to parent views object.
  Standard2DGraphicViews& m_views;
};