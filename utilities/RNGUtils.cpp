#include "utilities/RNGUtils.h"

#include <chrono>
#include <memory>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "utilities/New.h"

using UniformIntDist = boost::random::uniform_int_distribution<>;
using NormalDist = boost::random::normal_distribution<>;

struct RNG::Impl
{
  Impl()
  {
    rng.reset(NEW boost::random::mt19937(static_cast<unsigned int>(std::time(0))));
  }

  std::unique_ptr<boost::random::mt19937> rng;
};

RNG::RNG() :
  m_impl{ NEW Impl() }
{}

RNG::~RNG() {}

RNG& RNG::instance()
{
  static std::unique_ptr<RNG> rng(NEW RNG());

  return *(rng.get());
}

bool RNG::flip_coin()
{
  UniformIntDist coin(0, 1);
  return (coin(*(m_impl->rng)) == 0 ? false : true);
}

/// Pick a number out of a uniform distribution.
unsigned int RNG::pick_uniform(unsigned int min, unsigned int max)
{
  UniformIntDist number(min, max);
  return number(*(m_impl->rng));
}

/// Pick a number out of a uniform distribution.
int RNG::pick_uniform(int min, int max)
{
  UniformIntDist number(min, max);
  return number(*(m_impl->rng));
}

double RNG::pick_normal(double min, double max)
{
  NormalDist number(min, max);
  return number(*(m_impl->rng));
}
