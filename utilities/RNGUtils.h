#pragma once

#include "game/App.h"

#define the_RNG   RNG::instance()

class RNG
{
public:
  virtual ~RNG();

  static RNG& instance();

  /// Return the results of a coin flip.
  bool flip_coin();

  /// Pick a number out of a uniform distribution.
  unsigned int pick_uniform(unsigned int min, unsigned int max);

  /// Pick a number out of a uniform distribution.
  int pick_uniform(int min, int max);

  /// Pick a number out of a normal distribution.
  double pick_normal(double min, double max);

  /// Choose one of two alternatives at random.
  template <class T> T choose_random(T a, T b)
  {
    int choice = pick_uniform(0, 1);
    return (choice ? a : b);
  }

  /// Choose one of three alternatives at random.
  template <class T> T choose_random(T a, T b, T c)
  {
    int choice = pick_uniform(0, 2);
    switch (choice)
    {
      case 0: return a;
      case 1: return b;
      case 2: return c;

      default: return b;  // should not happen, here to shut compiler up
    }
  }

protected:
  RNG();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
