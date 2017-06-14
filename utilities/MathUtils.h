#pragma once

#include "types/common.h"

namespace Math
{
  constexpr double PI = 3.14159265359;
  constexpr double PI_HALF = PI / 2.0;
  constexpr double PI_QUARTER = PI / 4.0;

  unsigned int nextPowerOfTwo(unsigned int n);
  double slope(double x1, double y1, double x2, double y2);
  float slope(RealVec2 s1, RealVec2 s2);
  double invSlope(double x1, double y1, double x2, double y2);
  float invSlope(RealVec2 s1, RealVec2 s2);
  int distSquared(int x1, int y1, int x2, int y2);
  float distSquared(IntVec2 s1, IntVec2 s2);
  RealVec2 toRealVec2(IntVec2 vec);
  unsigned int divideAndRoundUp(unsigned int value, unsigned int multiple);
  bool adjacent(IntVec2 first, IntVec2 second);

  template <typename T>
  T bounded(T lo, T val, T hi)
  {
    return std::max(std::min(val, hi), lo);
  }
} // end namespace