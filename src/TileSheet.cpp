#include "TileSheet.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/log/trivial.hpp>
#include <map>

#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "MathUtils.h"
#include "New.h"

// Static declarations.
std::unique_ptr<TileSheet> TileSheet::instance_;

struct TileSheet::Impl
{
  sf::Texture texture;
  unsigned int texture_size;
  boost::dynamic_bitset<> used;

  /// Return bitset index based on coordinates.
  unsigned int get_index(unsigned int x, unsigned int y)
  {
    return (y * texture_size) + x;
  }

  /// Return true if the requested area is totally unused.
  bool area_is_unused(sf::Vector2u start, sf::Vector2u size)
  {
    for (unsigned int y = start.y; y < start.y + size.y; ++y)
    {
      for (unsigned int x = start.x; x < start.x + size.x; ++x)
      {
        if (used[get_index(x, y)])
        {
          return false;
        }
      }
    }
    return true;
  }

  /// Find the first free tile area.
  /// @param size Size of the area to search for, IN TILES.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  sf::Vector2u find_unused_area(sf::Vector2u size)
  {
    sf::Vector2u start(0, 0);

    uint32_t texture_size_in_tiles = texture_size / Settings.get<unsigned int>("map_tile_size");

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
      ++start.y;
    }

    /// If we got here, there's no free space.
    throw std::out_of_range("Out of space on tile sheet");
  }

  /// Mark a rectangle of tiles as being used.
  /// @param upper_left_corner  Upper-left corner of rectangle.
  /// @param size               Size of the rectangle to mark.
  /// @todo This is an extremely naive algorithm and can definitely be optimized.
  void mark_tiles_used(sf::Vector2u upper_left_corner, sf::Vector2u size)
  {
    for (uint32_t y = upper_left_corner.y; y < upper_left_corner.y + size.y; ++y)
    {
      for (uint32_t x = upper_left_corner.x; x < upper_left_corner.x + size.x; ++x)
      {
        used[get_index(x, y)] = true;
      }
    }
  }

};

TileSheet::TileSheet()
  : pImpl(NEW Impl())
{
  /// @todo Make this a configurable setting.
  pImpl->texture_size = 1024;
  //pImpl->texture_size = pImpl->texture.getMaximumSize();
  
  bool success = pImpl->texture.create(pImpl->texture_size, pImpl->texture_size);

  if (!success)
  {
    FATAL_ERROR("Could not create TileSheet texture. Now we're sad.");
  }

  uint32_t used_map_size = (pImpl->texture_size / Settings.get<unsigned int>("map_tile_size")) *
                           (pImpl->texture_size / Settings.get<unsigned int>("map_tile_size"));
  pImpl->used.resize(used_map_size);
}

TileSheet::~TileSheet()
{
  //dtor
}

TileSheet& TileSheet::instance()
{
  if (!instance_)
  {
    instance_.reset(NEW TileSheet());
  }

  return *(instance_.get());
}

sf::Vector2u TileSheet::load_collection(std::string const& filename)
{
  sf::Image image;
  if (!image.loadFromFile(filename))
  {
    throw std::exception(std::string("Tile collection file not found: \"" + filename + "\"").c_str());
  }

  image.createMaskFromColor(sf::Color(255, 0, 255));

  unsigned int tile_size = Settings.get<unsigned int>("map_tile_size");

  sf::Vector2u image_size = image.getSize();

  sf::Vector2u image_size_in_tiles =
    sf::Vector2u(divide_and_round_up(image_size.x, tile_size),
                 divide_and_round_up(image_size.y, tile_size));

  sf::Vector2u free_coords = pImpl->find_unused_area(image_size_in_tiles);

  pImpl->texture.update(image, free_coords.x * tile_size, free_coords.y * tile_size);

  pImpl->mark_tiles_used(free_coords, image_size_in_tiles);

  return free_coords;
}

sf::IntRect TileSheet::get_tile(sf::Vector2u tile) const
{
  sf::IntRect rect;
  unsigned int tile_size = Settings.get<unsigned int>("map_tile_size");
  rect.left = tile.x * tile_size;
  rect.top = tile.y * tile_size;
  rect.width = tile_size;
  rect.height = tile_size;

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
  return pImpl->texture;
}

void TileSheet::add_quad(sf::VertexArray& vertices,
                             sf::Vector2u tile_coords, sf::Color bg_color,
                             sf::Vector2f ul_coord, sf::Vector2f ur_coord,
                             sf::Vector2f lr_coord, sf::Vector2f ll_coord)
{
  sf::Vertex new_vertex;
  float ts(Settings.get<float>("map_tile_size"));
  sf::Vector2f texNW(tile_coords.x * ts, tile_coords.y * ts);

  new_vertex.color = bg_color;

  new_vertex.position = ul_coord;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.position = ur_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y);
  vertices.append(new_vertex);

  new_vertex.position = lr_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.position = ll_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x,
                                      texNW.y + ts);
  vertices.append(new_vertex);
}

void TileSheet::add_gradient_quad(sf::VertexArray& vertices,
                                  sf::Vector2u tile_coords,
                                  sf::Color ul_color, sf::Vector2f ul_coord,
                                  sf::Color ur_color, sf::Vector2f ur_coord,
                                  sf::Color lr_color, sf::Vector2f lr_coord,
                                  sf::Color ll_color, sf::Vector2f ll_coord)
{
  sf::Vertex new_vertex;
  float ts(Settings.get<float>("map_tile_size"));
  sf::Vector2f texNW(tile_coords.x * ts, tile_coords.y * ts);

  new_vertex.color = ul_color;
  new_vertex.position = ul_coord;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.color = ur_color;
  new_vertex.position = ur_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y);
  vertices.append(new_vertex);

  new_vertex.color = lr_color;
  new_vertex.position = lr_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts,
                                      texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.color = ll_color;
  new_vertex.position = ll_coord;
  new_vertex.texCoords = sf::Vector2f(texNW.x,
                                      texNW.y + ts);
  vertices.append(new_vertex);
}

void TileSheet::add_gradient_quad(sf::VertexArray& vertices,
  sf::Vector2u tile_coords,
  sf::Vector2f coordNW, sf::Vector2f coordNE, sf::Vector2f coordSE, sf::Vector2f coordSW,
  sf::Color colorC, 
  sf::Color colorNW, sf::Color colorN,
  sf::Color colorNE, sf::Color colorE,
  sf::Color colorSE, sf::Color colorS,
  sf::Color colorSW, sf::Color colorW)
{
  float ts(Settings.get<float>("map_tile_size"));
  float half_ts = (ts / 2.0f);

  sf::Vector2f coordC((coordNW.x + coordNE.x + coordSE.x + coordSW.x) / 4, (coordNW.y + coordNE.y + coordSE.y + coordSW.y) / 4);
  sf::Vector2f coordN((coordNW.x + coordNE.x) / 2, (coordNW.y + coordNE.y) / 2);
  sf::Vector2f coordE((coordNE.x + coordSE.x) / 2, (coordNE.y + coordSE.y) / 2);
  sf::Vector2f coordS((coordSW.x + coordSE.x) / 2, (coordSW.y + coordSE.y) / 2);
  sf::Vector2f coordW((coordNW.x + coordSW.x) / 2, (coordNW.y + coordSW.y) / 2);

  sf::Vector2f texNW(tile_coords.x * ts, tile_coords.y * ts);
  sf::Vector2f texN(texNW.x + half_ts, texNW.y);
  sf::Vector2f texNE(texNW.x + ts, texNW.y);
  sf::Vector2f texE(texNW.x + ts, texNW.y + half_ts);
  sf::Vector2f texSE(texNW.x + ts, texNW.y + ts);
  sf::Vector2f texS(texNW.x + half_ts, texNW.y + ts);
  sf::Vector2f texSW(texNW.x, texNW.y + ts);
  sf::Vector2f texW(texNW.x, texNW.y + half_ts);
  sf::Vector2f texC(texNW.x + half_ts, texNW.y + half_ts);

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
                                     sf::Vector2f ul_coord,
                                     sf::Vector2f ur_coord,
                                     sf::Vector2f lr_coord,
                                     sf::Vector2f ll_coord)
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
