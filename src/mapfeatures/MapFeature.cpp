#include "MapFeature.h"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <deque>
#include <SFML/Graphics.hpp>

#include "App.h"

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapFeature::Impl
{
  Impl(Map& m) : gameMap(m) {}

  Map& gameMap;
  sf::IntRect coords;
  std::deque<GeoVector> vecs;
};

MapFeature::MapFeature(Map& m)
  : pImpl (NEW Impl(m))
{
  //ctor
}

MapFeature::~MapFeature()
{
  //dtor
}

sf::IntRect const& MapFeature::get_coords() const
{
  return pImpl->coords;
}

Map& MapFeature::get_map() const
{
  return pImpl->gameMap;
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
