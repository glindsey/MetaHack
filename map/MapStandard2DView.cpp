#include "stdafx.h"

#include "map/MapStandard2DView.h"
#include "maptile/MapTileStandard2DView.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/New.h"

/// @todo Lighting system should not be a constructor argument, but should be passed into methods that need it instead.

MapStandard2DView::MapStandard2DView(std::string name, 
                                     Map& map, 
                                     UintVec2 size, 
                                     TileSheet& tileSheet)
  :
  MapView(name, map, size),
  m_tileSheet(tileSheet)
{
  reset_cached_render_data();

  // Create a grid of tile views, each tied to a map tile.
  m_map_tile_views.reset(new Grid2D<MapTileStandard2DView>(map.getSize(), 
                                                           [&](IntVec2 coords) -> MapTileStandard2DView*
  {
    return NEW MapTileStandard2DView(map.getTile(coords), m_tileSheet);
  }));

}

void MapStandard2DView::update_tiles(EntityId viewer, Systems::SystemLighting& lighting)
{
  auto& map = getMap();
  auto& map_size = map.getSize();

  static RealVec2 position;

  // Loop through and draw tiles.
  m_map_seen_vertices.clear();
  m_map_memory_vertices.clear();

  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).addTileVertices(viewer, 
                                                      m_map_seen_vertices, 
                                                      m_map_memory_vertices,
                                                      lighting);
    }
  }
}

void MapStandard2DView::update_things(EntityId viewer, 
                                      Systems::SystemLighting& lighting,
                                      int frame)
{
  auto& map = getMap();
  auto& map_size = map.getSize();

  // Loop through and draw entities.
  m_thing_vertices.clear();
  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).addEntitiesFloorVertices(viewer, 
                                                                m_thing_vertices, 
                                                                &lighting, 
                                                                frame);
    }
  }
}

bool MapStandard2DView::render_map(sf::RenderTexture& texture, int frame)
{
  the_shader.setParameter("texture", sf::Shader::CurrentTexture);

  sf::RenderStates render_states = sf::RenderStates::Default;
  render_states.shader = &the_shader;
  render_states.texture = &(m_tileSheet.getTexture());

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
                                       RealVec2 location,
                                       Color fgColor,
                                       Color bgColor,
                                       int frame)
{
  auto& config = Service<IConfigSettings>::get();
  float map_tile_size = config.get("map-tile-size");

  float half_ts(map_tile_size * 0.5f);
  RealVec2 vSW(location.x - half_ts, location.y + half_ts);
  RealVec2 vSE(location.x + half_ts, location.y + half_ts);
  RealVec2 vNW(location.x - half_ts, location.y - half_ts);
  RealVec2 vNE(location.x + half_ts, location.y - half_ts);

  sf::RectangleShape box_shape;
  RealVec2 box_position;
  RealVec2 box_size(map_tile_size, map_tile_size);
  RealVec2 box_half_size(box_size.x / 2, box_size.y / 2);
  box_position.x = (location.x - box_half_size.x);
  box_position.y = (location.y - box_half_size.y);
  box_shape.setPosition(box_position);
  box_shape.setSize(box_size);
  box_shape.setOutlineColor(fgColor);
  box_shape.setOutlineThickness(config.get("tile-highlight-border-width"));
  box_shape.setFillColor(bgColor);

  target.draw(box_shape);
}

std::string MapStandard2DView::getViewName()
{
  return "standard2D";
}


void MapStandard2DView::drawPreChildren_(sf::RenderTexture & texture, int frame)
{
  /// @todo WRITE ME
}

void MapStandard2DView::reset_cached_render_data()
{
  auto& map = getMap();
  auto map_size = map.getSize();

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