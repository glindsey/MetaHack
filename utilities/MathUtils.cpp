#include "stdafx.h"

#include "utilities/MathUtils.h"

namespace Math
{

  unsigned int nextPowerOfTwo(unsigned int n)
  {
    n--;
    n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
    n |= n >> 2;   // and then or the results.
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;           // The result is a number of 1 bits equal to the number
                   // of bits in the original number, plus 1. That's the
                   // next highest power of 2.
    return n;
  }

  double slope(double x1, double y1, double x2, double y2)
  {
    return (x1 - x2) / (y1 - y2);
  }

  float slope(RealVec2 s1, RealVec2 s2)
  {
    float x_dist = static_cast<float>(s1.x - s2.x);
    float y_dist = static_cast<float>(s1.y - s2.y);
    return x_dist / y_dist;
  }

  double invSlope(double x1, double y1, double x2, double y2)
  {
    return (y1 - y2) / (x1 - x2);
  }

  float invSlope(RealVec2 s1, RealVec2 s2)
  {
    float x_dist = static_cast<float>(s1.x - s2.x);
    float y_dist = static_cast<float>(s1.y - s2.y);
    return y_dist / x_dist;
  }

  int distSquared(int x1, int y1, int x2, int y2)
  {
    return ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
  }

  float distSquared(IntVec2 s1, IntVec2 s2)
  {
    float x_dist = static_cast<float>(s1.x - s2.x);
    float y_dist = static_cast<float>(s1.y - s2.y);
    return (x_dist * x_dist) + (y_dist * y_dist);
  }

  RealVec2 toRealVec2(IntVec2 vec)
  {
    return RealVec2{ static_cast<float>(vec.x), static_cast<float>(vec.y) };
  }

  unsigned int divideAndRoundUp(unsigned int value, unsigned int multiple)
  {
    return ((value / multiple) + ((value % multiple == 0) ? 0 : 1));
  }

  /// Return whether two sets of coordinates are adjacent to each other.
  /// In this context, adjacent includes corners as well as edges.
  /// (e.g. A tile at (4, 4) and a tile at (5, 5) are considered adjacent.)
  /// A tile is also considered adjacent to itself.
  bool adjacent(IntVec2 first, IntVec2 second)
  {
    return ((abs(first.x - second.x) <= 1) && (abs(first.y - second.y) <= 1));
  }

} // end namespace