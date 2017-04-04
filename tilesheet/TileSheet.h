#ifndef TILESHEET_H
#define TILESHEET_H

#include "stdafx.h"

// Forward declarations
class Color;

/// Sheet that contains a group of tiles on one texture.
class TileSheet
{
public:
  TileSheet(unsigned int tileSize, unsigned int textureSize);

  virtual ~TileSheet();

  /// Load a tile collection from disk and find a place to put them.
  /// @return The location that the tiles were placed on the sheet.
  UintVec2 load_collection(FileName const& filename);

  /// Get a particular tile from the sheet.
  /// @warning Assumes tile is within the bounds of the loaded texture,
  ///          unless DEBUG is defined, in order to save execution time.
  sf::IntRect getTile(UintVec2 tile) const;

  /// Get a reference to the texture to render.
  sf::Texture& getTexture(void);

  /// Add vertices to the requested vertex array.
  /// This method maps the requested tile onto a quadrilateral specified
  /// by the four coordinate pairs passed in.  This allows for pseudo-3D
  /// walls to be drawn.
  /// @param vertices Vertex array to add to.
  /// @param tile_coords Coordinates of the tile to render.
  /// @param bg_color Color to blend with (for lighting).
  /// @param ul_coord Upper-left coordinates.
  /// @param ur_coord Upper-right coordinates.
  /// @param ll_coord Lower-left coordinates.
  /// @param lr_coord Lower-right coordinates.
  void add_quad(sf::VertexArray& vertices,
                UintVec2 tile_coords, Color bg_color,
                RealVec2 ul_coord, RealVec2 ur_coord,
                RealVec2 ll_coord, RealVec2 lr_coord);

  /// Add a quad with colors specified in a 3x3 grid.
  void add_gradient_quad(sf::VertexArray& vertices,
                         UintVec2 tile_coords,
                         RealVec2 coordNW, RealVec2 coordNE,
                         RealVec2 coordSW, RealVec2 coordSE,
                         Color colorNW, Color colorN, Color colorNE,
                         Color colorW, Color colorC, Color colorE,
                         Color colorSW, Color colorS, Color colorSE);

  /// Add outline vertices to the requested vertex array.
  /// This method draws a hollow quadrilateral in the color specified by
  /// bg_color.
  /// @param vertices Vertex array to add to.
  /// @param bg_color Color to blend with (for lighting).
  /// @param ul_coord Upper-left coordinates.
  /// @param ur_coord Upper-right coordinates.
  /// @param lr_coord Lower-right coordinates.
  /// @param ll_coord Lower-left coordinates.
  void add_outline_vertices(sf::VertexArray& vertices,
                            Color bg_color,
                            RealVec2 ul_coord,
                            RealVec2 ur_coord,
                            RealVec2 lr_coord,
                            RealVec2 ll_coord);

protected:
  /// Return bitset index based on coordinates.
  unsigned int getIndex(UintVec2 coords);

  /// Return true if the requested area is totally unused.
  bool area_is_unused(UintVec2 start, UintVec2 size);

  /// Find the first free tile area.
  /// @param size Size of the area to search for, IN TILES.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  UintVec2 find_unused_area(UintVec2 size);

  /// Mark a rectangle of tiles as being used.
  /// @param upper_left_corner  Upper-left corner of rectangle.
  /// @param size               Size of the rectangle to mark.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  void mark_tiles_used(UintVec2 upper_left_corner, UintVec2 size);


private:
  sf::Texture m_texture;
  unsigned int m_textureSize;
  unsigned int m_tileSize;
  boost::dynamic_bitset<size_t> m_used; // size_t gets rid of 64-bit compile warning

};

#endif // TILESHEET_H