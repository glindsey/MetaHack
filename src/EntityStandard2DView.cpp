#include "stdafx.h"

#include "IConfigSettings.h"
#include "EntityStandard2DView.h"
#include "RNGUtils.h"
#include "Service.h"

#include "ShaderEffect.h"

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
  auto& entity = get_entity();
  MapTile* root_tile = get_entity().get_maptile();
  auto& texture = m_tile_sheet.getTexture();

  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  auto target_coords = get_location();
  auto target_size = get_size();
  auto tile_size = config.get<unsigned int>("map_tile_size");
  if (target_size == 0)
  {
    target_size = { static_cast<float>(tile_size), static_cast<float>(tile_size) };
  }

  Vec2u tile_coords = get_tile_sheet_coords(frame);
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

std::string EntityStandard2DView::get_view_name()
{
  return "standard2D";
}

Vec2u EntityStandard2DView::get_tile_sheet_coords(int frame) const
{
  auto& entity = get_entity();

  /// Get tile coordinates on the sheet.
  Vec2u start_coords = entity.get_metadata().get_tile_coords();

  /// Call the Lua function to get the offset (tile to choose).
  Vec2u offset = entity.call_lua_function<Vec2u>("get_tile_offset", { frame });

  /// Add them to get the resulting coordinates.
  Vec2u tile_coords = start_coords + offset;

  return tile_coords;
}
