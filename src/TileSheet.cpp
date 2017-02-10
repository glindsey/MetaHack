#include "stdafx.h"

#include "TileSheet.h"

#include "ErrorHandler.h"
#include "MathUtils.h"

TileSheet::TileSheet(unsigned int tileSize_)
  :
  tileSize{ tileSize_ }
{
  SET_UP_LOGGER("TileSheet", true);

  /// @todo Make this a configurable setting.
  textureSize = 1024;
 
  //textureSize = pImpl->texture.getMaximumSize();

  bool success = texture.create(textureSize, textureSize);

  if (!success)
  {
    FATAL_ERROR("Could not create TileSheet texture. Now we're sad.");
  }

  uint32_t texture_dimension_in_tiles = textureSize / tileSize;
  uint32_t used_map_size = texture_dimension_in_tiles * texture_dimension_in_tiles;
  used.resize(used_map_size);

  CLOG(TRACE, "TileSheet") << "Created " << textureSize << " x " << textureSize << " tilesheet texture";
  CLOG(TRACE, "TileSheet") << "Fits "
    << texture_dimension_in_tiles << " x " << texture_dimension_in_tiles
    << " tiles of size "
    << tileSize << " x " << tileSize;
}

TileSheet::~TileSheet()
{
  //dtor
}

Vec2u TileSheet::load_collection(FileName const& filename)
{
  sf::Image image;
  if (!image.loadFromFile(filename))
  {
    throw std::runtime_error("Tile collection file not found: \"" + filename + "\"");
  }

  image.createMaskFromColor(sf::Color(255, 0, 255));

  Vec2u image_size = image.getSize();

  Vec2u image_size_in_tiles =
    Vec2u(divide_and_round_up(image_size.x, tileSize),
                 divide_and_round_up(image_size.y, tileSize));

  Vec2u free_coords = find_unused_area(image_size_in_tiles);

  texture.update(image, free_coords.x * tileSize, free_coords.y * tileSize);

  mark_tiles_used(free_coords, image_size_in_tiles);

  return free_coords;
}

sf::IntRect TileSheet::get_tile(Vec2u tile) const
{
  sf::IntRect rect;
  rect.left = tile.x * tileSize;
  rect.top = tile.y * tileSize;
  rect.width = tileSize;
  rect.height = tileSize;

#ifdef DEBUG
  if ((rect.left < 0) || (rect.top < 0) ||
    (rect.left + rect.width >= static_cast<int>(pImpl->texture.getSize().x)) ||
      (rect.top + rect.height >= static_cast<int>(pImpl->texture.getSize().y)))
  {
    MAJOR_ERROR("Request for tile (%d, %d) is out of bounds on the sprite sheet!",
                tile.x, tile.y);
  }
#endif // DEBUG

  return rect;
}

sf::Texture& TileSheet::getTexture(void)
{
  return texture;
}

void TileSheet::add_quad(sf::VertexArray& vertices,
                         Vec2u tile_coords, sf::Color bg_color,
                         Vec2f ul_coord, Vec2f ur_coord,
                         Vec2f ll_coord, Vec2f lr_coord)
{
  sf::Vertex new_vertex;
  float ts{ static_cast<float>(tileSize) };
  Vec2f texNW(tile_coords.x * ts, tile_coords.y * ts);

  new_vertex.color = bg_color;

  new_vertex.position = ul_coord;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.position = ur_coord;
  new_vertex.texCoords = Vec2f(texNW.x + ts, texNW.y);
  vertices.append(new_vertex);

  new_vertex.position = lr_coord;
  new_vertex.texCoords = Vec2f(texNW.x + ts, texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.position = ll_coord;
  new_vertex.texCoords = Vec2f(texNW.x, texNW.y + ts);
  vertices.append(new_vertex);
}

void TileSheet::add_gradient_quad(sf::VertexArray& vertices,
                                  Vec2u tile_coords,
                                  Vec2f coordNW, Vec2f coordNE,
                                  Vec2f coordSW, Vec2f coordSE,
                                  sf::Color colorNW, sf::Color colorN, sf::Color colorNE,
                                  sf::Color colorW, sf::Color colorC, sf::Color colorE,
                                  sf::Color colorSW, sf::Color colorS, sf::Color colorSE)
{
  float ts{ static_cast<float>(tileSize) };
  float half_ts{ ts / 2.0f };

  Vec2f coordC((coordNW.x + coordNE.x + coordSE.x + coordSW.x) / 4, (coordNW.y + coordNE.y + coordSE.y + coordSW.y) / 4);
  Vec2f coordN((coordNW.x + coordNE.x) / 2, (coordNW.y + coordNE.y) / 2);
  Vec2f coordE((coordNE.x + coordSE.x) / 2, (coordNE.y + coordSE.y) / 2);
  Vec2f coordS((coordSW.x + coordSE.x) / 2, (coordSW.y + coordSE.y) / 2);
  Vec2f coordW((coordNW.x + coordSW.x) / 2, (coordNW.y + coordSW.y) / 2);

  Vec2f texNW(tile_coords.x * ts, tile_coords.y * ts);
  Vec2f texN(texNW.x + half_ts, texNW.y);
  Vec2f texNE(texNW.x + ts, texNW.y);
  Vec2f texE(texNW.x + ts, texNW.y + half_ts);
  Vec2f texSE(texNW.x + ts, texNW.y + ts);
  Vec2f texS(texNW.x + half_ts, texNW.y + ts);
  Vec2f texSW(texNW.x, texNW.y + ts);
  Vec2f texW(texNW.x, texNW.y + half_ts);
  Vec2f texC(texNW.x + half_ts, texNW.y + half_ts);

  // Upper left
  vertices.append(sf::Vertex(coordNW, colorNW, texNW));
  vertices.append(sf::Vertex(coordN, colorN, texN));
  vertices.append(sf::Vertex(coordC, colorC, texC));
  vertices.append(sf::Vertex(coordW, colorW, texW));

  // Upper right
  vertices.append(sf::Vertex(coordN, colorN, texN));
  vertices.append(sf::Vertex(coordNE, colorNE, texNE));
  vertices.append(sf::Vertex(coordE, colorE, texE));
  vertices.append(sf::Vertex(coordC, colorC, texC));

  // Lower right
  vertices.append(sf::Vertex(coordC, colorC, texC));
  vertices.append(sf::Vertex(coordE, colorE, texE));
  vertices.append(sf::Vertex(coordSE, colorSE, texSE));
  vertices.append(sf::Vertex(coordS, colorS, texS));

  // Lower left
  vertices.append(sf::Vertex(coordW, colorW, texW));
  vertices.append(sf::Vertex(coordC, colorC, texC));
  vertices.append(sf::Vertex(coordS, colorS, texS));
  vertices.append(sf::Vertex(coordSW, colorSW, texSW));
}

void TileSheet::add_outline_vertices(sf::VertexArray& vertices,
                                     sf::Color bg_color,
                                     Vec2f ul_coord,
                                     Vec2f ur_coord,
                                     Vec2f lr_coord,
                                     Vec2f ll_coord)
{
  sf::Vertex new_vertex;

  new_vertex.color = bg_color;
  new_vertex.position = ul_coord;
  vertices.append(new_vertex);

  new_vertex.position = ur_coord;
  vertices.append(new_vertex);
  vertices.append(new_vertex);

  new_vertex.position = lr_coord;
  vertices.append(new_vertex);
  vertices.append(new_vertex);

  new_vertex.position = ll_coord;
  vertices.append(new_vertex);
  vertices.append(new_vertex);

  new_vertex.color = bg_color;
  new_vertex.position = ul_coord;
  vertices.append(new_vertex);
}

/// === PROTECTED METHODS =====================================================

unsigned int TileSheet::get_index(Vec2u coords)
{
  uint32_t texture_size_in_tiles = textureSize / tileSize;
  return (coords.y * texture_size_in_tiles) + coords.x;
}

bool TileSheet::area_is_unused(Vec2u start, Vec2u size)
{
  uint32_t texture_size_in_tiles = textureSize / tileSize;

  if (((start.x + size.x) > texture_size_in_tiles) ||
    ((start.y + size.y) > texture_size_in_tiles))
  {
    return false;
  }

  for (unsigned int y = start.y; y < start.y + size.y; ++y)
  {
    for (unsigned int x = start.x; x < start.x + size.x; ++x)
    {
      if (used[get_index({ x, y })])
      {
        return false;
      }
    }
  }
  return true;
}

Vec2u TileSheet::find_unused_area(Vec2u size)
{
  Vec2u start(0, 0);

  uint32_t texture_size_in_tiles = textureSize / tileSize;

  while (start.y < texture_size_in_tiles)
  {
    while (start.x < texture_size_in_tiles)
    {
      if (area_is_unused(start, size))
      {
        return start;
      }
      ++start.x;
    }
    start.x = 0;
    ++start.y;
  }

  /// If we got here, there's no free space.
  throw std::out_of_range("Out of space on tile sheet");
}

/// Mark a rectangle of tiles as being used.
/// @param upper_left_corner  Upper-left corner of rectangle.
/// @param size               Size of the rectangle to mark.
/// @todo This is an extremely naive algorithm and can definitely be optimized.
void TileSheet::mark_tiles_used(Vec2u upper_left_corner, Vec2u size)
{
  for (uint32_t y = upper_left_corner.y; y < upper_left_corner.y + size.y; ++y)
  {
    for (uint32_t x = upper_left_corner.x; x < upper_left_corner.x + size.x; ++x)
    {
      used[get_index({ x, y })] = true;
    }
  }
}
