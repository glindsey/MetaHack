#include <stdafx.h>

#include "systems/SystemLighting.h"

#include "AssertHelper.h"
#include "components/ComponentAppearance.h"
#include "components/ComponentHealth.h"
#include "components/ComponentLightSource.h"
#include "components/ComponentPosition.h"
#include "map/Map.h"
#include "systems/SystemSpacialRelationships.h"
#include "types/LightInfluence.h"

SystemLighting::SystemLighting(GameState& gameState,
                               ComponentMap<ComponentAppearance> const& appearance,
                               ComponentMap<ComponentHealth> const& health,
                               ComponentMap<ComponentLightSource>& lightSource,
                               ComponentMap<ComponentPosition> const& position) :
  SystemCRTP<SystemLighting>({}),
  m_gameState{ gameState },
  m_appearance{ appearance },
  m_health{ health },
  m_lightSource{ lightSource },
  m_position{ position },
  m_tileCalculatedLightColors{ NEW TileCalculatedLightColors({1, 1}) },
  m_tileLightSet{ NEW TileLightData({1, 1}) },
  m_ambientLightColor{ 48, 48, 48 } ///< @todo Make this configurable
{
}

SystemLighting::~SystemLighting()
{}

void SystemLighting::doCycleUpdate()
{
  MapId currentMap = map();
  if (currentMap == MapId::Null()) return;

  // Step 1: Handle light propogation for lights on the map.
  if (m_recalculateAllLights == true)
  {
    resetAllMapLightingData(currentMap);
    for (auto& lightSourcePair : m_lightSource.data())
    {
      EntityId lightSource = lightSourcePair.first;
      auto& lightSourceData = lightSourcePair.second;
      bool onMap = m_position.existsFor(lightSource) && (m_position.of(lightSource).map() == currentMap);
      if (onMap) applyLightFrom(lightSource, m_position.of(lightSource).parent());
    }
  }
  else
  {
    for (auto& lightSource : m_lightsToRecalculate)
    {
      removeLightFromMap(lightSource);
      auto& lightSourceData = m_lightSource.of(lightSource);
      bool onMap = m_position.existsFor(lightSource) && (m_position.of(lightSource).map() == currentMap);
      if (onMap) applyLightFrom(lightSource, m_position.of(lightSource).parent());
    }
  }

  // Step 2. Update light level calculations for affected tiles.
  if (m_recalculateAllTiles == true)
  {
    auto mapSize = currentMap->getSize();
    for (int y = 0; y < mapSize.y; ++y)
    {
      for (int x = 0; x < mapSize.x; ++x)
      {
        calculateTileLightLevels({ x, y });
      }
    }
    m_recalculateAllTiles = false;
  }
  else
  {
    for (auto& tile : m_tilesToRecalculate)
    {
      calculateTileLightLevels(tile);
    }
    m_tilesToRecalculate.clear();
  }
}

void SystemLighting::resetAllMapLightingData(MapId map)
{  
  auto mapSize = map->getSize();
  m_tileCalculatedLightColors.reset(NEW TileCalculatedLightColors(mapSize));
  m_tileLightSet.reset(NEW TileLightData(mapSize));
  m_lightTileSet.clear();
  m_recalculateAllLights = true;
  m_recalculateAllTiles = true;
}

void SystemLighting::clearMapLightingCalculations(MapId map)
{
  auto mapSize = map->getSize();
  m_tileCalculatedLightColors.reset(NEW TileCalculatedLightColors(mapSize));
  m_recalculateAllTiles = true;
}

Color SystemLighting::getLightLevel(IntVec2 coords) const
{
  return getWallLightLevel(coords, Direction::Self);
}

Color SystemLighting::getWallLightLevel(IntVec2 coords, Direction direction) const
{
  auto& calculatedLightColors = m_tileCalculatedLightColors->get(coords);
  if (calculatedLightColors.count(direction.get_map_index()) == 0)
  {
    return m_ambientLightColor;
  }
  else
  {
    return m_ambientLightColor + calculatedLightColors.at(direction.get_map_index());
  }
}

void SystemLighting::setMapNVO(MapId newMap)
{
  resetAllMapLightingData(newMap);
}

void SystemLighting::applyLightFrom(EntityId light, EntityId location)
{
  // Use visitor pattern.
  if (m_lightSource[light].lit())
  {
    if (location != EntityId::Mu())
    {
      bool locationIsOpaque =
        m_appearance.existsFor(location) &&
        m_appearance.of(location).isTotallyOpaque();
      bool locationHasHealth = m_health.existsFor(location);


      bool result = m_gameState.lua().call_thing_function("on_lit_by", location, light, true);
      if (result)
      {
        //notifyObservers(Event::Updated);
      }

      //if (!isOpaque() || is wielding(light) || is wearing(light))
      if (!locationIsOpaque || locationHasHealth)
      {
        auto locationParent = m_position.of(location).parent();
        applyLightFrom(light, locationParent);
      }
    }
    else // (lightSourceLocation == EntityId::Mu())
    {
      // Add influence to tile.
      addLightToMap(light);
    }
  }
}

void SystemLighting::calculateTileLightLevels(IntVec2 coords)
{
  auto& lights = m_tileLightSet->get(coords);
  auto& lightLevels = m_tileCalculatedLightColors->get(coords);

  lightLevels.clear();
  for (auto& light : lights)
  {
    addLightToTileLightLevels(coords, light);
  }

  m_tilesToRecalculate.erase(coords);
}

void SystemLighting::addLightToTileLightLevels(IntVec2 tileCoords, EntityId source)
{
  auto& lights = m_tileLightSet->get(tileCoords);
  auto& lightLevels = m_tileCalculatedLightColors->get(tileCoords);

  // Add this light if it isn't already in the tile's light set.
  if (lights.count(source) == 0)
  {
    lights.insert(source);
  }

  // Bail if light doesn't have a position component.
  if (!m_position.existsFor(source))
  {
    return;
  }

  auto& lightData = m_lightSource[source];
  auto& lightPosition = m_position.of(source);
  auto& lightCoords = lightPosition.coords();
  if (lightData.lit())
  {
    float dist_squared = static_cast<float>(calc_vis_distance(tileCoords, lightCoords));

    Color addColor{ 0, 0, 0, 255 };

    float dist_factor;

    if (lightData.strength() == 0)
    {
      dist_factor = 1.0f;
    }
    else
    {
      dist_factor = dist_squared / static_cast<float>(lightData.strength());
    }

    std::vector<Direction> const directions
    {
      Direction::Self,
      Direction::North,
      Direction::East,
      Direction::South,
      Direction::West
    };

    for (Direction d : directions)
    {
      //if (!isOpaque() || (d != Direction::Self))
      {
        float light_factor = (1.0f - dist_factor);
        float wall_factor = Direction::calculate_light_factor(lightCoords, tileCoords, d);
        float factor = wall_factor * light_factor;

        float newR = static_cast<float>(lightData.color().r()) * factor;
        float newG = static_cast<float>(lightData.color().g()) * factor;
        float newB = static_cast<float>(lightData.color().b()) * factor;

        addColor.setR(newR);
        addColor.setG(newG);
        addColor.setB(newB);

        unsigned int index = d.get_map_index();
        auto prevColor = lightLevels[index];

        lightLevels[index] = prevColor + addColor;
      }
    } // end for (Direction d : directions)
  } // end if (lightData.lit)
}

void SystemLighting::addLightToMap(EntityId source)
{
  // First check if this light is already on the map. If so, remove it so it
  // isn't counted twice.
  if (m_lightTileSet.count(source) > 0 &&
      !m_lightTileSet[source].empty())
  {
    removeLightFromMap(source);
  }

  // Get the location of the light source.
  auto& position = m_position.of(source);
  IntVec2 coords = position.coords();

  Color light_color = m_lightSource[source].color();
  int max_depth_squared = m_lightSource[source].strength();

  /// @todo Re-implement direction.
  Direction light_direction = Direction::Up;

  /// @todo Handle the special case of Direction::Self.
  /// If a light source's direction is set to "Self", it should be treated as
  /// omnidirectional but dimmer when not held by an DynamicEntity, and the same
  /// direction as the DynamicEntity when it is held.

  /// @todo: Handle "dark sources" with negative light strength properly --
  ///        right now they'll cause Very Bad Behavior!

  // Add an influence to the tile the light is on.
  addLightToTile(coords, source);

    // Octant is an integer representing the following:
  // \ 1|2 /  |
  //  \ | /   |
  // 8 \|/ 3  |
  // ---+---  |
  // 7 /|\ 4  |
  //  / | \   |
  // / 6|5 \  |

  // Directional lighting:
  // Direction  1 2 3 4 5 6 7 8
  // ==========================
  // Self/Up/Dn x x x x x x x x
  // North      x x - - - - - -
  // Northeast  - x x - - - - -
  // East       - - x x - - - -
  // Southeast  - - - x x - - -
  // South      - - - - x x - -
  // Southwest  - - - - - x x -
  // West       - - - - - - x x
  // Northwest  x - - - - - - x

  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Northwest) ||
      (light_direction == Direction::North))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 1);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::North) ||
      (light_direction == Direction::Northeast))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 2);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Northeast) ||
      (light_direction == Direction::East))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 3);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::East) ||
      (light_direction == Direction::Southeast))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 4);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southeast) ||
      (light_direction == Direction::South))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 5);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::South) ||
      (light_direction == Direction::Southwest))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 6);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southwest) ||
      (light_direction == Direction::West))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 7);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::West) ||
      (light_direction == Direction::Northwest))
  {
    doRecursiveLighting(source, coords, max_depth_squared, 8);
  }
}

void SystemLighting::removeLightFromMap(EntityId source)
{
  if (m_lightTileSet.count(source) == 0 || m_lightTileSet[source].empty()) return;

  for (auto& coords : m_lightTileSet[source])
  {
    removeLightFromTile(coords, source);
  }

  m_lightTileSet.erase(source);
}

void SystemLighting::addLightToTile(IntVec2 coords, EntityId source)
{
  m_tileLightSet->get(coords).insert(source);
  m_lightTileSet[source].insert(coords);
  m_tilesToRecalculate.insert(coords);
}

void SystemLighting::removeLightFromTile(IntVec2 coords, EntityId source)
{
  m_tileLightSet->get(coords).erase(source);
  m_lightTileSet[source].erase(coords);
  m_tilesToRecalculate.insert(coords);
}

void SystemLighting::doRecursiveLighting(EntityId source,
                                         IntVec2 const& origin,
                                         int const max_depth_squared,
                                         int octant,
                                         int depth,
                                         float slope_A,
                                         float slope_B)
{
  MapId currentMap = map();
  Assert("Lighting", octant >= 1 && octant <= 8, "Octant" << octant << "passed in is not between 1 and 8 inclusively");
  IntVec2 new_coords;

  Color addColor;

  std::function< bool(RealVec2, RealVec2, float) > loop_condition;
  Direction dir;
  std::function< float(RealVec2, RealVec2) > recurse_slope;
  std::function< float(RealVec2, RealVec2) > loop_slope;

  switch (octant)
  {
  case 1:
    new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
    new_coords.y = origin.y - depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
    dir = Direction::West;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
    break;

  case 2:
    new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
    new_coords.y = origin.y - depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
    dir = Direction::East;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Northeast.half(), b); };
    break;

  case 3:
    new_coords.x = origin.x + depth;
    new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
    dir = Direction::North;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
    break;

  case 4:
    new_coords.x = origin.x + depth;
    new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
    dir = Direction::South;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
    break;

  case 5:
    new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
    new_coords.y = origin.y + depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
    dir = Direction::East;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
    break;

  case 6:
    new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
    new_coords.y = origin.y + depth;
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
    dir = Direction::West;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Southwest.half(), b); };
    break;

  case 7:
    new_coords.x = origin.x - depth;
    new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
    dir = Direction::South;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
    break;

  case 8:
    new_coords.x = origin.x - depth;
    new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));

    loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
    dir = Direction::North;
    recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
    loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
    break;

  default:
    break;
  }

  while (loop_condition(to_v2f(new_coords), to_v2f(origin), slope_B))
  {
    if (calc_vis_distance(new_coords, origin) <= max_depth_squared)
    {
      if (currentMap->getTile(new_coords).isTotallyOpaque())
      {
        if (!currentMap->getTile(new_coords + (IntVec2)dir).isTotallyOpaque())
        {
          doRecursiveLighting(source, origin, 
                              max_depth_squared,
                              octant, depth + 1,
                              slope_A, recurse_slope(to_v2f(new_coords), to_v2f(origin)));
        }
      }
      else
      {
        if (currentMap->getTile(new_coords + (IntVec2)dir).isTotallyOpaque())
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(origin));
        }
      }

      addLightToTileLightLevels(new_coords, source);
    }
    new_coords -= (IntVec2)dir;
  }
  new_coords += (IntVec2)dir;

  if ((depth*depth < max_depth_squared) && (!currentMap->getTile(new_coords).isTotallyOpaque()))
  {
    doRecursiveLighting(source, origin, 
                        max_depth_squared,
                        octant, depth + 1,
                        slope_A, slope_B);
  }
}

bool SystemLighting::onEvent(Event const& event)
{
  auto id = event.getId();
  if (id == SystemSpacialRelationships::EventEntityMoved::id)
  {
    auto& castEvent = static_cast<SystemSpacialRelationships::EventEntityMoved const&>(event);

    // If entity is a light source, add it to the recalculate list.
    if (m_lightSource.existsFor(castEvent.entity))
    {
      m_lightsToRecalculate.insert(castEvent.entity);
    }

    // Get the old coordinates of this entity.
    IntVec2 oldCoords = castEvent.oldPosition.coords();

    // Step through all lights shining on those coordinates, and flag the
    // sources responsible for recalculation.
    for (auto& influence : m_tileLightSet->get(oldCoords))
    {
      m_lightsToRecalculate.insert(influence);
    }

    // Get the new coordinates of this entity (if any).
    // It shouldn't be possible for an entity to have no position data at
    // all, but defensively guard against this just in case.
    if (!m_position.existsFor(castEvent.entity))
    {
      CLOG(ERROR, "Lighting") << "Entity " << castEvent.entity << " has no position component when trying to recalculate lighting!?";
      return false;
    }

    IntVec2 newCoords = m_position.of(castEvent.entity).coords();

    // Same as above.
    for (auto& influence : m_tileLightSet->get(newCoords))
    {
      m_lightsToRecalculate.insert(influence);
    }
  }
  return false;
}
