#include "stdafx.h"

#include "views/EntityView2D.h"

#include "components/ComponentManager.h"
#include "config/Bible.h"
#include "config/Settings.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "systems/SystemLighting.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/RNGUtils.h"

EntityView2D::EntityView2D(EntityId entity)
  :
  EntityView(entity)
{
}

bool EntityView2D::onEvent(Event const& event)
{
  /// @todo WRITE ME
  return false;
}

EntityView2D::~EntityView2D()
{
}

void EntityView2D::draw(sf::RenderTarget& target,
                        Systems::Lighting* lighting,
                        bool use_smoothing,
                        int frame)
{
  auto& config = Config::settings();
  auto entity = getEntity();
  auto& texture = App::the_tilesheet().getTexture();

  // Can't render if it doesn't have a Position component.
  if (!COMPONENTS.position.existsFor(entity)) return;

  auto& position = COMPONENTS.position[entity];

  // Can't render if it's in another object.
  if (position.parent() != EntityId::Void) return;

  MapTile& tile = MAPS.get(position.map()).getTile(position.coords());

  sf::RectangleShape rectangle;
  sf::IntRect textureCoords;

  auto targetCoords = getLocation();
  auto targetSize = getSize();
  UintVec2 tileSize = config.get("graphics-tile-size");
  if (targetSize == RealVec2(0, 0))
  {
    targetSize = {
      static_cast<float>(tileSize.x),
      static_cast<float>(tileSize.y)
    };
  }

  UintVec2 tileCoords = getTileSheetCoords(frame);
  textureCoords.left = tileCoords.x * tileSize.x;
  textureCoords.top = tileCoords.y * tileSize.y;
  textureCoords.width = tileSize.x;
  textureCoords.height = tileSize.y;

  Color thing_color;
  if (lighting != nullptr)
  {
    thing_color = lighting->getLightLevel(position.coords());
  }
  else
  {
    thing_color = Color::White;
  }

  if (use_smoothing)
  {
    texture.setSmooth(true);
  }

  rectangle.setPosition(targetCoords);
  rectangle.setSize(targetSize);
  rectangle.setTexture(&texture);
  rectangle.setTextureRect(textureCoords);
  rectangle.setFillColor(sf::Color(thing_color));

  target.draw(rectangle);

  if (use_smoothing)
  {
    texture.setSmooth(false);
  }
}

sf::RectangleShape EntityView2D::drawRectangle(int frame)
{
  auto& config = Config::settings();
  auto entity = getEntity();
  auto& texture = App::the_tilesheet().getTexture();

  sf::RectangleShape rectangle;
  sf::IntRect textureCoords;

  auto targetCoords = getLocation();
  auto targetSize = getSize();
  UintVec2 tileSize = config.get("graphics-tile-size");
  if (targetSize == RealVec2(0, 0))
  {
    targetSize = {
      static_cast<float>(tileSize.x),
      static_cast<float>(tileSize.y)
    };
  }

  UintVec2 tileCoords = getTileSheetCoords(frame);
  textureCoords.left = tileCoords.x * tileSize.x;
  textureCoords.top = tileCoords.y * tileSize.y;
  textureCoords.width = tileSize.x;
  textureCoords.height = tileSize.y;

  rectangle.setPosition(targetCoords);
  rectangle.setSize(targetSize);
  rectangle.setTexture(&texture);
  rectangle.setTextureRect(textureCoords);
  rectangle.setFillColor(Color::White);

  return rectangle;
}

std::string EntityView2D::getViewName()
{
  return "standard2D";
}

UintVec2 EntityView2D::getTileSheetCoords(int frame) const
{
  auto& entity = getEntity();
  auto& categoryData = Config::bible().categoryData(COMPONENTS.category[entity]);
  UintVec2 offset;

  // Get tile coordinates on the sheet.
  UintVec2 start_coords = App::the_tilesheet().getTileSheetCoords(COMPONENTS.category[entity]);

  // If the entity has the "animated" component, call the Lua function to get the offset (tile to choose).
  if (categoryData["components"].count("animated") > 0)
  {
    offset = GAME.lua().callEntityFunction("get_tile_offset", entity, frame, UintVec2(0, 0));
  }

  // Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
