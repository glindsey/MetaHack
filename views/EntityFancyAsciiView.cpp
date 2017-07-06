#include "stdafx.h"

#include "views/EntityFancyAsciiView.h"

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
#include "views/FancyAsciiGraphicViews.h"

EntityFancyAsciiView::EntityFancyAsciiView(EntityId entity,
                                           FancyAsciiGraphicViews& views)
  :
  EntityView(entity),
  m_views{ views }
{
}

bool EntityFancyAsciiView::onEvent(Event const& event)
{
  /// @todo WRITE ME
  return false;
}

EntityFancyAsciiView::~EntityFancyAsciiView()
{
}

void EntityFancyAsciiView::draw(sf::RenderTarget& target,
                                Systems::Lighting* lighting,
                                bool use_smoothing,
                                int frame)
{
  auto& config = S<IConfigSettings>();
  auto& entity = getEntity();
  auto& texture = m_views.getTileSheet().getTexture();

  // Can't render if it doesn't have a Position component.
  if (!COMPONENTS.position.existsFor(entity)) return;

  auto& position = COMPONENTS.position[entity];

  // Can't render if it's in another object.
  if (position.parent() != EntityId::Void) return;

  MapTile& tile = MAPS.get(position.map()).getTile(position.coords());

  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  auto target_coords = getLocation();
  auto target_size = getSize();
  unsigned int tile_size = config.get("map-tile-size");
  if (target_size == RealVec2(0, 0))
  {
    target_size = { 
      static_cast<float>(tile_size), 
      static_cast<float>(tile_size)
    };
  }

  UintVec2 tile_coords = getTileSheetCoords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

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

  rectangle.setPosition(target_coords);
  rectangle.setSize(target_size);
  rectangle.setTexture(&texture);
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(sf::Color(thing_color));

  target.draw(rectangle);

  if (use_smoothing)
  {
    texture.setSmooth(false);
  }
}

std::string EntityFancyAsciiView::getViewName()
{
  return "fancyASCII";
}

UintVec2 EntityFancyAsciiView::getTileSheetCoords(int frame) const
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
