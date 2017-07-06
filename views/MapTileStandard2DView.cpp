#include "stdafx.h"

#include "views/MapTileStandard2DView.h"

#include "components/ComponentManager.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "systems/Manager.h"
#include "systems/SystemLighting.h"
#include "systems/SystemSenseSight.h"
#include "tilesheet/TileSheet.h"
#include "types/Color.h"
#include "types/ShaderEffect.h"
#include "utilities/RNGUtils.h"
#include "views/Standard2DGraphicViews.h"

std::string MapTileStandard2DView::getViewName()
{
  return "standard2D";
}

MapTileStandard2DView::MapTileStandard2DView(MapTile& map_tile, 
                                             Standard2DGraphicViews& views)
  :
  MapTileView(map_tile),
  m_tileOffset{ 0 }, //m_tileOffset{ pick_uniform(0, 4) },
  m_views{ views }
{
}

UintVec2 MapTileStandard2DView::getFloorTileSheetCoords() const
{
  /// @todo Deal with selecting one of the other tiles.
  auto& entity = getMapTile().getFloorEntity();
  auto& categoryData = S<IGameRules>().categoryData(COMPONENTS.category[entity]);
  UintVec2 offset;

  // Get tile coordinates on the sheet.
  UintVec2 start_coords = m_views.getTileSheetCoords(COMPONENTS.category[entity]);

  UintVec2 tile_coords(start_coords.x + m_tileOffset, start_coords.y);
  return tile_coords;
}

UintVec2 MapTileStandard2DView::getSpaceTileSheetCoords() const
{
  /// @todo Deal with selecting one of the other tiles.
  auto& entity = getMapTile().getSpaceEntity();
  auto& categoryData = S<IGameRules>().categoryData(COMPONENTS.category[entity]);
  UintVec2 offset;

  // Get tile coordinates on the sheet.
  UintVec2 start_coords = m_views.getTileSheetCoords(COMPONENTS.category[entity]);

  UintVec2 tile_coords(start_coords.x + m_tileOffset, start_coords.y);
  return tile_coords;
}

/// @todo Instead of repeating this method in EntityStandard2DView, call the view from here.
UintVec2 MapTileStandard2DView::getEntityTileSheetCoords(EntityId entity, int frame) const
{
  /// Get tile coordinates on the sheet.
  UintVec2 start_coords = m_views.getTileSheetCoords(COMPONENTS.category[entity]);

  /// Call the Lua function to get the offset (tile to choose).
  UintVec2 offset = GAME.lua().callEntityFunction("get_tile_offset", entity, frame, UintVec2(0, 0));

  /// Add them to get the resulting coordinates.
  UintVec2 tile_coords = start_coords + offset;

  return tile_coords;
}
void MapTileStandard2DView::addTileVertices(EntityId viewer,
                                            sf::VertexArray& horizVertices,
                                            sf::VertexArray& vertVertices,
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

  if (canSee(coords))
  {
    addHorizontalSurfaceVerticesTo(horizVertices, viewer, lighting);
    addVerticalSurfaceVerticesTo(vertVertices, viewer, &lighting);
  }
  else
  {
    addMemoryVerticesTo(memoryVertices, viewer);
  }
}

void MapTileStandard2DView::addMemoryVerticesTo(sf::VertexArray& vertices,
                                                EntityId viewer)
{
  if (!COMPONENTS.position.existsFor(viewer) || 
      !COMPONENTS.spacialMemory.existsFor(viewer)) return;

  auto& config = S<IConfigSettings>();
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
  RealVec2 ts = config.get("map-tile-size");
  RealVec2 ts2 = { ts.x * 0.5f, ts.y * 0.5f };

  RealVec2 location(coords.x * ts.x, coords.y * ts.y);
  RealVec2 vSW(location.x - ts2.x, location.y + ts2.y);
  RealVec2 vSE(location.x + ts2.x, location.y + ts2.y);
  RealVec2 vNW(location.x - ts2.x, location.y - ts2.y);
  RealVec2 vNE(location.x + ts2.x, location.y - ts2.y);

  if (!spacialMemory.contains(coords)) return;

  auto& memories = spacialMemory[coords].getSpecs();

  for (auto& memory : memories)
  {
    json& tileCategoryData = S<IGameRules>().categoryData(memory.category);

    Color opacity = Color::White;
    auto& components = tileCategoryData["components"]; // guaranteed to be there by FileSystemGameRules::loadCategoryIfNecessary
    
    std::string category = components["category"].get<std::string>();

    // Bail if the object has no associated tiles.
    if (!m_views.hasTilesFor(category)) return;

    if (components.count("appearance") != 0)
    {
      auto& appearance = components["appearance"];
      if (appearance.is_object() && 
          appearance.count("opacity") != 0 && 
          appearance["opacity"].is_object())
      {
        opacity = appearance["opacity"];
      }
    }

    // Bail if the object is totally transparent.
    if (opacity == Color::Black) return;

    /// @todo Call a script to handle selecting a tile other than the one
    ///       in the upper-left corner.
    UintVec2 tileCoords = m_views.getTileSheetCoords(category);

    m_views.getTileSheet().addQuad(vertices,
                                   tileCoords, Color::White,
                                   vNW, vNE,
                                   vSW, vSE);
  }
}

void MapTileStandard2DView::addHorizontalSurfaceVerticesTo(sf::VertexArray& vertices,
                                                           EntityId viewer,
                                                           Systems::Lighting& lighting)
{
  auto& config = S<IConfigSettings>();

  auto& tile = getMapTile();
  auto& coords = tile.getCoords();

  auto canSee = [viewer](IntVec2 coords) -> bool
  {
    return SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords);
  };

  if (!canSee(coords)) return;

  sf::Vertex new_vertex;
  RealVec2 ts = config.get("map-tile-size");
  RealVec2 halfTs = { ts.x * 0.5f, ts.y * 0.5f };

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

  RealVec2 location{ coords.x * ts.x, coords.y * ts.y };
  RealVec2 vNE{ location.x + halfTs.x, location.y - halfTs.y };
  RealVec2 vSE{ location.x + halfTs.x, location.y + halfTs.y };
  RealVec2 vSW{ location.x - halfTs.x, location.y + halfTs.y };
  RealVec2 vNW{ location.x - halfTs.x, location.y - halfTs.y };

  auto opaque = getMapTile().isTotallyOpaque();
  auto transparent = getMapTile().isTotallyTransparent();

  if (!opaque)
  {
    UintVec2 floorTileCoords = getFloorTileSheetCoords();
    m_views.getTileSheet().addGradientQuadTo(vertices, floorTileCoords,
                                             vNW, vNE,
                                             vSW, vSE,
                                             Color::White,
                                             lightNW, lightN, lightNE,
                                             lightW, light, lightE,
                                             lightSW, lightS, lightSE);
  }

  if (!transparent)
  {
    UintVec2 spaceTileCoords = getSpaceTileSheetCoords();
    m_views.getTileSheet().addGradientQuadTo(vertices, spaceTileCoords,
                                             vNW, vNE,
                                             vSW, vSE,
                                             Color::White,
                                             lightNW, lightN, lightNE,
                                             lightW, light, lightE,
                                             lightSW, lightS, lightSE);
  }
}

void MapTileStandard2DView::addEntitiesVertices(EntityId viewer,
                                                sf::VertexArray & vertices,
                                                Systems::Lighting* lighting,
                                                int frame)
{
  auto& tile = getMapTile();
  auto coords = tile.getCoords();

  EntityId contents = tile.getSpaceEntity();
  auto& inv = COMPONENTS.inventory[contents];

  if (SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords))
  {
    if (inv.count() > 0)
    {
      // Only draw the largest entity on that tile.
      EntityId biggestEntity = inv.getLargestEntity();
      if (biggestEntity != EntityId::Void)
      {
        addEntityVertices(biggestEntity, vertices, lighting, frame);
      }
    }
  }

  // Always draw the viewer itself last, if it is present at that tile.
  if (inv.contains(viewer))
  {
    addEntityVertices(viewer, vertices, lighting, frame);
  }
}

void MapTileStandard2DView::addEntityVertices(EntityId entityId, 
                                              sf::VertexArray& vertices,
                                              Systems::Lighting* lighting,
                                              int frame)
{
  auto& config = S<IConfigSettings>();
  auto& category = COMPONENTS.category[entityId];
  sf::Vertex new_vertex;
  RealVec2 ts = config.get("map-tile-size");
  RealVec2 ts2 = { ts.x * 0.5f, ts.y * 0.5f };

  // If this entity doesn't have a Position component, bail.
  if (!COMPONENTS.position.existsFor(entityId)) return;

  // If this entity doesn't have associated tiles, bail.
  if (!m_views.hasTilesFor(category)) return;

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

  RealVec2 location(coords.x * ts.x, coords.y * ts.y);
  RealVec2 vSW(location.x - ts2.x, location.y + ts2.y);
  RealVec2 vSE(location.x + ts2.x, location.y + ts2.y);
  RealVec2 vNW(location.x - ts2.x, location.y - ts2.y);
  RealVec2 vNE(location.x + ts2.x, location.y - ts2.y);
  UintVec2 tileCoords = getEntityTileSheetCoords(entityId, frame);

  m_views.getTileSheet().addQuad(vertices,
                                 tileCoords, thingColor,
                                 vNW, vNE,
                                 vSW, vSE);
}


void MapTileStandard2DView::addVerticalSurfaceVerticesTo(sf::VertexArray& vertices,
                                                         EntityId viewer,
                                                         Systems::Lighting* lighting)
{
  auto& tile = getMapTile();
  auto coords = tile.getCoords();
  auto& x = coords.x;
  auto& y = coords.y;

  auto& config = S<IConfigSettings>();
  RealVec2 mapTileSize = config.get("map-tile-size");

  auto canSee = [viewer](IntVec2 coords) -> bool
  {
    return SYSTEMS.senseSight()->subjectCanSeeCoords(viewer, coords);
  };

  if (!canSee(coords)) return;

  bool this_is_empty = tile.isPassable();
  bool nwEmpty = (canSee({ x - 1, y - 1 }) && tile.getAdjacentTile(Direction::Northwest).isPassable());
  bool nEmpty = (canSee({ x, y - 1 }) && tile.getAdjacentTile(Direction::North).isPassable());
  bool neEmpty = (canSee({ x + 1, y - 1 }) && tile.getAdjacentTile(Direction::Northeast).isPassable());
  bool eEmpty = (canSee({ x + 1, y }) && tile.getAdjacentTile(Direction::East).isPassable());
  bool seEmpty = (canSee({ x + 1, y + 1 }) && tile.getAdjacentTile(Direction::Southeast).isPassable());
  bool sEmpty = (canSee({ x, y + 1 }) && tile.getAdjacentTile(Direction::South).isPassable());
  bool swEmpty = (canSee({ x - 1, y + 1 }) && tile.getAdjacentTile(Direction::Southwest).isPassable());
  bool wEmpty = (canSee({ x - 1, y }) && tile.getAdjacentTile(Direction::West).isPassable());

  // Let's first see if this tile is totally transparent. If it is, no vertical
  // surfaces will be visible.
  if (tile.isTotallyTransparent()) return;

  // Checks to see N/S/E/W walls.
  bool playerSeesNWall{ false };
  bool playerSeesSWall{ false };
  bool playerSeesEWall{ false };
  bool playerSeesWWall{ false };

  // Player.
  EntityId player = COMPONENTS.globals.player();
  bool playerHasLocation = COMPONENTS.position.existsFor(player);

  if (player != EntityId::Void && playerHasLocation)
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
  RealVec2 ts = mapTileSize;

  // Half of the tile size.
  RealVec2 halfTs = { mapTileSize.x * 0.5f, mapTileSize.y * 0.5f };

  // Wall size (configurable).
  /// @todo Make this configurable.
  RealVec2 ws = { mapTileSize.x * 0.4f, mapTileSize.y * 0.4f };

  // Adjacent tile coordinates
  IntVec2 coordsN = coords + static_cast<IntVec2>(Direction::North);
  IntVec2 coordsE = coords + static_cast<IntVec2>(Direction::East);
  IntVec2 coordsS = coords + static_cast<IntVec2>(Direction::South);
  IntVec2 coordsW = coords + static_cast<IntVec2>(Direction::West);

  // Tile vertices.
  RealVec2 location(coords.x * ts.x, coords.y * ts.y);
  RealVec2 vTileN(location.x, location.y - halfTs.y);
  RealVec2 vTileNE(location.x + halfTs.x, location.y - halfTs.y);
  RealVec2 vTileE(location.x + halfTs.x, location.y);
  RealVec2 vTileSE(location.x + halfTs.x, location.y + halfTs.y);
  RealVec2 vTileS(location.x, location.y + halfTs.y);
  RealVec2 vTileSW(location.x - halfTs.x, location.y + halfTs.y);
  RealVec2 vTileW(location.x - halfTs.x, location.y);
  RealVec2 vTileNW(location.x - halfTs.x, location.y - halfTs.y);

  UintVec2 tileSheetCoords = this->getSpaceTileSheetCoords();

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
    RealVec2 vS(vTileN.x, vTileN.y + ws.y);
    RealVec2 vSW(vTileNW.x, vTileNW.y + ws.y);
    if (wEmpty)
    {
      vSW.x += ws.x;
    }
    else if (!nwEmpty)
    {
      vSW.x -= ws.x;
    }
    RealVec2 vSE(vTileNE.x, vTileNE.y + ws.y);
    if (eEmpty)
    {
      vSE.x -= ws.x;
    }
    else if (!neEmpty)
    {
      vSE.x += ws.x;
    }

    m_views.getTileSheet().addGradientQuadTo(vertices, tileSheetCoords,
                                             vTileNW, vTileNE,
                                             vSW, vSE,
                                             Color::White,
                                             wallNColorW, wallNColorC, wallNColorE,
                                             wallNColorW, wallNColorC, wallNColorE,
                                             wallNColorW, wallNColorC, wallNColorE);
  }

  // EAST WALL
  if (eEmpty)
  {
    RealVec2 vW(vTileE.x - ws.x, vTileE.y);
    RealVec2 vNW(vTileNE.x - ws.x, vTileNE.y);
    if (nEmpty)
    {
      vNW.y += ws.y;
    }
    else if (!neEmpty)
    {
      vNW.y -= ws.y;
    }
    RealVec2 vSW(vTileSE.x - ws.x, vTileSE.y);
    if (sEmpty)
    {
      vSW.y -= ws.y;
    }
    else if (!seEmpty)
    {
      vSW.y += ws.y;
    }

    m_views.getTileSheet().addGradientQuadTo(vertices, tileSheetCoords,
                                             vNW, vTileNE,
                                             vSW, vTileSE,
                                             Color::White,
                                             wallEColorN, wallEColorN, wallEColorN,
                                             wallEColorC, wallEColorC, wallEColorC,
                                             wallEColorS, wallEColorS, wallEColorS);
  }

  // SOUTH WALL
  if (sEmpty)
  {
    RealVec2 vN(vTileS.x, vTileS.y - ws.y);
    RealVec2 vNW(vTileSW.x, vTileSW.y - ws.y);
    if (wEmpty)
    {
      vNW.x += ws.x;
    }
    else if (!swEmpty)
    {
      vNW.x -= ws.x;
    }
    RealVec2 vNE(vTileSE.x, vTileSE.y - ws.y);
    if (eEmpty)
    {
      vNE.x -= ws.x;
    }
    else if (!seEmpty)
    {
      vNE.x += ws.x;
    }

    m_views.getTileSheet().addGradientQuadTo(vertices, tileSheetCoords,
                                             vNW, vNE,
                                             vTileSW, vTileSE,
                                             Color::White,
                                             wallSColorW, wallSColorC, wallSColorE,
                                             wallSColorW, wallSColorC, wallSColorE,
                                             wallSColorW, wallSColorC, wallSColorE);
  }

  // WEST WALL
  if (wEmpty)
  {
    RealVec2 vE(vTileW.x + ws.x, vTileW.y);
    RealVec2 vNE(vTileNW.x + ws.x, vTileNW.y);
    if (nEmpty)
    {
      vNE.y += ws.y;
    }
    else if (!nwEmpty)
    {
      vNE.y -= ws.y;
    }
    RealVec2 vSE(vTileSW.x + ws.x, vTileSW.y);
    if (sEmpty)
    {
      vSE.y -= ws.y;
    }
    else if (!swEmpty)
    {
      vSE.y += ws.y;
    }

    m_views.getTileSheet().addGradientQuadTo(vertices, tileSheetCoords,
                                             vTileNW, vNE,
                                             vTileSW, vSE,
                                             Color::White,
                                             wallWColorN, wallWColorN, wallWColorN,
                                             wallWColorC, wallWColorC, wallWColorC,
                                             wallWColorS, wallWColorS, wallWColorS);
  }
}

bool MapTileStandard2DView::onEvent(Event const& event)
{
  return false;
}

