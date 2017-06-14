#pragma once



#include <SFML/Graphics.hpp>

#include "maptile/MapTileView.h"

#include "entity/EntityStandard2DView.h"

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
                        TileSheet& tileSheet);

  /// Return the coordinates of the tile on the tilesheet.
  UintVec2 getTileSheetCoords() const;


  UintVec2 getEntityTileSheetCoords(EntityId entity, int frame) const;

  /// Add the vertices for the maptile to the seen and memory vertices.
  /// @param viewer Entity that is viewing this tile.
  /// @param seen_vertices Array to add seen vertices to.
  /// @param memory_vertices Array to add memory vertices to.
  void addTileVertices(EntityId viewer, 
                       sf::VertexArray& seen_vertices,
                       sf::VertexArray& memory_vertices,
                       Systems::Lighting& lighting);

  /// Add the vertices from the viewer's memory.
  /// @param vertices Array to add vertices to.
  /// @param viewer Entity that is remembering this tile.
  void addMemoryVerticesTo(sf::VertexArray & vertices,
                           EntityId viewer);

  /// Add the floor vertices for the maptile to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  void addTileFloorVerticesTo(sf::VertexArray& vertices,
                              Systems::Lighting& lighting);

  /// Add the floor vertices for the entities on this tile to a VertexArray to be drawn.
  void addEntitiesFloorVertices(EntityId viewer,
                                sf::VertexArray& vertices,
                                Systems::Lighting* lighting,
                                int frame);

  /// Add the floor vertices for the entity specified.
  /// @todo Move into a Entity view.
  void addEntityFloorVertices(EntityId entity, 
                                sf::VertexArray& vertices, 
                                Systems::Lighting* lighting,
                                int frame);

  /// Add this MapTile's walls to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  /// @param lighting     Pointer to lighting system, or nullptr if
  ///                     no lighting is to be used.
  /// @param nw_is_empty  Whether tile to the northwest is empty.
  /// @param n_is_empty   Whether tile to the north is empty.
  /// @param ne_is_empty  Whether tile to the northeast is empty.
  /// @param e_is_empty   Whether tile to the east is empty.
  /// @param se_is_empty  Whether tile to the southeast is empty.
  /// @param s_is_empty   Whether tile to the south is empty.
  /// @param sw_is_empty  Whether tile to the southwest is empty.
  /// @param w_is_empty   Whether tile to the west is empty.
  void addWallVerticesTo(sf::VertexArray& vertices,
                         Systems::Lighting* lighting,
                         bool nw_is_empty, bool n_is_empty,
                         bool ne_is_empty, bool e_is_empty,
                         bool se_is_empty, bool s_is_empty,
                         bool sw_is_empty, bool w_is_empty);

  virtual bool onEvent(Event const& event) override;

private:
  /// Random tile offset.
  int m_tileOffset;

  /// Reference to tilesheet containing MapTile graphics.
  TileSheet& m_tileSheet;
};