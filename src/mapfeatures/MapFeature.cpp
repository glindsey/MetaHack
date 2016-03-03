#include "stdafx.h"

#include "MapFeature.h"

#include "App.h"
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