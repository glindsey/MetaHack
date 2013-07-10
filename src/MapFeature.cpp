#include "MapFeature.h"

#include <algorithm>
#include <boost/random/uniform_int_distribution.hpp>
#include <deque>
#include <SFML/Graphics.hpp>

#include "App.h"
#include "ErrorHandler.h"

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
  : impl (new Impl(m))
{
  //ctor
}

MapFeature::~MapFeature()
{
  //dtor
}

sf::IntRect const& MapFeature::get_coords() const
{
  return impl->coords;
}

Map& MapFeature::get_map() const
{
  return impl->gameMap;
}

unsigned int MapFeature::get_num_growth_vectors() const
{
  return impl->vecs.size();
}

GeoVector const& MapFeature::get_random_growth_vector() const
{
  uniform_int_dist vecDist(0, impl->vecs.size() - 1);
  int randomVector = vecDist(the_RNG);
  return impl->vecs[randomVector];
}

bool MapFeature::erase_growth_vector(GeoVector vec)
{
  std::deque<GeoVector>::iterator iter;
  iter = std::find(impl->vecs.begin(), impl->vecs.end(), vec);

  if (iter != impl->vecs.end())
  {
    impl->vecs.erase(iter);
    return true;
  }

  return false;
}

void MapFeature::set_coords(sf::IntRect coords)
{
  impl->coords = coords;
}

void MapFeature::clear_growth_vectors()
{
  impl->vecs.clear();
}

void MapFeature::add_growth_vector(GeoVector vec)
{
  impl->vecs.push_back(vec);
}
