#ifndef TILESHEET_H
#define TILESHEET_H

#include "stdafx.h"

/// Sheet that contains a group of tiles on one texture.
class TileSheet
{
public:
  TileSheet(unsigned int tileSize, unsigned int textureSize);

  virtual ~TileSheet();

  /// Load a tile collection from disk and find a place to put them.
  /// @return The location that the tiles were placed on the sheet.
  Vec2u load_collection(FileName const& filename);

  /// Get a particular tile from the sheet.
  /// @warning Assumes tile is within the bounds of the loaded texture,
  ///          unless DEBUG is defined, in order to save execution time.
  sf::IntRect get_tile(Vec2u tile) const;

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
                Vec2u tile_coords, sf::Color bg_color,
                Vec2f ul_coord, Vec2f ur_coord,
                Vec2f ll_coord, Vec2f lr_coord);

  /// Add a quad with colors specified in a 3x3 grid.
  void add_gradient_quad(sf::VertexArray& vertices,
                         Vec2u tile_coords,
                         Vec2f coordNW, Vec2f coordNE,
                         Vec2f coordSW, Vec2f coordSE,
                         sf::Color colorNW, sf::Color colorN, sf::Color colorNE,
                         sf::Color colorW, sf::Color colorC, sf::Color colorE,
                         sf::Color colorSW, sf::Color colorS, sf::Color colorSE);

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
                            sf::Color bg_color,
                            Vec2f ul_coord,
                            Vec2f ur_coord,
                            Vec2f lr_coord,
                            Vec2f ll_coord);

protected:
  /// Return bitset index based on coordinates.
  unsigned int get_index(Vec2u coords);

  /// Return true if the requested area is totally unused.
  bool area_is_unused(Vec2u start, Vec2u size);

  /// Find the first free tile area.
  /// @param size Size of the area to search for, IN TILES.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  Vec2u find_unused_area(Vec2u size);

  /// Mark a rectangle of tiles as being used.
  /// @param upper_left_corner  Upper-left corner of rectangle.
  /// @param size               Size of the rectangle to mark.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  void mark_tiles_used(Vec2u upper_left_corner, Vec2u size);


private:
  sf::Texture m_texture;
  unsigned int m_textureSize;
  unsigned int m_tileSize;
  boost::dynamic_bitset<size_t> m_used; // size_t gets rid of 64-bit compile warning

};

#endif // TILESHEET_H