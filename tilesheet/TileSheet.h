#pragma once

#include <boost/dynamic_bitset.hpp>

#include "types/Color.h"
#include "types/Vec2.h"

// Forward declarations
class Color;

/// Sheet that contains a group of tiles on one texture.
class TileSheet
{
public:
  TileSheet(UintVec2 tileSize, UintVec2 textureSize);

  virtual ~TileSheet();

  /// Load a tile collection from disk and find a place to put them.
  /// @return The location that the tiles were placed on the sheet.
  UintVec2 loadCollection(FileName const& filename);

  /// Get a particular tile from the sheet.
  /// @warning Assumes tile is within the bounds of the loaded texture,
  ///          unless DEBUG is defined, in order to save execution time.
  sf::IntRect getTile(UintVec2 tile) const;

  /// Get a reference to the texture to render.
  sf::Texture& getTexture(void);

  /// Get a reference to the tilesheet image.
  sf::Image& getImage(void);

  /// Add vertices to the requested vertex array.
  /// This method maps the requested tile onto a quadrilateral specified
  /// by the four coordinate pairs passed in.  This allows for pseudo-3D
  /// walls to be drawn.
  /// @param vertices Vertex array to add to.
  /// @param tileCoords Coordinates of the tile to render.
  /// @param bgColor Color to blend with (for lighting).
  /// @param ulCoord Upper-left coordinates.
  /// @param urCoord Upper-right coordinates.
  /// @param llCoord Lower-left coordinates.
  /// @param lrCoord Lower-right coordinates.
  void addQuad(sf::VertexArray& vertices,
                UintVec2 tileCoords, Color bgColor,
                RealVec2 ulCoord, RealVec2 urCoord,
                RealVec2 llCoord, RealVec2 lrCoord);

  /// Add a quad with colors specified in a 3x3 grid.
  void addGradientQuadTo(sf::VertexArray& vertices,
                         UintVec2 tileCoords,
                         RealVec2 coordNW, RealVec2 coordNE,
                         RealVec2 coordSW, RealVec2 coordSE,
                         Color colorOverall,
                         Color colorNW, Color colorN, Color colorNE,
                         Color colorW, Color colorC, Color colorE,
                         Color colorSW, Color colorS, Color colorSE);

  /// Add outline vertices to the requested vertex array.
  /// This method draws a hollow quadrilateral in the color specified by
  /// bgColor.
  /// @param vertices Vertex array to add to.
  /// @param bgColor Color to blend with (for lighting).
  /// @param ulCoord Upper-left coordinates.
  /// @param urCoord Upper-right coordinates.
  /// @param lrCoord Lower-right coordinates.
  /// @param llCoord Lower-left coordinates.
  void addOutlineVertices(sf::VertexArray& vertices,
                            Color bgColor,
                            RealVec2 ulCoord,
                            RealVec2 urCoord,
                            RealVec2 lrCoord,
                            RealVec2 llCoord);

protected:
  /// Return bitset index based on coordinates.
  unsigned int getIndex(UintVec2 coords);

  /// Return true if the requested area is totally unused.
  bool areaIsUnused(UintVec2 start, UintVec2 size);

  /// Find the first free tile area.
  /// @param size Size of the area to search for, IN TILES.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  UintVec2 findUnusedArea(UintVec2 size);

  /// Mark a rectangle of tiles as being used.
  /// @param upper_left_corner  Upper-left corner of rectangle.
  /// @param size               Size of the rectangle to mark.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  void markTilesUsed(UintVec2 upper_left_corner, UintVec2 size);


private:
  sf::Texture m_texture;
  sf::Image m_image;  // required for stuff like SFGUI that can't access textures directly
  UintVec2 m_textureSize;
  UintVec2 m_tileSize;
  boost::dynamic_bitset<size_t> m_used; // size_t gets rid of 64-bit compile warning

};
