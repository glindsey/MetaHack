#include "stdafx.h"

#include "map/MapFeature.h"

#include "game/App.h"
#include "map/MapCorridor.h"
#include "map/MapDiamond.h"
#include "map/MapDonutRoom.h"
#include "map/MapLRoom.h"
#include "map/MapRoom.h"
#include "maptile/MapTile.h"
#include "properties/PropertyDictionary.h"
#include "utilities/RNGUtils.h"

MapFeature::MapFeature(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  m_gameMap{ m },
  m_settings{ s },
  m_startVec{ vec }
{}

MapFeature::~MapFeature()
{}

sf::IntRect const& MapFeature::getCoords() const
{
  return m_coords;
}

Map& MapFeature::getMap() const
{
  return m_gameMap;
}

PropertyDictionary const& MapFeature::getSettings() const
{
  return m_settings;
}

size_t MapFeature::getNumOfGrowthVectors() const
{
  return m_highPriorityVecs.size() + m_lowPriorityVecs.size();
}

GeoVector const& MapFeature::getRandomGrowthVector() const
{
  if (m_highPriorityVecs.size() > 0)
  {
    int randomVector = the_RNG.pick_uniform(0, static_cast<int>(m_highPriorityVecs.size() - 1));
    return m_highPriorityVecs[randomVector];
  }
  else if (m_lowPriorityVecs.size() > 0)
  {
    int randomVector = the_RNG.pick_uniform(0, static_cast<int>(m_lowPriorityVecs.size() - 1));
    return m_lowPriorityVecs[randomVector];
  }
  else
  {
    throw MapFeatureException("Out of growth vectors");
  }
}

bool MapFeature::eraseGrowthVector(GeoVector vec)
{
  std::deque<GeoVector>::iterator iter;
  iter = std::find(m_highPriorityVecs.begin(), m_highPriorityVecs.end(), vec);

  if (iter != m_highPriorityVecs.end())
  {
    m_highPriorityVecs.erase(iter);
    return true;
  }

  iter = std::find(m_lowPriorityVecs.begin(), m_lowPriorityVecs.end(), vec);

  if (iter != m_lowPriorityVecs.end())
  {
    m_lowPriorityVecs.erase(iter);
    return true;
  }

  return false;
}

std::unique_ptr<MapFeature> MapFeature::construct(Map& gameMap, PropertyDictionary const & settings, GeoVector vec)
{
  std::unique_ptr<MapFeature> feature;
  std::string feature_type = settings.get("type");

  try
  {
    if (feature_type == "room")
    {
      feature.reset(NEW MapRoom(gameMap, settings, vec));
    }
    else if (feature_type == "corridor")
    {
      feature.reset(NEW MapCorridor(gameMap, settings, vec));
    }
    else if (feature_type == "room_l")
    {
      feature.reset(NEW MapLRoom(gameMap, settings, vec));
    }
    else if (feature_type == "room_diamond")
    {
      feature.reset(NEW MapDiamond(gameMap, settings, vec));
    }
    else if (feature_type == "room_torus")
    {
      feature.reset(NEW MapDonutRoom(gameMap, settings, vec));
    }
    else
    {
      CLOG(WARNING, "MapGenerator") << "Unknown feature type \"" <<
        feature_type << "\" requested";
    }
  }
  catch (MapFeatureException&)
  {
    // This is okay; we can assume that sometimes rooms will fail to construct
    // due to there simply being insufficient space left on the map.
  }
  catch (...)
  {
    // Any other exceptions should be thrown.
    throw;
  }

  // Return either our feature, or an empty unique_ptr if no feature was created.
  return feature;
}

void MapFeature::setCoords(sf::IntRect coords)
{
  m_coords = coords;
}

void MapFeature::clearGrowthVectors()
{
  m_highPriorityVecs.clear();
  m_lowPriorityVecs.clear();
}

void MapFeature::addGrowthVector(GeoVector vec, bool highPriority)
{
  (highPriority == true ?
   m_highPriorityVecs :
   m_lowPriorityVecs).push_back(vec);
}

bool MapFeature::doesBoxPassCriterion(IntVec2 upperLeft,
                                      IntVec2 lowerRight,
                                      std::function<bool(MapTile&)> criterion)
{
  for (int xCheck = upperLeft.x; xCheck <= lowerRight.x; ++xCheck)
  {
    for (int yCheck = upperLeft.y; yCheck <= lowerRight.y; ++yCheck)
    {
      auto& tile = getMap().getTile({ xCheck, yCheck });
      if (!criterion(tile))
      {
        return false;
      }
    }
  }
  return true;
}

void MapFeature::setBox(IntVec2 upperLeft, IntVec2 lowerRight, EntitySpecs floor, EntitySpecs space)
{
  Map& map = getMap();

  for (int xCheck = upperLeft.x; xCheck <= lowerRight.x; ++xCheck)
  {
    for (int yCheck = upperLeft.y; yCheck <= lowerRight.y; ++yCheck)
    {
      if (map.isInBounds({ xCheck, yCheck }))
      {
        auto& tile = getMap().getTile({ xCheck, yCheck });
        tile.setTileType(floor, space);
      }
    }
  }
}

void MapFeature::setBox(sf::IntRect rect, EntitySpecs floor, EntitySpecs space)
{
  setBox({ rect.left, rect.top }, { rect.left + rect.width - 1, rect.top + rect.height - 1 }, floor, space);
}
