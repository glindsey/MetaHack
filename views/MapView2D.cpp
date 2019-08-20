#include "stdafx.h"

#include "views/MapView2D.h"

#include "config/Settings.h"
#include "game/App.h"
#include "map/Map.h"
#include "tilesheet/TileSheet.h"
#include "types/ShaderEffect.h"
#include "utilities/New.h"
#include "views/MapTileView2D.h"

MapView2D::MapView2D(std::string name,
                     Map& map)
  :
  MapView(name, map)
{
  resetCachedRenderData();

  // Create a grid of tile views, each tied to a map tile.
  m_map_tile_views.reset(new Grid2D<MapTileView2D>(map.getSize(),
                         [&](IntVec2 coords) -> MapTileView2D*
  {
    return NEW MapTileView2D(map.getTile(coords));
  }));

}

MapView2D::~MapView2D()
{
}

void MapView2D::updateTiles(EntityId viewer, Systems::Lighting& lighting)
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

void MapView2D::updateEntities(EntityId viewer,
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

bool MapView2D::renderMap(sf::RenderTexture& texture, int frame)
{
  the_shader.setUniform("texture", sf::Shader::CurrentTexture);

  sf::RenderStates render_states = sf::RenderStates::Default;
  render_states.shader = &the_shader;
  render_states.texture = &(the_tilesheet.getTexture());

  the_shader.setUniform("effect", ShaderEffect::Lighting);
  texture.draw(m_mapHorizVertices, render_states);
  the_shader.setUniform("effect", ShaderEffect::Sepia);
  texture.draw(m_mapMemoryVertices, render_states);
  the_shader.setUniform("effect", ShaderEffect::Lighting);
  texture.draw(m_entityVertices, render_states);
  the_shader.setUniform("effect", ShaderEffect::Lighting);
  texture.draw(m_mapVertVertices, render_states);

  return true;
}

void MapView2D::drawHighlight(sf::RenderTarget& target,
                                       RealVec2 location,
                                       Color fgColor,
                                       Color bgColor,
                                       int frame)
{
  auto& config = Config::settings();
  RealVec2 mapTileSize = config.get("map-tile-size");

  RealVec2 halfTs = { mapTileSize.x * 0.5f, mapTileSize.y * 0.5f };
  RealVec2 vSW(location.x - halfTs.x, location.y + halfTs.y);
  RealVec2 vSE(location.x + halfTs.x, location.y + halfTs.y);
  RealVec2 vNW(location.x - halfTs.x, location.y - halfTs.y);
  RealVec2 vNE(location.x + halfTs.x, location.y - halfTs.y);

  sf::RectangleShape boxShape;
  RealVec2 boxPosition;
  RealVec2 boxSize = mapTileSize;
  RealVec2 boxHalfSize(boxSize.x / 2, boxSize.y / 2);
  boxPosition.x = (location.x - boxHalfSize.x);
  boxPosition.y = (location.y - boxHalfSize.y);
  boxShape.setPosition(boxPosition);
  boxShape.setSize(boxSize);
  boxShape.setOutlineColor(fgColor);
  boxShape.setOutlineThickness(config.get("tile-highlight-border-width"));
  boxShape.setFillColor(bgColor);

  target.draw(boxShape);
}

std::string MapView2D::getViewName()
{
  return "standard2D";
}


void MapView2D::drawPreChildren_(sf::RenderTexture & texture, int frame)
{
  /// @todo WRITE ME
}

void MapView2D::resetCachedRenderData()
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
