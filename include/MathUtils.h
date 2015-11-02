#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "Direction.h"

const double PI = 3.14159265359;
constexpr double PI_HALF = PI / 2.0;
constexpr double PI_QUARTER = PI / 4.0;

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

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

inline unsigned int iabs(int value)
{
	return (value >= 0) ? 
		static_cast<unsigned int>(value) : 
		static_cast<unsigned int>(-value);
}

inline double calc_slope(double x1, double y1, double x2, double y2)
{
  return (x1 - x2) / (y1 - y2);
}

inline double calc_inv_slope(double x1, double y1, double x2, double y2)
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

inline bool flip_coin()
{
  uniform_int_dist coin(0, 1);
  return (coin(the_RNG) == 0 ? false : true);
}

/// Choose one of two alternatives at random.
template <class T> T choose_random (T a, T b)
{
  uniform_int_dist choose(0, 1);
  int choice = choose(the_RNG);
  return (choice ? a : b);
}

/// Choose one of three alternatives at random.
template <class T> T choose_random (T a, T b, T c)
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

/// Divide and round up.
inline unsigned int divide_and_round_up(unsigned int value, unsigned int multiple)
{
  return ((value / multiple) + ((value % multiple == 0) ? 0 : 1));
}

/// Determine light factor based on light source, wall location, and which wall
/// the light is hitting.
inline float calculate_light_factor(sf::Vector2i source, sf::Vector2i target, Direction direction)
{
  float x_diff = static_cast<float>(iabs(source.x - target.x));
  float y_diff = static_cast<float>(iabs(source.y - target.y));

  if ((x_diff == 0) && (y_diff == 0)) return 0;

  float h_diff = sqrt((x_diff * x_diff) + (y_diff * y_diff));

  switch (direction)
  {
  case Direction::North:
    return (source.y < target.y) ? (y_diff / h_diff) : 0;
  case Direction::South:
    return (source.y > target.y) ? (y_diff / h_diff) : 0;
  case Direction::West:
    return (source.x < target.x) ? (x_diff / h_diff) : 0;
  case Direction::East:
    return (source.x > target.x) ? (x_diff / h_diff) : 0;
  default:
    throw std::out_of_range(std::string("Invalid direction " + boost::lexical_cast<std::string>(direction) + " passed to calculate_light_factor").c_str());
  }
}

#endif // _MATHUTILS_H_
