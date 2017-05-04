#include <stdafx.h>

#include "systems/SystemLighting.h"

#include "components/ComponentLightSource.h"
#include "components/ComponentPosition.h"
#include "map/Map.h"

SystemLighting::SystemLighting(ComponentMap<ComponentLightSource>& lightSources, 
                               ComponentMap<ComponentPosition>& positions) :
  m_lightSources{ lightSources },
  m_positions{ positions }
{}

MapId SystemLighting::map() const
{
  return m_map;
}

void SystemLighting::setMap(MapId map)
{
  m_map = map;
}

void SystemLighting::recalculate()
{
  if (m_map == MapId::Null()) return;

  // Clear it first.
  auto mapSize = m_map->getSize();

  for (int y = 0; y < mapSize.y; ++y)
  {
    for (int x = 0; x < mapSize.x; ++x)
    {
      auto& tile = m_map->getTile({ x, y });
      tile.clearLightInfluences();
    }
  }

  for (auto& lightSourcePair : m_lightSources.data())
  {
    EntityId lightSource = lightSourcePair.first;
    auto& lightSourceData = lightSourcePair.second;
    bool onMap = m_positions.existsFor(lightSource) && (m_positions[lightSource].map() == m_map);
    if (onMap) lightSource->light_up_surroundings();
  }

  //notifyObservers(Event::Updated);
}
