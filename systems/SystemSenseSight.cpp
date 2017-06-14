#include "stdafx.h"

#include "systems/SystemSenseSight.h"

#include "components/ComponentInventory.h"
#include "components/ComponentPosition.h"
#include "components/ComponentSenseSight.h"
#include "components/ComponentSpacialMemory.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "systems/Manager.h"
#include "systems/SystemGeometry.h"
#include "systems/SystemTimekeeper.h"
#include "types/Direction.h"
#include "utilities/MathUtils.h"

namespace Systems
{

  SenseSight::SenseSight(GameState const& gameState,
                         Components::ComponentMap<Components::ComponentInventory> const& inventory,
                         Components::ComponentMap<Components::ComponentPosition> const& position,
                         Components::ComponentMap<Components::ComponentSenseSight>& senseSight,
                         Components::ComponentMap<Components::ComponentSpacialMemory>& spacialMemory) :
    CRTP<SenseSight>({}),
    m_gameState{ gameState },
    m_inventory{ inventory },
    m_position{ position },
    m_senseSight{ senseSight },
    m_spacialMemory{ spacialMemory }
  {}

  SenseSight::~SenseSight()
  {}

  void SenseSight::doCycleUpdate()
  {
    for (auto& pair : m_senseSight.data())
    {
      EntityId entity = pair.first;
      findSeenTiles(entity);
    }
    m_needsUpdate.clear();
  }

  void SenseSight::findSeenTiles(EntityId id)
  {
    // If we don't have a position component, bail.
    if (!m_position.existsFor(id)) return;

    // Are we on a map (i.e. not inside another entity)?  Bail out if we aren't.
    /// @todo Might want to deal with mapping the "inside of an entity" at some point.
    EntityId location = m_position.of(id).parent();
    if (location == EntityId::Mu())
    {
      return;
    }

    // Clear the "tiles seen" bitset.
    m_senseSight[id].resetSeen();

    /// @todo Handle field-of-view here.
    ///       Field of view for an DynamicEntity can be:
    ///          * NARROW (90 degrees straight ahead)
    ///		   * WIDE (180 degrees in facing direction)
    ///          * FRONTBACK (90 degrees ahead/90 degrees back)
    ///          * FULL (all 360 degrees)
    Components::ComponentPosition const& position = m_position.of(id);

    for (int n = 1; n <= 8; ++n)
    {
      calculateRecursiveVisibility(id, position, n);
    }
  }

  void SenseSight::calculateRecursiveVisibility(EntityId id,
                                                Components::ComponentPosition const& thisPosition,
                                                int octant,
                                                int depth,
                                                float slope_A,
                                                float slope_B)
  {
    Assert("SenseSight", octant >= 1 && octant <= 8, "Octant" << octant << "passed in is not between 1 and 8 inclusively");
    IntVec2 newCoords;
    //int x = 0;
    //int y = 0;

    // Are we on a map?  Bail out if we aren't.
    if (!m_position.existsFor(id) || m_position.of(id).isInsideAnotherEntity())
    {
      return;
    }

    IntVec2 thisCoords = thisPosition.coords();
    MapID thisMap = thisPosition.map();

    static const int mv = 128;
    static constexpr int mw = (mv * mv);

    std::function< bool(RealVec2, RealVec2, float) > loop_condition;
    Direction dir;
    std::function< float(RealVec2, RealVec2) > recurse_slope;
    std::function< float(RealVec2, RealVec2) > loop_slope;

    switch (octant)
    {
    case 1:
      newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) - (slope_A * static_cast<float>(depth))));
      newCoords.y = thisCoords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) + (slope_A * static_cast<float>(depth))));
      newCoords.y = thisCoords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -Math::slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      newCoords.x = thisCoords.x + depth;
      newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::invSlope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -Math::invSlope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      newCoords.x = thisCoords.x + depth;
      newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::invSlope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) + (slope_A * static_cast<float>(depth))));
      newCoords.y = thisCoords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      newCoords.x = static_cast<int>(rint(static_cast<float>(thisCoords.x) - (slope_A * static_cast<float>(depth))));
      newCoords.y = thisCoords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -Math::slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      newCoords.x = thisCoords.x - depth;
      newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::invSlope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -Math::invSlope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      newCoords.x = thisCoords.x - depth;
      newCoords.y = static_cast<int>(rint(static_cast<float>(thisCoords.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return Math::invSlope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return Math::invSlope(a + Direction::Northwest.half(), b); };
      break;

    default:
      break;
    }

    auto& map = m_gameState.maps().get(thisMap);

    while (loop_condition(Math::toRealVec2(newCoords), Math::toRealVec2(thisCoords), slope_B))
    {
      if (Math::distSquared(newCoords, thisCoords) <= mw)
      {
        if (map.tileIsOpaque(newCoords))
        {
          if (!map.tileIsOpaque(newCoords + (IntVec2)dir))
          {
            calculateRecursiveVisibility(id, thisPosition,
                                         octant, depth + 1,
                                         slope_A, recurse_slope(Math::toRealVec2(newCoords), Math::toRealVec2(thisCoords)));
          }
        }
        else
        {
          if (map.tileIsOpaque(newCoords + (IntVec2)dir))
          {
            slope_A = loop_slope(Math::toRealVec2(newCoords), Math::toRealVec2(thisCoords));
          }
        }

        m_senseSight[id].setSeen(newCoords);

        if (m_spacialMemory.existsFor(id))
        {
          MapMemoryChunk new_memory{ map.getTile(newCoords).getTileType(), SYSTEMS.timekeeper().clock() };
          m_spacialMemory[id].ofMap(thisMap)[newCoords] = new_memory;
        }
      }
      newCoords -= (IntVec2)dir;
    }
    newCoords += (IntVec2)dir;

    if ((depth < mv) && (!map.getTile(newCoords).isTotallyOpaque()))
    {
      calculateRecursiveVisibility(id, thisPosition,
                                   octant, depth + 1,
                                   slope_A, slope_B);
    }
  }

  bool SenseSight::subjectCanSeeCoords(EntityId subject, IntVec2 coords) const
  {
    // Make sure we are able to see at all.
    if (!m_senseSight.existsFor(subject)) return false;

    auto& subjectPosition = m_position.of(subject);
    MapID subjectMap = subjectPosition.map();
    IntVec2 subjectCoords = subjectPosition.coords();

    // If the coordinates match, then yes, we can indeed see the target.
    if (subjectCoords == coords) return true;

    auto subjectMapSize = m_gameState.maps().get(subjectMap).getSize();

    // Check for target coords out of bounds. If they're out of bounds, we can't see it.
    if ((coords.x < 0) || (coords.y < 0) || (coords.x >= subjectMapSize.x) || (coords.y >= subjectMapSize.y))
    {
      return false;
    }

    // Return seen data.
    return m_senseSight[subject].canSee(coords);
  }

  void SenseSight::setMap_V(MapID newMap)
  {
  }

  bool SenseSight::onEvent(Event const& event)
  {
    auto id = event.getId();

    if (id == Geometry::EventEntityMoved::id)
    {
      auto& castEvent = static_cast<Geometry::EventEntityMoved const&>(event);
      m_needsUpdate.insert(castEvent.entity);
    }
    else if (id == Geometry::EventEntityChangedMaps::id)
    {
      auto& castEvent = static_cast<Geometry::EventEntityChangedMaps const&>(event);
      MapID newMap = m_position.of(castEvent.entity).map();
      setMap(newMap);
      IntVec2 newMapSize = m_gameState.maps().get(newMap).getSize();
      m_senseSight[castEvent.entity].resizeSeen(newMapSize);

      if (m_spacialMemory.existsFor(castEvent.entity))
      {
        auto& spacialMemory = m_spacialMemory[castEvent.entity];
        if (!spacialMemory.containsMap(newMap))
        {
          m_spacialMemory[castEvent.entity].ofMap(newMap).resize(newMapSize);
        }
      }

      m_needsUpdate.insert(castEvent.entity);
    }

    return false;
  }

} // end namespace Systems
