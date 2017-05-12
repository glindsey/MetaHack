#include "stdafx.h"

#include "maptile/MapTileStandard2DView.h"

#include "Components/ComponentManager.h"
#include "map/Map.h"
#include "map/MapId.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "systems/SystemManager.h"
#include "systems/SystemSenseSight.h"
#include "tilesheet/TileSheet.h"
#include "types/Color.h"
#include "types/ShaderEffect.h"
#include "utilities/RNGUtils.h"

std::string MapTileStandard2DView::getViewName()
{
  return "standard2D";
}

MapTileStandard2DView::MapTileStandard2DView(MapTile& map_tile, 
                                             TileSheet& tile_sheet)
  :
  MapTileView(map_tile),
  m_tileOffset{ pick_uniform(0, 4) },
  m_tileSheet{ tile_sheet }
{
}

UintVec2 MapTileStandard2DView::get_tile_sheet_coords() const
{
  /// @todo Deal with selecting one of the other tiles.
  UintVec2 start_coords = get_map_tile().getCategoryData().value("tile-location", UintVec2(0, 0));
  UintVec2 tile_coords(start_coords.x + m_tileOffset, start_coords.y);
  return tile_coords;
}

/// @todo Instead of repeating this method in EntityStandard2DView, call the view from here.
UintVec2 MapTileStandard2DView::get_entity_tile_sheet_coords(Entity& entity, int frame) const
{
  /// Get tile coordinates on the sheet.
  UintVec2 start_coords = entity.getCategoryData().value("tile-location", UintVec2(0, 0));

  /// Call the Lua function to get the offset (tile to choose).
  UintVec2 offset = entity.call_lua_function("get_tile_offset", frame, UintVec2(0, 0));

  /// Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
void MapTileStandard2DView::add_tile_vertices(EntityId viewer,
                                              sf::VertexArray& seen_vertices,
                                              sf::VertexArray& memory_vertices,
                                              SystemLighting& lighting)
{
  auto& tile = get_map_tile();
  auto coords = tile.getCoords();
  auto& x = coords.x;
  auto& y = coords.y;

  auto canSee = [viewer](IntVec2 coords) -> bool
  {
    return SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords);
  };

  bool this_is_empty = tile.isPassable();
  bool nw_is_empty = (canSee({ x - 1, y - 1 }) && tile.getAdjacentTile(Direction::Northwest).isPassable());
  bool n_is_empty = (canSee({ x, y - 1 }) && tile.getAdjacentTile(Direction::North).isPassable());
  bool ne_is_empty = (canSee({ x + 1, y - 1 }) && tile.getAdjacentTile(Direction::Northeast).isPassable());
  bool e_is_empty = (canSee({ x + 1, y }) && tile.getAdjacentTile(Direction::East).isPassable());
  bool se_is_empty = (canSee({ x + 1, y + 1 }) && tile.getAdjacentTile(Direction::Southeast).isPassable());
  bool s_is_empty = (canSee({ x, y + 1 }) && tile.getAdjacentTile(Direction::South).isPassable());
  bool sw_is_empty = (canSee({ x - 1, y + 1 }) && tile.getAdjacentTile(Direction::Southwest).isPassable());
  bool w_is_empty = (canSee({ x - 1, y }) && tile.getAdjacentTile(Direction::West).isPassable());

  if (canSee(coords))
  {
    if (this_is_empty)
    {
      add_tile_floor_vertices(seen_vertices, lighting);
    }
    else
    {
      add_wall_vertices_to(seen_vertices, &lighting,
                           nw_is_empty, n_is_empty,
                           ne_is_empty, e_is_empty,
                           se_is_empty, s_is_empty,
                           sw_is_empty, w_is_empty);
    }
  }
  else
  {
    add_memory_vertices_to(memory_vertices, viewer);
  }
}

void MapTileStandard2DView::add_memory_vertices_to(sf::VertexArray& vertices,
                                                   EntityId viewer)
{
  if (!COMPONENTS.position.existsFor(viewer) || !COMPONENTS.spacialMemory.existsFor(viewer)) return;

  auto& config = Service<IConfigSettings>::get();
  auto& tile = get_map_tile();
  auto coords = tile.getCoords();
  auto& viewerPosition = COMPONENTS.position[viewer];
  MapId map = viewerPosition.map();

  if (map == MapFactory::null_map_id)
  {
    return;
  }

  auto& spacialMemory = COMPONENTS.spacialMemory[viewer].ofMap(map);
  static sf::Vertex new_vertex;
  float ts = config.get("map-tile-size");
  float ts2 = ts * 0.5f;

  RealVec2 location(coords.x * ts, coords.y * ts);
  RealVec2 vSW(location.x - ts2, location.y + ts2);
  RealVec2 vSE(location.x + ts2, location.y + ts2);
  RealVec2 vNW(location.x - ts2, location.y - ts2);
  RealVec2 vNE(location.x + ts2, location.y - ts2);

  std::string tile_type = spacialMemory[coords].getType();
  if (tile_type == "") { tile_type = "MTUnknown"; }
  json& tile_data = Service<IGameRules>::get().category(tile_type);

  /// @todo Call a script to handle selecting a tile other than the one
  ///       in the upper-left corner.
  UintVec2 tile_coords = tile_data.value("tile-location", UintVec2(0, 0));

  m_tileSheet.add_quad(vertices,
                        tile_coords, Color::White,
                        vNW, vNE,
                        vSW, vSE);
}

void MapTileStandard2DView::add_tile_floor_vertices(sf::VertexArray& vertices,
                                                    SystemLighting& lighting)
{
  auto& config = Service<IConfigSettings>::get();

  auto& tile = get_map_tile();
  auto& coords = tile.getCoords();

  sf::Vertex new_vertex;
  float ts = config.get("map-tile-size");
  float half_ts = ts * 0.5f;

  Color colorN{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::North)) };
  Color colorNE{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::Northeast)) };
  Color colorE{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::East)) };
  Color colorSE{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::Southeast)) };
  Color colorS{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::South)) };
  Color colorSW{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::Southwest)) };
  Color colorW{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::West)) };
  Color colorNW{ lighting.getLightLevel(coords + static_cast<IntVec2>(Direction::Northwest)) };

  Color light = lighting.getLightLevel(coords);
  Color lightN = average(light, colorN);
  Color lightNE = average(light, colorN, colorNE, colorE);
  Color lightE = average(light, colorE);
  Color lightSE = average(light, colorE, colorSE, colorS);
  Color lightS = average(light, colorS);
  Color lightSW = average(light, colorS, colorSW, colorW);
  Color lightW = average(light, colorW);
  Color lightNW = average(light, colorW, colorNW, colorN);
  
  RealVec2 location{ coords.x * ts, coords.y * ts };
  RealVec2 vNE{ location.x + half_ts, location.y - half_ts };
  RealVec2 vSE{ location.x + half_ts, location.y + half_ts };
  RealVec2 vSW{ location.x - half_ts, location.y + half_ts };
  RealVec2 vNW{ location.x - half_ts, location.y - half_ts };

  UintVec2 tile_coords = get_tile_sheet_coords();

  m_tileSheet.add_gradient_quad(vertices, tile_coords,
                                 vNW, vNE,
                                 vSW, vSE,
                                 lightNW, lightN, lightNE,
                                 lightW, light, lightE,
                                 lightSW, lightS, lightSE);
}

void MapTileStandard2DView::add_things_floor_vertices(EntityId viewer,
                                                      sf::VertexArray & vertices,
                                                      SystemLighting* lighting,
                                                      int frame)
{
  auto& tile = get_map_tile();
  auto coords = tile.getCoords();
  EntityId contents = tile.getTileContents();
  ComponentInventory& inv = COMPONENTS.inventory[contents];

  if (SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords))
  {
    if (inv.count() > 0)
    {
      // Only draw the largest entity on that tile.
      EntityId biggest_thing = inv.get_largest_thing();
      if (biggest_thing != EntityId::Mu())
      {
        add_thing_floor_vertices(biggest_thing, vertices, lighting, frame);
      }
    }
  }

  // Always draw the viewer itself last, if it is present at that tile.
  if (inv.contains(viewer))
  {
    add_thing_floor_vertices(viewer, vertices, lighting, frame);
  }
}

void MapTileStandard2DView::add_thing_floor_vertices(EntityId entityId, 
                                                     sf::VertexArray& vertices,
                                                     SystemLighting* lighting,
                                                     int frame)
{
  auto& entity = GAME.entities().get(entityId);
  auto& config = Service<IConfigSettings>::get();
  sf::Vertex new_vertex;
  float ts = config.get("map-tile-size");
  float ts2 = ts * 0.5f;

  // If this entity doesn't have a Position component, bail.
  if (!COMPONENTS.position.existsFor(entityId)) return;

  auto& position = COMPONENTS.position[entityId];
  IntVec2 const& coords = position.coords();
//  MapTile& tile = position.map()->getTile(coords);

  Color thing_color;
  if (lighting != nullptr)
  {
    thing_color = lighting->getLightLevel(coords);
  }
  else
  {
    thing_color = Color::White;
  }

  RealVec2 location(coords.x * ts, coords.y * ts);
  RealVec2 vSW(location.x - ts2, location.y + ts2);
  RealVec2 vSE(location.x + ts2, location.y + ts2);
  RealVec2 vNW(location.x - ts2, location.y - ts2);
  RealVec2 vNE(location.x + ts2, location.y - ts2);
  UintVec2 tile_coords = get_entity_tile_sheet_coords(entity, frame);

  m_tileSheet.add_quad(vertices,
                        tile_coords, thing_color,
                        vNW, vNE,
                        vSW, vSE);
}


void MapTileStandard2DView::add_wall_vertices_to(sf::VertexArray& vertices,
                                                 SystemLighting* lighting,
                                                 bool nw_is_empty, bool n_is_empty,
                                                 bool ne_is_empty, bool e_is_empty,
                                                 bool se_is_empty, bool s_is_empty,
                                                 bool sw_is_empty, bool w_is_empty)
{
  auto& config = Service<IConfigSettings>::get();
  float map_tile_size = config.get("map-tile-size");

  // This tile.
  MapTile& tile = get_map_tile();

  // Checks to see N/S/E/W walls.
  bool playerSeesNWall{ false };
  bool playerSeesSWall{ false };
  bool playerSeesEWall{ false };
  bool playerSeesWWall{ false };

  // Player.
  EntityId player = GAME.getPlayer();
  bool playerHasLocation = COMPONENTS.position.existsFor(player);

  if (player != EntityId::Mu() && playerHasLocation)
  {
    auto& playerLocation = COMPONENTS.position[player];

    if (playerLocation.map() == tile.map())
    {
      IntVec2 playerCoords = playerLocation.coords();
      IntVec2 tileCoords = tile.getCoords();

      playerSeesNWall = (playerCoords.y <= tileCoords.y);
      playerSeesSWall = (playerCoords.y >= tileCoords.y);
      playerSeesEWall = (playerCoords.x >= tileCoords.x);
      playerSeesWWall = (playerCoords.x <= tileCoords.x);
    }
  }

  // Tile color.
  Color tile_color{ Color::White };

  // Wall colors.
  Color wall_n_color_w{ Color::White };
  Color wall_n_color{ Color::White };
  Color wall_n_color_e{ Color::White };

  Color wall_e_color_n{ Color::White };
  Color wall_e_color{ Color::White };
  Color wall_e_color_s{ Color::White };

  Color wall_s_color_w{ Color::White };
  Color wall_s_color{ Color::White };
  Color wall_s_color_e{ Color::White };

  Color wall_w_color_n{ Color::White };
  Color wall_w_color{ Color::White };
  Color wall_w_color_s{ Color::White };

  // Full tile size.
  float ts(map_tile_size);

  // Half of the tile size.
  float half_ts(map_tile_size * 0.5f);

  // Wall size (configurable).
  float ws(map_tile_size * 0.4f);

  // Adjacent tile coordinates
  IntVec2 coords = tile.getCoords();
  IntVec2 coordsN = coords + static_cast<IntVec2>(Direction::North);
  IntVec2 coordsE = coords + static_cast<IntVec2>(Direction::East);
  IntVec2 coordsS = coords + static_cast<IntVec2>(Direction::South);
  IntVec2 coordsW = coords + static_cast<IntVec2>(Direction::West);

  // Tile vertices.
  RealVec2 location(coords.x * ts, coords.y * ts);
  RealVec2 vTileN(location.x, location.y - half_ts);
  RealVec2 vTileNE(location.x + half_ts, location.y - half_ts);
  RealVec2 vTileE(location.x + half_ts, location.y);
  RealVec2 vTileSE(location.x + half_ts, location.y + half_ts);
  RealVec2 vTileS(location.x, location.y + half_ts);
  RealVec2 vTileSW(location.x - half_ts, location.y + half_ts);
  RealVec2 vTileW(location.x - half_ts, location.y);
  RealVec2 vTileNW(location.x - half_ts, location.y - half_ts);

  UintVec2 tile_sheet_coords = this->get_tile_sheet_coords();

  if (lighting != nullptr)
  {
    tile_color = lighting->getLightLevel(coords);

    if (playerSeesNWall)
    {
      wall_n_color = lighting->getWallLightLevel(coords, Direction::North);
      wall_n_color_w = average(wall_n_color, lighting->getWallLightLevel(coordsW, Direction::North));
      wall_n_color_e = average(wall_n_color, lighting->getWallLightLevel(coordsE, Direction::North));
    }
    else
    {
      wall_n_color = Color::Black;
      wall_n_color_w = Color::Black;
      wall_n_color_e = Color::Black;
    }

    if (playerSeesEWall)
    {
      wall_e_color = lighting->getWallLightLevel(coords, Direction::East);
      wall_e_color_n = average(wall_e_color, lighting->getWallLightLevel(coordsN, Direction::East));
      wall_e_color_s = average(wall_e_color, lighting->getWallLightLevel(coordsS, Direction::East));
    }
    else
    {
      wall_e_color = Color::Black;
      wall_e_color_n = Color::Black;
      wall_e_color_s = Color::Black;
    }

    if (playerSeesSWall)
    {
      wall_s_color = lighting->getWallLightLevel(coords, Direction::South);
      wall_s_color_w = average(wall_s_color, lighting->getWallLightLevel(coordsW, Direction::South));
      wall_s_color_e = average(wall_s_color, lighting->getWallLightLevel(coordsE, Direction::South));
    }
    else
    {
      wall_s_color = Color::Black;
      wall_s_color_w = Color::Black;
      wall_s_color_e = Color::Black;
    }

    if (playerSeesWWall)
    {
      wall_w_color = lighting->getWallLightLevel(coords, Direction::West);
      wall_w_color_n = average(wall_w_color, lighting->getWallLightLevel(coordsN, Direction::West));
      wall_w_color_s = average(wall_w_color, lighting->getWallLightLevel(coordsS, Direction::West));
    }
    else
    {
      wall_w_color = Color::Black;
      wall_w_color_n = Color::Black;
      wall_w_color_s = Color::Black;
    }
  }

  // NORTH WALL
  if (n_is_empty)
  {
    RealVec2 vS(vTileN.x, vTileN.y + ws);
    RealVec2 vSW(vTileNW.x, vTileNW.y + ws);
    if (w_is_empty)
    {
      vSW.x += ws;
    }
    else if (!nw_is_empty)
    {
      vSW.x -= ws;
    }
    RealVec2 vSE(vTileNE.x, vTileNE.y + ws);
    if (e_is_empty)
    {
      vSE.x -= ws;
    }
    else if (!ne_is_empty)
    {
      vSE.x += ws;
    }

    m_tileSheet.add_gradient_quad(vertices, tile_sheet_coords,
                                   vTileNW, vTileNE,
                                   vSW, vSE,
                                   wall_n_color_w, wall_n_color, wall_n_color_e,
                                   wall_n_color_w, wall_n_color, wall_n_color_e,
                                   wall_n_color_w, wall_n_color, wall_n_color_e);
  }

  // EAST WALL
  if (e_is_empty)
  {
    RealVec2 vW(vTileE.x - ws, vTileE.y);
    RealVec2 vNW(vTileNE.x - ws, vTileNE.y);
    if (n_is_empty)
    {
      vNW.y += ws;
    }
    else if (!ne_is_empty)
    {
      vNW.y -= ws;
    }
    RealVec2 vSW(vTileSE.x - ws, vTileSE.y);
    if (s_is_empty)
    {
      vSW.y -= ws;
    }
    else if (!se_is_empty)
    {
      vSW.y += ws;
    }

    m_tileSheet.add_gradient_quad(vertices, tile_sheet_coords,
                                   vNW, vTileNE,
                                   vSW, vTileSE,
                                   wall_e_color_n, wall_e_color_n, wall_e_color_n,
                                   wall_e_color, wall_e_color, wall_e_color,
                                   wall_e_color_s, wall_e_color_s, wall_e_color_s);
  }

  // SOUTH WALL
  if (s_is_empty)
  {
    RealVec2 vN(vTileS.x, vTileS.y - ws);
    RealVec2 vNW(vTileSW.x, vTileSW.y - ws);
    if (w_is_empty)
    {
      vNW.x += ws;
    }
    else if (!sw_is_empty)
    {
      vNW.x -= ws;
    }
    RealVec2 vNE(vTileSE.x, vTileSE.y - ws);
    if (e_is_empty)
    {
      vNE.x -= ws;
    }
    else if (!se_is_empty)
    {
      vNE.x += ws;
    }

    m_tileSheet.add_gradient_quad(vertices, tile_sheet_coords,
                                   vNW, vNE,
                                   vTileSW, vTileSE,
                                   wall_s_color_w, wall_s_color, wall_s_color_e,
                                   wall_s_color_w, wall_s_color, wall_s_color_e,
                                   wall_s_color_w, wall_s_color, wall_s_color_e);
  }

  // WEST WALL
  if (w_is_empty)
  {
    RealVec2 vE(vTileW.x + ws, vTileW.y);
    RealVec2 vNE(vTileNW.x + ws, vTileNW.y);
    if (n_is_empty)
    {
      vNE.y += ws;
    }
    else if (!nw_is_empty)
    {
      vNE.y -= ws;
    }
    RealVec2 vSE(vTileSW.x + ws, vTileSW.y);
    if (s_is_empty)
    {
      vSE.y -= ws;
    }
    else if (!sw_is_empty)
    {
      vSE.y += ws;
    }

    m_tileSheet.add_gradient_quad(vertices, tile_sheet_coords,
                                   vTileNW, vNE,
                                   vTileSW, vSE,
                                   wall_w_color_n, wall_w_color_n, wall_w_color_n,
                                   wall_w_color, wall_w_color, wall_w_color,
                                   wall_w_color_s, wall_w_color_s, wall_w_color_s);
  }
}

EventResult MapTileStandard2DView::onEvent_NVI(Event const & event)
{
  return{ EventHandled::Yes, ContinueBroadcasting::Yes };
}

