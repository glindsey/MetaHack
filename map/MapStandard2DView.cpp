#include "stdafx.h"

#include "game/App.h"
#include "map/MapStandard2DView.h"
#include "maptile/MapTileStandard2DView.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/Standard2DGraphicViews.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/New.h"

/// @todo Lighting system should not be a constructor argument, but should be passed into methods that need it instead.

MapStandard2DView::MapStandard2DView(metagui::Desktop& desktop,
                                     std::string name,
                                     Map& map, 
                                     UintVec2 size, 
                                     Standard2DGraphicViews& views)
  :
  MapView(desktop, name, map, size),
  m_views(views)
{
  resetCachedRenderData();

  // Create a grid of tile views, each tied to a map tile.
  m_map_tile_views.reset(new Grid2D<MapTileStandard2DView>(map.getSize(), 
                                                           [&](IntVec2 coords) -> MapTileStandard2DView*
  {
    return NEW MapTileStandard2DView(map.getTile(coords), views);
  }));

}

void MapStandard2DView::updateTiles(EntityId viewer, Systems::Lighting& lighting)
{
  auto& map = getMap();
  auto& map_size = map.getSize();

  static RealVec2 position;

  // Loop through and draw tiles.
  m_mapHorizVertices.clear();
  m_mapVertVertices.clear();
  m_mapMemoryVertices.clear();

  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).addTileVertices(viewer, 
                                                      m_mapHorizVertices, 
                                                      m_mapVertVertices,
                                                      m_mapMemoryVertices,
                                                      lighting);
    }
  }
}

void MapStandard2DView::updateEntities(EntityId viewer, 
                                       Systems::Lighting& lighting,
                                       int frame)
{
  auto& map = getMap();
  auto& map_size = map.getSize();

  // Loop through and draw entities.
  m_entityVertices.clear();
  for (int y = 0; y < map_size.y; ++y)
  {
    for (int x = 0; x < map_size.x; ++x)
    {
      m_map_tile_views->get({ x, y }).addEntitiesVertices(viewer, 
                                                          m_entityVertices,
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
  render_states.texture = &(m_views.getTileSheet().getTexture());

  the_shader.setParameter("effect", ShaderEffect::Lighting);
  texture.draw(m_mapHorizVertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Sepia);
  texture.draw(m_mapMemoryVertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Lighting);
  texture.draw(m_entityVertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Lighting);
  texture.draw(m_mapVertVertices, render_states);

  return true;
}

void MapStandard2DView::draw_highlight(sf::RenderTarget& target,
                                       RealVec2 location,
                                       Color fgColor,
                                       Color bgColor,
                                       int frame)
{
  auto& config = S<IConfigSettings>();
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

void MapStandard2DView::resetCachedRenderData()
{
  auto& map = getMap();
  auto map_size = map.getSize();

  // Size vertex arrays:
  // 4 vertices * 4 quads * 2 for the floor and ceiling = 32
  // 4 vertices * 4 quads * 4 potential walls = 64
  // Memory vertices could be even more than that, once we start showing
  // remembered objects on tiles, but for now we stick with floor/ceiling.

  m_mapHorizVertices.resize(map_size.x * map_size.y * 32);
  m_mapVertVertices.resize(map_size.x * map_size.y * 64);
  m_mapMemoryVertices.resize(map_size.x * map_size.y * 32);

  // Create the vertex arrays.
  m_mapHorizVertices.clear();
  m_mapHorizVertices.setPrimitiveType(sf::PrimitiveType::Quads);
  m_mapVertVertices.clear();
  m_mapVertVertices.setPrimitiveType(sf::PrimitiveType::Quads);
  m_mapMemoryVertices.clear();
  m_mapMemoryVertices.setPrimitiveType(sf::PrimitiveType::Quads);
  m_entityVertices.clear();
  m_entityVertices.setPrimitiveType(sf::PrimitiveType::Quads);
}