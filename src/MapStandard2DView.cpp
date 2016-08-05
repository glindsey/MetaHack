#include "stdafx.h"

#include "MapStandard2DView.h"

#include "ShaderEffect.h"

MapStandard2DView::MapStandard2DView()
  :
  MapView()
{}

MapStandard2DView::MapStandard2DView(MapId map_id)
  :
  MapView(map_id)
{}

void MapStandard2DView::update_tiles(ThingId viewer)
{
  auto location = viewer->get_location();
  auto& map = get_map();
  auto& map_size = map.get_size();

  static sf::Vector2f position;

  // Loop through and draw tiles.
  m_map_seen_vertices.clear();
  m_map_memory_vertices.clear();

  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      auto& tile = map.get_tile(x, y);
      bool this_is_empty = tile.is_empty_space();
      bool nw_is_empty = ((x > 0) && (y > 0)) ? (viewer->can_see(x - 1, y - 1) && map.get_tile(x - 1, y - 1).is_empty_space()) : false;
      bool n_is_empty = (y > 0) ? (viewer->can_see(x, y - 1) && map.get_tile(x, y - 1).is_empty_space()) : false;
      bool ne_is_empty = ((x < map_size.x - 1) && (y > 0)) ? (viewer->can_see(x + 1, y - 1) && map.get_tile(x + 1, y - 1).is_empty_space()) : false;
      bool e_is_empty = (x < map_size.x - 1) ? (viewer->can_see(x + 1, y) && map.get_tile(x + 1, y).is_empty_space()) : false;
      bool se_is_empty = ((x < map_size.x - 1) && (y < map_size.y - 1)) ? (viewer->can_see(x + 1, y + 1) && map.get_tile(x + 1, y + 1).is_empty_space()) : false;
      bool s_is_empty = (y < map_size.y - 1) ? (viewer->can_see(x, y + 1) && map.get_tile(x, y + 1).is_empty_space()) : false;
      bool sw_is_empty = ((x > 0) && (y < map_size.y - 1)) ? (viewer->can_see(x - 1, y + 1) && map.get_tile(x - 1, y + 1).is_empty_space()) : false;
      bool w_is_empty = (x > 0) ? (viewer->can_see(x - 1, y) && map.get_tile(x - 1, y).is_empty_space()) : false;

      if (viewer->can_see(x, y))
      {
        if (this_is_empty)
        {
          tile.add_floor_vertices_to(m_map_seen_vertices, true);
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
        viewer->add_memory_vertices_to(m_map_memory_vertices, x, y);
      }
    } // end for (int x)
  } // end for (int y)
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
      ThingId contents = map.get_tile(x, y).get_tile_contents();
      Inventory& inv = contents->get_inventory();

      if (viewer->can_see(x, y))
      {
        if (inv.count() > 0)
        {
          ThingId biggest_thing = inv.get_largest_thing();
          if (biggest_thing != ThingId::Mu())
          {
            biggest_thing->add_floor_vertices_to(m_thing_vertices, true, frame);
          }
        }
      }

      if (inv.contains(viewer))
      {
        viewer->add_floor_vertices_to(m_thing_vertices, true, frame);
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