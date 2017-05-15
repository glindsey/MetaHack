#include <stdafx.h>

#include "systems/SystemLighting.h"

#include "components/ComponentAppearance.h"
#include "components/ComponentHealth.h"
#include "components/ComponentLightSource.h"
#include "components/ComponentPosition.h"
#include "map/Map.h"
#include "types/LightInfluence.h"

SystemLighting::SystemLighting(ComponentMap<ComponentAppearance> const& appearance,
                               ComponentMap<ComponentHealth> const& health,
                               ComponentMap<ComponentLightSource>& lightSource,
                               ComponentMap<ComponentPosition> const& position) :
  SystemCRTP<SystemLighting>(),
  m_appearance{ appearance },
  m_health{ health },
  m_lightSource{ lightSource },
  m_position{ position },
  m_lightingData{ NEW Grid2D<TileLightingData>({1, 1}) },
  m_ambientLightColor{ 48, 48, 48 } ///< @todo Make this configurable
{
}

SystemLighting::~SystemLighting()
{}

void SystemLighting::doCycleUpdate()
{
  MapId currentMap = map();
  if (currentMap == MapId::Null()) return;

  clearAllLightingData();

  for (auto& lightSourcePair : m_lightSource.data())
  {
    EntityId lightSource = lightSourcePair.first;
    auto& lightSourceData = lightSourcePair.second;
    bool onMap = m_position.existsFor(lightSource) && (m_position.of(lightSource).map() == currentMap);
    if (onMap) applyLightFrom(lightSource, m_position.of(lightSource).parent());
  }

  //notifyObservers(Event::Updated);
}

void SystemLighting::clearAllLightingData()
{  
  MapId currentMap = map();
  if (currentMap == MapId::Null()) return;

  // Clear it first.
  auto mapSize = currentMap->getSize();

  for (int y = 0; y < mapSize.y; ++y)
  {
    for (int x = 0; x < mapSize.x; ++x)
    {
      clearLightingData({ x, y });
    }
  }
}

Color SystemLighting::getLightLevel(IntVec2 coords) const
{
  return getWallLightLevel(coords, Direction::Self);
}

Color SystemLighting::getWallLightLevel(IntVec2 coords, Direction direction) const
{
  auto& calculatedLightColors = m_lightingData->get(coords).calculatedLightColors;
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
  auto mapSize = newMap->getSize();
  // Reset light influence grid.
  m_lightingData.reset(NEW Grid2D<TileLightingData>(mapSize));
}

void SystemLighting::clearLightingData(IntVec2 coords)
{
  auto& lightingData = m_lightingData->get(coords);
  lightingData.lights.clear();
  lightingData.calculatedLightColors.clear();
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


      bool result = the_lua_instance.call_thing_function("on_lit_by", location, light, true);
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

void SystemLighting::addLightInfluenceToTile(IntVec2 coords, EntityId source, LightInfluence influence)
{
  auto& lightingData = m_lightingData->get(coords);

  if (lightingData.lights.count(source) == 0)
  {
    lightingData.lights[source] = influence;

    float dist_squared = static_cast<float>(calc_vis_distance(coords.x, coords.y, influence.coords.x, influence.coords.y));

    Color const& light_color = influence.color;
    int light_intensity = influence.intensity;

    Color addColor{ 0, 0, 0, 255 };

    float dist_factor;

    if (light_intensity == 0)
    {
      dist_factor = 1.0f;
    }
    else
    {
      dist_factor = dist_squared / static_cast<float>(light_intensity);
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
        float wall_factor = Direction::calculate_light_factor(influence.coords, coords, d);
        float factor = wall_factor * light_factor;

        float newR = static_cast<float>(light_color.r()) * factor;
        float newG = static_cast<float>(light_color.g()) * factor;
        float newB = static_cast<float>(light_color.b()) * factor;

        addColor.setR(newR);
        addColor.setG(newG);
        addColor.setB(newB);

        unsigned int index = d.get_map_index();
        auto prevColor = lightingData.calculatedLightColors[index];

        lightingData.calculatedLightColors[index] = prevColor + addColor;
      }
    }
  }
}

void SystemLighting::addLightToMap(EntityId source)
{
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

  // Add a light influence to the tile the light is on.
  LightInfluence influence;
  influence.coords = coords;
  influence.color = light_color;
  influence.intensity = max_depth_squared;
  addLightInfluenceToTile(coords, source, influence);

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
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 1);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::North) ||
      (light_direction == Direction::Northeast))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 2);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Northeast) ||
      (light_direction == Direction::East))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 3);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::East) ||
      (light_direction == Direction::Southeast))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 4);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southeast) ||
      (light_direction == Direction::South))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 5);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::South) ||
      (light_direction == Direction::Southwest))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 6);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southwest) ||
      (light_direction == Direction::West))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 7);
  }
  if ((light_direction == Direction::Self) ||
    (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::West) ||
      (light_direction == Direction::Northwest))
  {
    doRecursiveLighting(source, coords, light_color, max_depth_squared, 8);
  }

  //notifyObservers(Event::Updated);
}

void SystemLighting::doRecursiveLighting(EntityId source,
                                         IntVec2 const& origin,
                                         Color const& light_color,
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
          doRecursiveLighting(source, origin, light_color,
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

      LightInfluence influence;
      influence.coords = origin;
      influence.color = light_color;
      influence.intensity = max_depth_squared;
      addLightInfluenceToTile(new_coords, source, influence);
    }
    new_coords -= (IntVec2)dir;
  }
  new_coords += (IntVec2)dir;

  if ((depth*depth < max_depth_squared) && (!currentMap->getTile(new_coords).isTotallyOpaque()))
  {
    doRecursiveLighting(source, origin, light_color,
                        max_depth_squared,
                        octant, depth + 1,
                        slope_A, slope_B);
  }
}

std::unordered_set<EventID> SystemLighting::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  /// @todo Add registered events here
  return events;
}

bool SystemLighting::onEvent_V(Event const& event)
{
  return false;
}
