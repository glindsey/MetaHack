#include "stdafx.h"

#include "components/ComponentManager.h"
#include "entity/EntityStandard2DView.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/RNGUtils.h"

EntityStandard2DView::EntityStandard2DView(Entity& entity, TileSheet& tile_sheet)
  :
  EntityView(entity),
  m_tile_sheet(tile_sheet)
{
}

EventResult EntityStandard2DView::onEvent_NVI(Event const & event)
{
  return{ EventHandled::Yes, ContinueBroadcasting::Yes };
}

EntityStandard2DView::~EntityStandard2DView()
{
}

void EntityStandard2DView::draw(sf::RenderTarget& target,
                                bool use_lighting,
                                bool use_smoothing,
                                int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto& entity = getEntity();
  auto& texture = m_tile_sheet.getTexture();

  // Can't render if it doesn't have a Position component.
  if (!COMPONENTS.position.existsFor(entity.getId())) return;

  auto& position = COMPONENTS.position[entity.getId()];

  // Can't render if it's in another object.
  if (position.parent() != EntityId::Mu()) return;

  MapTile& tile = position.map()->getTile(position.coords());

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

  UintVec2 tile_coords = get_tile_sheet_coords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

  Color thing_color;
  if (use_lighting)
  {
    thing_color = tile.getLightLevel();
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

std::string EntityStandard2DView::getViewName()
{
  return "standard2D";
}

UintVec2 EntityStandard2DView::get_tile_sheet_coords(int frame) const
{
  auto& entity = getEntity();
  auto& category = entity.getCategoryData();
  UintVec2 offset;

  // Get tile coordinates on the sheet.
  UintVec2 start_coords = category.value("tile-location", UintVec2(0, 0));

  // If the entity has the "animated" component, call the Lua function to get the offset (tile to choose).
  if (category["components"].count("animated") > 0)
  {
    offset = entity.call_lua_function("get_tile_offset", frame, UintVec2(0, 0));
  }

  // Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
