#include "stdafx.h"

#include "views/EntityStandard2DView.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "systems/SystemLighting.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/RNGUtils.h"
#include "views/Standard2DGraphicViews.h"

EntityStandard2DView::EntityStandard2DView(EntityId entity,
                                           Standard2DGraphicViews& views)
  :
  EntityView(entity),
  m_views{ views }
{
}

bool EntityStandard2DView::onEvent(Event const& event)
{
  /// @todo WRITE ME
  return false;
}

EntityStandard2DView::~EntityStandard2DView()
{
}

void EntityStandard2DView::draw(sf::RenderTarget& target,
                                Systems::Lighting* lighting,
                                bool use_smoothing,
                                int frame)
{
  auto& config = S<IConfigSettings>();
  auto entity = getEntity();
  auto& texture = m_views.getTileSheet().getTexture();

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

sf::RectangleShape EntityStandard2DView::drawRectangle(int frame)
{
  auto& config = S<IConfigSettings>();
  auto entity = getEntity();
  auto& texture = m_views.getTileSheet().getTexture();

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

std::string EntityStandard2DView::getViewName()
{
  return "standard2D";
}

UintVec2 EntityStandard2DView::getTileSheetCoords(int frame) const
{
  auto& entity = getEntity();
  auto& categoryData = S<IGameRules>().categoryData(COMPONENTS.category[entity]);
  UintVec2 offset;

  // Get tile coordinates on the sheet.
  UintVec2 start_coords = m_views.getTileSheetCoords(COMPONENTS.category[entity]);

  // If the entity has the "animated" component, call the Lua function to get the offset (tile to choose).
  if (categoryData["components"].count("animated") > 0)
  {
    offset = GAME.lua().callEntityFunction("get_tile_offset", entity, frame, UintVec2(0, 0));
  }

  // Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
