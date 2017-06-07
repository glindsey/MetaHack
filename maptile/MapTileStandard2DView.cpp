#include "stdafx.h"

#include "maptile/MapTileStandard2DView.h"

#include "Components/ComponentManager.h"
#include "map/Map.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "systems/Manager.h"
#include "systems/SystemLighting.h"
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

UintVec2 MapTileStandard2DView::getTileSheetCoords() const
{
  /// @todo Deal with selecting one of the other tiles.
  UintVec2 start_coords = getMapTile().getCategoryData().value("tile-location", UintVec2(0, 0));
  UintVec2 tile_coords(start_coords.x + m_tileOffset, start_coords.y);
  return tile_coords;
}

/// @todo Instead of repeating this method in EntityStandard2DView, call the view from here.
UintVec2 MapTileStandard2DView::getEntityTileSheetCoords(Entity& entity, int frame) const
{
  /// Get tile coordinates on the sheet.
  UintVec2 start_coords = entity.getCategoryData().value("tile-location", UintVec2(0, 0));

  /// Call the Lua function to get the offset (tile to choose).
  UintVec2 offset = entity.call_lua_function("get_tile_offset", frame, UintVec2(0, 0));

  /// Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
void MapTileStandard2DView::addTileVertices(EntityId viewer,
                                            sf::VertexArray& seenVertices,
                                            sf::VertexArray& memoryVertices,
                                            Systems::Lighting& lighting)
{
  auto& tile = getMapTile();
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
      addTileFloorVerticesTo(seenVertices, lighting);
    }
    else
    {
      addWallVerticesTo(seenVertices, &lighting,
                        nw_is_empty, n_is_empty,
                        ne_is_empty, e_is_empty,
                        se_is_empty, s_is_empty,
                        sw_is_empty, w_is_empty);
    }
  }
  else
  {
    addMemoryVerticesTo(memoryVertices, viewer);
  }
}

void MapTileStandard2DView::addMemoryVerticesTo(sf::VertexArray& vertices,
                                                EntityId viewer)
{
  if (!COMPONENTS.position.existsFor(viewer) || !COMPONENTS.spacialMemory.existsFor(viewer)) return;

  auto& config = Service<IConfigSettings>::get();
  auto& tile = getMapTile();
  auto coords = tile.getCoords();
  auto& viewerPosition = COMPONENTS.position[viewer];
  MapID map = viewerPosition.map();

  if (map.empty())
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

  m_tileSheet.addQuad(vertices,
                        tile_coords, Color::White,
                        vNW, vNE,
                        vSW, vSE);
}

void MapTileStandard2DView::addTileFloorVerticesTo(sf::VertexArray& vertices,
                                                   Systems::Lighting& lighting)
{
  auto& config = Service<IConfigSettings>::get();

  auto& tile = getMapTile();
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

  UintVec2 tile_coords = getTileSheetCoords();

  m_tileSheet.addGradientQuadTo(vertices, tile_coords,
                                vNW, vNE,
                                vSW, vSE,
                                lightNW, lightN, lightNE,
                                lightW, light, lightE,
                                lightSW, lightS, lightSE);
}

void MapTileStandard2DView::addEntitiesFloorVertices(EntityId viewer,
                                                     sf::VertexArray & vertices,
                                                     Systems::Lighting* lighting,
                                                     int frame)
{
  auto& tile = getMapTile();
  auto coords = tile.getCoords();
  EntityId contents = tile.getTileContents();
  auto& inv = COMPONENTS.inventory[contents];

  if (SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords))
  {
    if (inv.count() > 0)
    {
      // Only draw the largest entity on that tile.
      EntityId biggestEntity = inv.getLargestEntity();
      if (biggestEntity != EntityId::Mu())
      {
        addEntityFloorVertices(biggestEntity, vertices, lighting, frame);
      }
    }
  }

  // Always draw the viewer itself last, if it is present at that tile.
  if (inv.contains(viewer))
  {
    addEntityFloorVertices(viewer, vertices, lighting, frame);
  }
}

void MapTileStandard2DView::addEntityFloorVertices(EntityId entityId, 
                                                   sf::VertexArray& vertices,
                                                   Systems::Lighting* lighting,
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

  Color thingColor;
  if (lighting != nullptr)
  {
    thingColor = lighting->getLightLevel(coords);
  }
  else
  {
    thingColor = Color::White;
  }

  RealVec2 location(coords.x * ts, coords.y * ts);
  RealVec2 vSW(location.x - ts2, location.y + ts2);
  RealVec2 vSE(location.x + ts2, location.y + ts2);
  RealVec2 vNW(location.x - ts2, location.y - ts2);
  RealVec2 vNE(location.x + ts2, location.y - ts2);
  UintVec2 tileCoords = getEntityTileSheetCoords(entity, frame);

  m_tileSheet.addQuad(vertices,
                      tileCoords, thingColor,
                      vNW, vNE,
                      vSW, vSE);
}


void MapTileStandard2DView::addWallVerticesTo(sf::VertexArray& vertices,
                                              Systems::Lighting* lighting,
                                              bool nwEmpty, bool nEmpty,
                                              bool neEmpty, bool eEmpty,
                                              bool seEmpty, bool sEmpty,
                                              bool swEmpty, bool wEmpty)
{
  auto& config = Service<IConfigSettings>::get();
  float mapTileSize = config.get("map-tile-size");

  // This tile.
  MapTile& tile = getMapTile();

  // Checks to see N/S/E/W walls.
  bool playerSeesNWall{ false };
  bool playerSeesSWall{ false };
  bool playerSeesEWall{ false };
  bool playerSeesWWall{ false };

  // Player.
  EntityId player = COMPONENTS.globals.player();
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
  Color tileColor{ Color::White };

  // Wall colors.
  Color wallNColorW{ Color::White };
  Color wallNColorC{ Color::White };
  Color wallNColorE{ Color::White };

  Color wallEColorN{ Color::White };
  Color wallEColorC{ Color::White };
  Color wallEColorS{ Color::White };

  Color wallSColorW{ Color::White };
  Color wallSColorC{ Color::White };
  Color wallSColorE{ Color::White };

  Color wallWColorN{ Color::White };
  Color wallWColorC{ Color::White };
  Color wallWColorS{ Color::White };

  // Full tile size.
  float ts(mapTileSize);

  // Half of the tile size.
  float halfTs(mapTileSize * 0.5f);

  // Wall size (configurable).
  float ws(mapTileSize * 0.4f);

  // Adjacent tile coordinates
  IntVec2 coords = tile.getCoords();
  IntVec2 coordsN = coords + static_cast<IntVec2>(Direction::North);
  IntVec2 coordsE = coords + static_cast<IntVec2>(Direction::East);
  IntVec2 coordsS = coords + static_cast<IntVec2>(Direction::South);
  IntVec2 coordsW = coords + static_cast<IntVec2>(Direction::West);

  // Tile vertices.
  RealVec2 location(coords.x * ts, coords.y * ts);
  RealVec2 vTileN(location.x, location.y - halfTs);
  RealVec2 vTileNE(location.x + halfTs, location.y - halfTs);
  RealVec2 vTileE(location.x + halfTs, location.y);
  RealVec2 vTileSE(location.x + halfTs, location.y + halfTs);
  RealVec2 vTileS(location.x, location.y + halfTs);
  RealVec2 vTileSW(location.x - halfTs, location.y + halfTs);
  RealVec2 vTileW(location.x - halfTs, location.y);
  RealVec2 vTileNW(location.x - halfTs, location.y - halfTs);

  UintVec2 tileSheetCoords = this->getTileSheetCoords();

  if (lighting != nullptr)
  {
    tileColor = lighting->getLightLevel(coords);

    if (playerSeesNWall)
    {
      wallNColorC = lighting->getWallLightLevel(coords, Direction::North);
      wallNColorW = average(wallNColorC, lighting->getWallLightLevel(coordsW, Direction::North));
      wallNColorE = average(wallNColorC, lighting->getWallLightLevel(coordsE, Direction::North));
    }
    else
    {
      wallNColorC = Color::Black;
      wallNColorW = Color::Black;
      wallNColorE = Color::Black;
    }

    if (playerSeesEWall)
    {
      wallEColorC = lighting->getWallLightLevel(coords, Direction::East);
      wallEColorN = average(wallEColorC, lighting->getWallLightLevel(coordsN, Direction::East));
      wallEColorS = average(wallEColorC, lighting->getWallLightLevel(coordsS, Direction::East));
    }
    else
    {
      wallEColorC = Color::Black;
      wallEColorN = Color::Black;
      wallEColorS = Color::Black;
    }

    if (playerSeesSWall)
    {
      wallSColorC = lighting->getWallLightLevel(coords, Direction::South);
      wallSColorW = average(wallSColorC, lighting->getWallLightLevel(coordsW, Direction::South));
      wallSColorE = average(wallSColorC, lighting->getWallLightLevel(coordsE, Direction::South));
    }
    else
    {
      wallSColorC = Color::Black;
      wallSColorW = Color::Black;
      wallSColorE = Color::Black;
    }

    if (playerSeesWWall)
    {
      wallWColorC = lighting->getWallLightLevel(coords, Direction::West);
      wallWColorN = average(wallWColorC, lighting->getWallLightLevel(coordsN, Direction::West));
      wallWColorS = average(wallWColorC, lighting->getWallLightLevel(coordsS, Direction::West));
    }
    else
    {
      wallWColorC = Color::Black;
      wallWColorN = Color::Black;
      wallWColorS = Color::Black;
    }
  }

  // NORTH WALL
  if (nEmpty)
  {
    RealVec2 vS(vTileN.x, vTileN.y + ws);
    RealVec2 vSW(vTileNW.x, vTileNW.y + ws);
    if (wEmpty)
    {
      vSW.x += ws;
    }
    else if (!nwEmpty)
    {
      vSW.x -= ws;
    }
    RealVec2 vSE(vTileNE.x, vTileNE.y + ws);
    if (eEmpty)
    {
      vSE.x -= ws;
    }
    else if (!neEmpty)
    {
      vSE.x += ws;
    }

    m_tileSheet.addGradientQuadTo(vertices, tileSheetCoords,
                                  vTileNW, vTileNE,
                                  vSW, vSE,
                                  wallNColorW, wallNColorC, wallNColorE,
                                  wallNColorW, wallNColorC, wallNColorE,
                                  wallNColorW, wallNColorC, wallNColorE);
  }

  // EAST WALL
  if (eEmpty)
  {
    RealVec2 vW(vTileE.x - ws, vTileE.y);
    RealVec2 vNW(vTileNE.x - ws, vTileNE.y);
    if (nEmpty)
    {
      vNW.y += ws;
    }
    else if (!neEmpty)
    {
      vNW.y -= ws;
    }
    RealVec2 vSW(vTileSE.x - ws, vTileSE.y);
    if (sEmpty)
    {
      vSW.y -= ws;
    }
    else if (!seEmpty)
    {
      vSW.y += ws;
    }

    m_tileSheet.addGradientQuadTo(vertices, tileSheetCoords,
                                  vNW, vTileNE,
                                  vSW, vTileSE,
                                  wallEColorN, wallEColorN, wallEColorN,
                                  wallEColorC, wallEColorC, wallEColorC,
                                  wallEColorS, wallEColorS, wallEColorS);
  }

  // SOUTH WALL
  if (sEmpty)
  {
    RealVec2 vN(vTileS.x, vTileS.y - ws);
    RealVec2 vNW(vTileSW.x, vTileSW.y - ws);
    if (wEmpty)
    {
      vNW.x += ws;
    }
    else if (!swEmpty)
    {
      vNW.x -= ws;
    }
    RealVec2 vNE(vTileSE.x, vTileSE.y - ws);
    if (eEmpty)
    {
      vNE.x -= ws;
    }
    else if (!seEmpty)
    {
      vNE.x += ws;
    }

    m_tileSheet.addGradientQuadTo(vertices, tileSheetCoords,
                                  vNW, vNE,
                                  vTileSW, vTileSE,
                                  wallSColorW, wallSColorC, wallSColorE,
                                  wallSColorW, wallSColorC, wallSColorE,
                                  wallSColorW, wallSColorC, wallSColorE);
  }

  // WEST WALL
  if (wEmpty)
  {
    RealVec2 vE(vTileW.x + ws, vTileW.y);
    RealVec2 vNE(vTileNW.x + ws, vTileNW.y);
    if (nEmpty)
    {
      vNE.y += ws;
    }
    else if (!nwEmpty)
    {
      vNE.y -= ws;
    }
    RealVec2 vSE(vTileSW.x + ws, vTileSW.y);
    if (sEmpty)
    {
      vSE.y -= ws;
    }
    else if (!swEmpty)
    {
      vSE.y += ws;
    }

    m_tileSheet.addGradientQuadTo(vertices, tileSheetCoords,
                                  vTileNW, vNE,
                                  vTileSW, vSE,
                                  wallWColorN, wallWColorN, wallWColorN,
                                  wallWColorC, wallWColorC, wallWColorC,
                                  wallWColorS, wallWColorS, wallWColorS);
  }
}

bool MapTileStandard2DView::onEvent(Event const& event)
{
  return false;
}

