#include "stdafx.h"

#include "entity/EntityStandard2DView.h"
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
  MapTile* root_tile = getEntity().get_maptile();
  auto& texture = m_tile_sheet.getTexture();

  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  auto target_coords = getLocation();
  auto target_size = getSize();
  auto tile_size = config.get("map_tile_size").as<int32_t>();
  if (target_size == RealVec2(0, 0))
  {
    target_size = { static_cast<float>(tile_size), static_cast<float>(tile_size) };
  }

  UintVec2 tile_coords = get_tile_sheet_coords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  if (use_smoothing)
  {
    texture.setSmooth(true);
  }

  rectangle.setPosition(target_coords);
  rectangle.setSize(target_size);
  rectangle.setTexture(&texture);
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

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

  /// Get tile coordinates on the sheet.
  UintVec2 start_coords = entity.get_metadata().get_tile_coords();

  /// Call the Lua function to get the offset (tile to choose).
  UintVec2 offset = entity.call_lua_function("get_tile_offset", { Property::from(frame) }).as<UintVec2>();

  /// Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
