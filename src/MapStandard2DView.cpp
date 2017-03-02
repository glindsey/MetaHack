#include "stdafx.h"

#include "IConfigSettings.h"
#include "MapStandard2DView.h"
#include "MapTileStandard2DView.h"
#include "New.h"
#include "Service.h"

#include "ShaderEffect.h"

MapStandard2DView::MapStandard2DView(Map& map)
  :
  MapView(map)
{
  reset_cached_render_data();

  // Create a grid of tile views, each tied to a map tile.
  m_map_tile_views.reset(new Grid2D<MapTileStandard2DView>(map.get_size(), 
                                                           [&](Vec2i coords) -> MapTileStandard2DView*
  {
    return NEW MapTileStandard2DView(map.get_tile(coords));
  }));

}

void MapStandard2DView::update_tiles(ThingId viewer)
{
  auto& map = get_map();
  auto& map_size = map.get_size();

  static Vec2f position;

  // Loop through and draw tiles.
  m_map_seen_vertices.clear();
  m_map_memory_vertices.clear();

  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).add_tile_vertices(viewer, m_map_seen_vertices, m_map_memory_vertices);
    }
  }
}

void MapStandard2DView::update_things(ThingId viewer, int frame)
{
  auto& map = get_map();
  auto& map_size = map.get_size();

  // Loop through and draw things.
  m_thing_vertices.clear();
  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).add_things_floor_vertices(viewer, m_thing_vertices, true, frame);
    }
  }
}

bool MapStandard2DView::render(sf::RenderTexture& texture, int frame)
{
  the_shader.setParameter("texture", sf::Shader::CurrentTexture);

  sf::RenderStates render_states = sf::RenderStates::Default;
  render_states.shader = &the_shader;
  render_states.texture = &(the_tilesheet.getTexture());

  the_shader.setParameter("effect", ShaderEffect::Lighting);
  //the_shader.setParameter("effect", ShaderEffect::Default);
  texture.draw(m_map_seen_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Sepia);
  texture.draw(m_map_memory_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Lighting);
  //the_shader.setParameter("effect", ShaderEffect::Default);
  texture.draw(m_thing_vertices, render_states);

  return true;
}

void MapStandard2DView::draw_highlight(sf::RenderTarget& target,
                                       Vec2f location,
                                       sf::Color fgColor,
                                       sf::Color bgColor,
                                       int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto map_tile_size = config.get<float>("map_tile_size");

  float half_ts(map_tile_size * 0.5f);
  Vec2f vSW(location.x - half_ts, location.y + half_ts);
  Vec2f vSE(location.x + half_ts, location.y + half_ts);
  Vec2f vNW(location.x - half_ts, location.y - half_ts);
  Vec2f vNE(location.x + half_ts, location.y - half_ts);

  sf::RectangleShape box_shape;
  Vec2f box_position;
  Vec2f box_size(map_tile_size, map_tile_size);
  Vec2f box_half_size(box_size.x / 2, box_size.y / 2);
  box_position.x = (location.x - box_half_size.x);
  box_position.y = (location.y - box_half_size.y);
  box_shape.setPosition(box_position);
  box_shape.setSize(box_size);
  box_shape.setOutlineColor(fgColor);
  box_shape.setOutlineThickness(config.get<float>("tile_highlight_border_width"));
  box_shape.setFillColor(bgColor);

  target.draw(box_shape);
}


void MapStandard2DView::reset_cached_render_data()
{
  auto& map = get_map();
  auto map_size = map.get_size();

  // Create vertices:
  // 4 vertices * 4 quads for the floor
  // 4 vertices * 4 quads * 4 potential walls
  // = 16 + 64 = 80 possible vertices per tile.
  m_map_seen_vertices.resize(map_size.x * map_size.y * 80);
  m_map_memory_vertices.resize(map_size.x * map_size.y * 80);

  // Create the vertex arrays.
  m_map_seen_vertices.clear();
  m_map_seen_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  m_map_memory_vertices.clear();
  m_map_memory_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  m_thing_vertices.clear();
  m_thing_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
}

void MapStandard2DView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
