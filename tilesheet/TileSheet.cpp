#include "stdafx.h"

#include "tilesheet/TileSheet.h"

#include "ErrorHandler.h"
#include "utilities/MathUtils.h"

TileSheet::TileSheet(unsigned int tileSize, unsigned int textureSize)
  :
  m_tileSize{ tileSize },
  m_textureSize{ textureSize }
{
  if (m_textureSize == 0) m_textureSize = m_texture.getMaximumSize();

  bool success = m_texture.create(m_textureSize, m_textureSize);

  if (!success)
  {
    FATAL_ERROR("Could not create TileSheet texture. Now we're sad.");
  }

  uint32_t texture_dimension_in_tiles = m_textureSize / m_tileSize;
  uint32_t used_map_size = texture_dimension_in_tiles * texture_dimension_in_tiles;
  m_used.resize(used_map_size);

  CLOG(TRACE, "TileSheet") << "Created " << m_textureSize << " x " << m_textureSize << " tilesheet texture";
  CLOG(TRACE, "TileSheet") << "Fits "
    << texture_dimension_in_tiles << " x " << texture_dimension_in_tiles
    << " tiles of size "
    << m_tileSize << " x " << m_tileSize;
}

TileSheet::~TileSheet()
{
  //dtor
}

UintVec2 TileSheet::load_collection(FileName const& filename)
{
  sf::Image image;
  if (!image.loadFromFile(filename))
  {
    throw std::runtime_error("Tile collection file not found: \"" + filename + "\"");
  }

  image.createMaskFromColor(sf::Color(255, 0, 255));

  UintVec2 image_size = image.getSize();

  UintVec2 image_size_in_tiles =
    UintVec2(divide_and_round_up(image_size.x, m_tileSize),
          divide_and_round_up(image_size.y, m_tileSize));

  UintVec2 free_coords = find_unused_area(image_size_in_tiles);

  m_texture.update(image, free_coords.x * m_tileSize, free_coords.y * m_tileSize);

  mark_tiles_used(free_coords, image_size_in_tiles);

  return free_coords;
}

sf::IntRect TileSheet::get_tile(UintVec2 tile) const
{
  sf::IntRect rect;
  rect.left = tile.x * m_tileSize;
  rect.top = tile.y * m_tileSize;
  rect.width = m_tileSize;
  rect.height = m_tileSize;

#ifdef _DEBUG
  if ((rect.left < 0) || (rect.top < 0) ||
    (rect.left + rect.width >= static_cast<int>(m_texture.getSize().x)) ||
      (rect.top + rect.height >= static_cast<int>(m_texture.getSize().y)))
  {
    MAJOR_ERROR("Request for tile (%d, %d) is out of bounds on the sprite sheet!",
                tile.x, tile.y);
  }
#endif // DEBUG

  return rect;
}

sf::Texture& TileSheet::getTexture(void)
{
  return m_texture;
}

void TileSheet::add_quad(sf::VertexArray& vertices,
                         UintVec2 tile_coords, sf::Color bg_color,
                         RealVec2 ul_coord, RealVec2 ur_coord,
                         RealVec2 ll_coord, RealVec2 lr_coord)
{
  sf::Vertex new_vertex;
  float ts{ static_cast<float>(m_tileSize) };
  RealVec2 texNW(tile_coords.x * ts, tile_coords.y * ts);

  new_vertex.color = bg_color;

  new_vertex.position = ul_coord;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.position = ur_coord;
  new_vertex.texCoords = RealVec2(texNW.x + ts, texNW.y);
  vertices.append(new_vertex);

  new_vertex.position = lr_coord;
  new_vertex.texCoords = RealVec2(texNW.x + ts, texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.position = ll_coord;
  new_vertex.texCoords = RealVec2(texNW.x, texNW.y + ts);
  vertices.append(new_vertex);
}

void TileSheet::add_gradient_quad(sf::VertexArray& vertices,
                                  UintVec2 tile_coords,
                                  RealVec2 coordNW, RealVec2 coordNE,
                                  RealVec2 coordSW, RealVec2 coordSE,
                                  sf::Color colorNW, sf::Color colorN, sf::Color colorNE,
                                  sf::Color colorW, sf::Color colorC, sf::Color colorE,
                                  sf::Color colorSW, sf::Color colorS, sf::Color colorSE)
{
  float ts{ static_cast<float>(m_tileSize) };
  float half_ts{ ts / 2.0f };

  RealVec2 coordC((coordNW.x + coordNE.x + coordSE.x + coordSW.x) / 4, (coordNW.y + coordNE.y + coordSE.y + coordSW.y) / 4);
  RealVec2 coordN((coordNW.x + coordNE.x) / 2, (coordNW.y + coordNE.y) / 2);
  RealVec2 coordE((coordNE.x + coordSE.x) / 2, (coordNE.y + coordSE.y) / 2);
  RealVec2 coordS((coordSW.x + coordSE.x) / 2, (coordSW.y + coordSE.y) / 2);
  RealVec2 coordW((coordNW.x + coordSW.x) / 2, (coordNW.y + coordSW.y) / 2);

  RealVec2 texNW(tile_coords.x * ts, tile_coords.y * ts);
  RealVec2 texN(texNW.x + half_ts, texNW.y);
  RealVec2 texNE(texNW.x + ts, texNW.y);
  RealVec2 texE(texNW.x + ts, texNW.y + half_ts);
  RealVec2 texSE(texNW.x + ts, texNW.y + ts);
  RealVec2 texS(texNW.x + half_ts, texNW.y + ts);
  RealVec2 texSW(texNW.x, texNW.y + ts);
  RealVec2 texW(texNW.x, texNW.y + half_ts);
  RealVec2 texC(texNW.x + half_ts, texNW.y + half_ts);

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
                                     RealVec2 ul_coord,
                                     RealVec2 ur_coord,
                                     RealVec2 lr_coord,
                                     RealVec2 ll_coord)
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

unsigned int TileSheet::get_index(UintVec2 coords)
{
  uint32_t texture_size_in_tiles = m_textureSize / m_tileSize;
  return (coords.y * texture_size_in_tiles) + coords.x;
}

bool TileSheet::area_is_unused(UintVec2 start, UintVec2 size)
{
  uint32_t texture_size_in_tiles = m_textureSize / m_tileSize;

  if (((start.x + size.x) > texture_size_in_tiles) ||
    ((start.y + size.y) > texture_size_in_tiles))
  {
    return false;
  }

  for (unsigned int y = start.y; y < start.y + size.y; ++y)
  {
    for (unsigned int x = start.x; x < start.x + size.x; ++x)
    {
      if (m_used[get_index({ x, y })])
      {
        return false;
      }
    }
  }
  return true;
}

UintVec2 TileSheet::find_unused_area(UintVec2 size)
{
  UintVec2 start(0, 0);

  uint32_t texture_size_in_tiles = m_textureSize / m_tileSize;

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
void TileSheet::mark_tiles_used(UintVec2 upper_left_corner, UintVec2 size)
{
  for (uint32_t y = upper_left_corner.y; y < upper_left_corner.y + size.y; ++y)
  {
    for (uint32_t x = upper_left_corner.x; x < upper_left_corner.x + size.x; ++x)
    {
      m_used[get_index({ x, y })] = true;
    }
  }
}
