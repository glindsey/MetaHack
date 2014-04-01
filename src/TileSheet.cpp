#include "TileSheet.h"

#include <map>

#include "ConfigSettings.h"
#include "ErrorHandler.h"

struct TileSheet::Impl
{
  sf::Texture texture;
};

TileSheet::TileSheet()
  : impl(new Impl())
{
  //ctor
}

TileSheet::~TileSheet()
{
  //dtor
}

bool TileSheet::load(std::string const& filename)
{
  sf::Image image;
  image.loadFromFile(filename);
  image.createMaskFromColor(sf::Color(192, 32, 64));
  return impl->texture.loadFromImage(image);
}

sf::IntRect TileSheet::get_tile(sf::Vector2u tile) const
{
  sf::IntRect rect;
  rect.left = tile.x * Settings.map_tile_size;
  rect.top = tile.y * Settings.map_tile_size;
  rect.width = Settings.map_tile_size;
  rect.height = Settings.map_tile_size;

  #ifdef DEBUG
  if ((rect.left < 0) || (rect.top < 0) ||
      (rect.left + rect.width >= static_cast<int>(impl->texture.getSize().x)) ||
      (rect.top + rect.height >= static_cast<int>(impl->texture.getSize().y)))
  {
    MAJOR_ERROR("Request for tile (%d, %d) is out of bounds on the sprite sheet!",
                tile.x, tile.y);
  }
  #endif // DEBUG

  return rect;
}

sf::Texture& TileSheet::getTexture(void)
{
  return impl->texture;
}

void TileSheet::add_quad(sf::VertexArray& vertices,
                             sf::Vector2u tile_coords, sf::Color bg_color,
                             sf::Vector2f ul_coord, sf::Vector2f ur_coord,
                             sf::Vector2f ll_coord, sf::Vector2f lr_coord)
{
  sf::Vertex new_vertex;
  float ts(static_cast<float>(Settings.map_tile_size));
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
                                  sf::Color ll_color, sf::Vector2f ll_coord,
                                  sf::Color lr_color, sf::Vector2f lr_coord)
{
  sf::Vertex new_vertex;
  float ts(static_cast<float>(Settings.map_tile_size));
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

void TileSheet::add_outline_vertices(sf::VertexArray& vertices,
                                     sf::Color bg_color,
                                     sf::Vector2f ul_coord,
                                     sf::Vector2f ur_coord,
                                     sf::Vector2f ll_coord,
                                     sf::Vector2f lr_coord)
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
