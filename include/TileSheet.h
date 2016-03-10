#ifndef TILESHEET_H
#define TILESHEET_H

#include "stdafx.h"

/// Sheet containing all tiles in the application.
class TileSheet
{
public:
  virtual ~TileSheet();

  /// Get the singleton tilesheet instance.
  static TileSheet& instance();

  /// Load a tile collection from disk and find a place to put them.
  /// @return The location that the tiles were placed on the sheet.
  sf::Vector2u load_collection(FileName const& filename);

  /// Get a particular tile from the sheet.
  /// @warning Assumes tile is within the bounds of the loaded texture,
  ///          unless DEBUG is defined, in order to save execution time.
  sf::IntRect get_tile(sf::Vector2u tile) const;

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
  static void add_quad(sf::VertexArray& vertices,
                       sf::Vector2u tile_coords, sf::Color bg_color,
                       sf::Vector2f ul_coord, sf::Vector2f ur_coord,
                       sf::Vector2f ll_coord, sf::Vector2f lr_coord);

  /// Add a quad with colors specified in a 3x3 grid.
  static void add_gradient_quad(sf::VertexArray& vertices,
                                sf::Vector2u tile_coords,
                                sf::Vector2f coordNW, sf::Vector2f coordNE,
                                sf::Vector2f coordSW, sf::Vector2f coordSE,
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
  static void add_outline_vertices(sf::VertexArray& vertices,
                                   sf::Color bg_color,
                                   sf::Vector2f ul_coord,
                                   sf::Vector2f ur_coord,
                                   sf::Vector2f lr_coord,
                                   sf::Vector2f ll_coord);

protected:
  TileSheet();

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  /// Static instance.
  static std::unique_ptr<TileSheet> instance_;
};

/// Macro to ease typing.
#define  TS       TileSheet::instance()

#endif // TILESHEET_H
