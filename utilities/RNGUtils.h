#pragma once

#include "stdafx.h"

#include "game/App.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

/// Return the results of a coin flip.
inline bool flip_coin()
{
  uniform_int_dist coin(0, 1);
  return (coin(the_RNG) == 0 ? false : true);
}

/// Pick a number out of a uniform distribution.
inline unsigned int pick_uniform(unsigned int min, unsigned int max)
{
  uniform_int_dist number(min, max);
  return number(the_RNG);
}

/// Pick a number out of a uniform distribution.
inline int pick_uniform(int min, int max)
{
  uniform_int_dist number(min, max);
  return number(the_RNG);
}

/// Choose one of two alternatives at random.
template <class T> T choose_random(T a, T b)
{
  uniform_int_dist choose(0, 1);
  int choice = choose(the_RNG);
  return (choice ? a : b);
}

/// Choose one of three alternatives at random.
template <class T> T choose_random(T a, T b, T c)
{
  uniform_int_dist choose(0, 2);
  int choice = choose(the_RNG);
  switch (choice)
  {
    case 0: return a;
    case 1: return b;
    case 2: return c;

    default: return b;  // should not happen, here to shut compiler up
  }
}