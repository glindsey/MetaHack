#include "TileSheet.h"

#include <map>

#include "ErrorHandler.h"

int const TileSheet::TileSize = 32;

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
  rect.left = tile.x * TileSheet::TileSize;
  rect.top = tile.y * TileSheet::TileSize;
  rect.width = TileSheet::TileSize;
  rect.height = TileSheet::TileSize;

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

sf::Texture const& TileSheet::getTexture(void) const
{
  return impl->texture;
}
