#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "Direction.h"

const double PI = 3.14159265359;
constexpr double PI_HALF = PI / 2.0;
constexpr double PI_QUARTER = PI / 4.0;

inline unsigned int next_power_of_two(unsigned int n)
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

inline float calc_slope(float x1, float y1, float x2, float y2)
{
  return (x1 - x2) / (y1 - y2);
}

inline float calc_inv_slope(float x1, float y1, float x2, float y2)
{
  return (y1 - y2) / (x1 - x2);
}

inline int calc_vis_distance(int x1, int y1, int x2, int y2)
{
  return ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
}

inline Direction get_approx_direction(int xSrc, int ySrc, int xDst, int yDst)
{
  if (yDst < ySrc)
  {
    if (xDst < xSrc)
    {
      return Direction::Northwest;
    }
    else if (xDst == xSrc)
    {
      return Direction::North;
    }
    else // if (xDst > xSrc)
    {
      return Direction::Northeast;
    }
  }
  else if (yDst == ySrc)
  {
    if (xDst < xSrc)
    {
      return Direction::West;
    }
    else if (xDst == xSrc)
    {
      return Direction::Self;
    }
    else // if (xDst > xSrc)
    {
      return Direction::East;
    }
  }
  else // if (yDst > ySrc)
  {
    if (xDst < xSrc)
    {
      return Direction::Southwest;
    }
    else if (xDst == xSrc)
    {
      return Direction::South;
    }
    else // if (xDst > xSrc)
    {
      return Direction::Southeast;
    }
  }
}

inline unsigned char saturation_add(unsigned char const& a,
                                    unsigned char const& b)
{
  unsigned int temp;
  temp = (static_cast<unsigned int>(a) + static_cast<unsigned int>(b));
  return static_cast<unsigned char>((temp <= 255U) ? temp : 255U);
}

#endif // _MATHUTILS_H_
