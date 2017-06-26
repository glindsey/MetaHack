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

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapFeature::MapFeature(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  m_gameMap{ m },
  m_settings{ s },
  m_start_vec{ vec }
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

PropertyDictionary const& MapFeature::get_settings() const
{
  return m_settings;
}

size_t MapFeature::get_num_growth_vectors() const
{
  return m_highPriorityVecs.size() + m_lowPriorityVecs.size();
}

GeoVector const& MapFeature::get_random_growth_vector() const
{
  if (m_highPriorityVecs.size() > 0)
  {
    uniform_int_dist vecDist(0, static_cast<int>(m_highPriorityVecs.size() - 1));
    int randomVector = vecDist(the_RNG);
    return m_highPriorityVecs[randomVector];
  }
  else if (m_lowPriorityVecs.size() > 0)
  {
    uniform_int_dist vecDist(0, static_cast<int>(m_lowPriorityVecs.size() - 1));
    int randomVector = vecDist(the_RNG);
    return m_lowPriorityVecs[randomVector];
  }
  else
  {
    throw MapFeatureException("Out of growth vectors");
  }
}

bool MapFeature::erase_growth_vector(GeoVector vec)
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

std::unique_ptr<MapFeature> MapFeature::construct(Map& game_map, PropertyDictionary const & settings, GeoVector vec)
{
  std::unique_ptr<MapFeature> feature;
  std::string feature_type = settings.get("type");

  try
  {
    if (feature_type == "room")
    {
      feature.reset(NEW MapRoom(game_map, settings, vec));
    }
    else if (feature_type == "corridor")
    {
      feature.reset(NEW MapCorridor(game_map, settings, vec));
    }
    else if (feature_type == "room_l")
    {
      feature.reset(NEW MapLRoom(game_map, settings, vec));
    }
    else if (feature_type == "room_diamond")
    {
      feature.reset(NEW MapDiamond(game_map, settings, vec));
    }
    else if (feature_type == "room_torus")
    {
      feature.reset(NEW MapDonutRoom(game_map, settings, vec));
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

void MapFeature::clear_growth_vectors()
{
  m_highPriorityVecs.clear();
  m_lowPriorityVecs.clear();
}

void MapFeature::add_growth_vector(GeoVector vec, bool highPriority)
{
  (highPriority == true ? 
   m_highPriorityVecs :
   m_lowPriorityVecs).push_back(vec);
}

bool MapFeature::does_box_pass_criterion(IntVec2 upper_left,
                                         IntVec2 lower_right,
                                         std::function<bool(MapTile&)> criterion)
{
  for (int xCheck = upper_left.x; xCheck <= lower_right.x; ++xCheck)
  {
    for (int yCheck = upper_left.y; yCheck <= lower_right.y; ++yCheck)
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

void MapFeature::set_box(IntVec2 upper_left, IntVec2 lower_right, std::string tile_type)
{
  Map& map = getMap();

  for (int xCheck = upper_left.x; xCheck <= lower_right.x; ++xCheck)
  {
    for (int yCheck = upper_left.y; yCheck <= lower_right.y; ++yCheck)
    {
      if (map.isInBounds({ xCheck, yCheck }))
      {
        auto& tile = getMap().getTile({ xCheck, yCheck });
        tile.setTileType(tile_type);
      }
    }
  }
}

void MapFeature::set_box(sf::IntRect rect, std::string tile_type)
{
  set_box({ rect.left, rect.top }, { rect.left + rect.width - 1, rect.top + rect.height - 1 }, tile_type);
}