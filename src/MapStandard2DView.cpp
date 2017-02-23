#include "stdafx.h"

#include "IConfigSettings.h"
#include "MapStandard2DView.h"
#include "Service.h"

#include "ShaderEffect.h"

MapStandard2DView::MapStandard2DView(MapId map_id)
  :
  MapView(map_id)
{}

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
      add_tile_vertices(viewer, { x, y });
    }
  }
}

void MapStandard2DView::update_things(ThingId viewer, int frame)
{
  /// @todo Update this to use MapTileViews.
  auto& map = get_map();
  auto& map_size = map.get_size();

  // Loop through and draw things.
  m_thing_vertices.clear();
  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      ThingId contents = map.get_tile({ x, y }).get_tile_contents();
      Inventory& inv = contents->get_inventory();

      if (viewer->can_see({ x, y }))
      {
        if (inv.count() > 0)
        {
          // Only draw the largest thing on that tile.
          ThingId biggest_thing = inv.get_largest_thing();
          if (biggest_thing != ThingId::Mu())
          {
            add_thing_floor_vertices(biggest_thing, true, frame);
          }
        }
      }

      // Always draw the viewer itself last, if it is present at that tile.
      if (inv.contains(viewer))
      {
        add_thing_floor_vertices(viewer, true, frame);
      }
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

void MapStandard2DView::add_tile_vertices(ThingId viewer, Vec2i coords)
{
  auto& map = get_map();
  auto& map_size = map.get_size();
  auto& x = coords.x;
  auto& y = coords.y;

  auto& tile = map.get_tile(coords);
  bool this_is_empty = tile.is_empty_space();
  bool nw_is_empty = ((x > 0) && (y > 0)) ? (viewer->can_see({ x - 1, y - 1 }) && map.get_tile({ x - 1, y - 1 }).is_empty_space()) : false;
  bool n_is_empty = (y > 0) ? (viewer->can_see({ x, y - 1 }) && map.get_tile({ x, y - 1 }).is_empty_space()) : false;
  bool ne_is_empty = ((x < map_size.x - 1) && (y > 0)) ? (viewer->can_see({ x + 1, y - 1 }) && map.get_tile({ x + 1, y - 1 }).is_empty_space()) : false;
  bool e_is_empty = (x < map_size.x - 1) ? (viewer->can_see({ x + 1, y }) && map.get_tile({ x + 1, y }).is_empty_space()) : false;
  bool se_is_empty = ((x < map_size.x - 1) && (y < map_size.y - 1)) ? (viewer->can_see({ x + 1, y + 1 }) && map.get_tile({ x + 1, y + 1 }).is_empty_space()) : false;
  bool s_is_empty = (y < map_size.y - 1) ? (viewer->can_see({ x, y + 1 }) && map.get_tile({ x, y + 1 }).is_empty_space()) : false;
  bool sw_is_empty = ((x > 0) && (y < map_size.y - 1)) ? (viewer->can_see({ x - 1, y + 1 }) && map.get_tile({ x - 1, y + 1 }).is_empty_space()) : false;
  bool w_is_empty = (x > 0) ? (viewer->can_see({ x - 1, y }) && map.get_tile({ x - 1, y }).is_empty_space()) : false;

  if (viewer->can_see(coords))
  {
    if (this_is_empty)
    {
      add_tile_floor_vertices({ x, y });
    }
    else
    {
      tile.add_wall_vertices_to(m_map_seen_vertices, true,
                                nw_is_empty, n_is_empty,
                                ne_is_empty, e_is_empty,
                                se_is_empty, s_is_empty,
                                sw_is_empty, w_is_empty);
    }
  }
  else
  {
    viewer->add_memory_vertices_to(m_map_memory_vertices, { x, y });
  }
}

void MapStandard2DView::add_tile_floor_vertices(Vec2i coords)
{
  auto& config = Service<IConfigSettings>::get();
  auto& map = get_map();

  auto& tile = map.get_tile(coords);
  auto& tileN = map.get_tile(coords + Vec2i(Direction::North));
  auto& tileNE = map.get_tile(coords + Vec2i(Direction::Northeast));
  auto& tileE = map.get_tile(coords + Vec2i(Direction::East));
  auto& tileSE = map.get_tile(coords + Vec2i(Direction::Southeast));
  auto& tileS = map.get_tile(coords + Vec2i(Direction::South));
  auto& tileSW = map.get_tile(coords + Vec2i(Direction::Southwest));
  auto& tileW = map.get_tile(coords + Vec2i(Direction::West));
  auto& tileNW = map.get_tile(coords + Vec2i(Direction::Northwest));

  sf::Vertex new_vertex;
  float ts = config.get<float>("map_tile_size");
  float half_ts = ts * 0.5f;

  sf::Color colorN{ tileN.get_light_level() };
  sf::Color colorNE{ tileNE.get_light_level() };
  sf::Color colorE{ tileE.get_light_level() };
  sf::Color colorSE{ tileSE.get_light_level() };
  sf::Color colorS{ tileS.get_light_level() };
  sf::Color colorSW{ tileSW.get_light_level() };
  sf::Color colorW{ tileW.get_light_level() };
  sf::Color colorNW{ tileNW.get_light_level() };

  sf::Color light = tile.get_light_level();
  sf::Color lightN = average(light, colorN);
  sf::Color lightNE = average(light, colorN, colorNE, colorE);
  sf::Color lightE = average(light, colorE);
  sf::Color lightSE = average(light, colorE, colorSE, colorS);
  sf::Color lightS = average(light, colorS);
  sf::Color lightSW = average(light, colorS, colorSW, colorW);
  sf::Color lightW = average(light, colorW);
  sf::Color lightNW = average(light, colorW, colorNW, colorN);

  Vec2f location{ coords.x * ts, coords.y * ts };
  Vec2f vNE{ location.x + half_ts, location.y - half_ts };
  Vec2f vSE{ location.x + half_ts, location.y + half_ts };
  Vec2f vSW{ location.x - half_ts, location.y + half_ts };
  Vec2f vNW{ location.x - half_ts, location.y - half_ts };

  Vec2u tile_coords = tile.get_tile_sheet_coords();

  the_tilesheet.add_gradient_quad(m_map_seen_vertices, tile_coords,
                                  vNW, vNE,
                                  vSW, vSE,
                                  lightNW, lightN, lightNE,
                                  lightW, light, lightE,
                                  lightSW, lightS, lightSE);
  }

void MapStandard2DView::add_tile_wall_vertices(Vec2i coords)
{
  /// @todo IMPLEMENT ME
}

void MapStandard2DView::add_thing_floor_vertices(ThingId thing,
                                                 bool use_lighting,
                                                 int frame)
{
  auto& config = Service<IConfigSettings>::get();
  sf::Vertex new_vertex;
  float ts = config.get<float>("map_tile_size");
  float ts2 = ts * 0.5f;

  MapTile* root_tile = thing->get_maptile();
  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  Vec2i const& coords = root_tile->get_coords();

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  Vec2f location(coords.x * ts, coords.y * ts);
  Vec2f vSW(location.x - ts2, location.y + ts2);
  Vec2f vSE(location.x + ts2, location.y + ts2);
  Vec2f vNW(location.x - ts2, location.y - ts2);
  Vec2f vNE(location.x + ts2, location.y - ts2);
  Vec2u tile_coords = thing->get_tile_sheet_coords(frame);

  the_tilesheet.add_quad(m_thing_vertices,
                        tile_coords, thing_color,
                        vNW, vNE,
                        vSW, vSE);
}