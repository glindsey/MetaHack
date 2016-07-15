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
  Impl(Map& m, PropertyDictionary const& s)
    :
    gameMap{ m },
    settings{ s }
  {}

  Map& gameMap;
  PropertyDictionary settings;
  sf::IntRect coords;
  std::deque<GeoVector> vecs;
};

MapFeature::MapFeature(Map& m, PropertyDictionary const& s)
  :
  pImpl(NEW Impl(m, s))
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

  if (feature_type == "room")
  {
    feature.reset(NEW MapRoom(game_map, settings));
  }
  else if (feature_type == "corridor")
  {
    feature.reset(NEW MapCorridor(game_map, settings));
  }
  else if (feature_type == "room_l")
  {
    feature.reset(NEW MapLRoom(game_map, settings));
  }
  else if (feature_type == "room_diamond")
  {
    feature.reset(NEW MapDiamond(game_map, settings));
  }
  else if (feature_type == "room_torus")
  {
    feature.reset(NEW MapDonutRoom(game_map, settings));
  }
  else
  {
    CLOG(WARNING, "MapGenerator") << "Unknown feature type \"" <<
      feature_type << "\" requested";
  }

  if (feature)
  {
    if (feature->create(vec))
    {
      return feature;
    }
  }

  return std::unique_ptr<MapFeature>();
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