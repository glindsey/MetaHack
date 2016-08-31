#include "stdafx.h"

#include "MapFeature.h"

#include "App.h"
#include "MapCorridor.h"
#include "MapDiamond.h"
#include "MapDonutRoom.h"
#include "MapLRoom.h"
#include "MapRoom.h"
#include "PropertyDictionary.h"

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapFeature::Impl
{
  Impl(Map& m, PropertyDictionary const& s, GeoVector vec)
    :
    gameMap{ m },
    settings{ s },
    start_vec{ vec }
  {}

  Map& gameMap;
  PropertyDictionary settings;
  sf::IntRect coords;
  GeoVector start_vec;
  std::deque<GeoVector> vecs;
};

MapFeature::MapFeature(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  pImpl(NEW Impl(m, s, vec))
{}

MapFeature::~MapFeature()
{}

sf::IntRect const& MapFeature::get_coords() const
{
  return pImpl->coords;
}

Map& MapFeature::get_map() const
{
  return pImpl->gameMap;
}

PropertyDictionary const& MapFeature::get_settings() const
{
  return pImpl->settings;
}

unsigned int MapFeature::get_num_growth_vectors() const
{
  return pImpl->vecs.size();
}

GeoVector const& MapFeature::get_random_growth_vector() const
{
  uniform_int_dist vecDist(0, pImpl->vecs.size() - 1);
  int randomVector = vecDist(the_RNG);
  return pImpl->vecs[randomVector];
}

bool MapFeature::erase_growth_vector(GeoVector vec)
{
  std::deque<GeoVector>::iterator iter;
  iter = std::find(pImpl->vecs.begin(), pImpl->vecs.end(), vec);

  if (iter != pImpl->vecs.end())
  {
    pImpl->vecs.erase(iter);
    return true;
  }

  return false;
}

std::unique_ptr<MapFeature> MapFeature::construct(Map& game_map, PropertyDictionary const & settings, GeoVector vec)
{
  std::unique_ptr<MapFeature> feature;
  std::string feature_type = settings.get<std::string>("type");

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

void MapFeature::set_coords(sf::IntRect coords)
{
  pImpl->coords = coords;
}

void MapFeature::clear_growth_vectors()
{
  pImpl->vecs.clear();
}

void MapFeature::add_growth_vector(GeoVector vec)
{
  pImpl->vecs.push_back(vec);
}

bool MapFeature::does_box_pass_criterion(Vec2i upper_left,
                                         Vec2i lower_right,
                                         std::function<bool(MapTile&)> criterion)
{
  for (int xCheck = upper_left.x; xCheck <= lower_right.x; ++xCheck)
  {
    for (int yCheck = upper_left.y; yCheck <= lower_right.y; ++yCheck)
    {
      auto& tile = get_map().get_tile(xCheck, yCheck);
      if (!criterion(tile))
      {
        return false;
      }
    }
  }
  return true;
}

void MapFeature::set_box(Vec2i upper_left, Vec2i lower_right, std::string tile_type)
{
  Map& map = get_map();

  for (int xCheck = upper_left.x; xCheck <= lower_right.x; ++xCheck)
  {
    for (int yCheck = upper_left.y; yCheck <= lower_right.y; ++yCheck)
    {
      if (map.is_in_bounds(xCheck, yCheck))
      {
        auto& tile = get_map().get_tile(xCheck, yCheck);
        tile.set_tile_type(tile_type);
      }
    }
  }
}

void MapFeature::set_box(sf::IntRect rect, std::string tile_type)
{
  set_box({ rect.left, rect.top }, { rect.left + rect.width - 1, rect.top + rect.height - 1 }, tile_type);
}