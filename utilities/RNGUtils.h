#pragma once

#include "game/App.h"

using UniformIntDist = boost::random::uniform_int_distribution<>;

/// Return the results of a coin flip.
inline bool flip_coin()
{
  UniformIntDist coin(0, 1);
  return (coin(the_RNG) == 0 ? false : true);
}

/// Pick a number out of a uniform distribution.
inline unsigned int pick_uniform(unsigned int min, unsigned int max)
{
  UniformIntDist number(min, max);
  return number(the_RNG);
}

/// Pick a number out of a uniform distribution.
inline int pick_uniform(int min, int max)
{
  UniformIntDist number(min, max);
  return number(the_RNG);
}

/// Choose one of two alternatives at random.
template <class T> T choose_random(T a, T b)
{
  UniformIntDist choose(0, 1);
  int choice = choose(the_RNG);
  return (choice ? a : b);
}

/// Choose one of three alternatives at random.
template <class T> T choose_random(T a, T b, T c)
{
  UniformIntDist choose(0, 2);
  int choice = choose(the_RNG);
  switch (choice)
  {
    case 0: return a;
    case 1: return b;
    case 2: return c;

    default: return b;  // should not happen, here to shut compiler up
  }
}