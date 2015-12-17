#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <SFML/Graphics.hpp>

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

inline float calc_slope(sf::Vector2f s1, sf::Vector2f s2)
{
  float x_dist = static_cast<float>(s1.x - s2.x);
  float y_dist = static_cast<float>(s1.y - s2.y);
  return x_dist / y_dist;
}

inline double calc_inv_slope(double x1, double y1, double x2, double y2)
{
  return (y1 - y2) / (x1 - x2);
}

inline float calc_inv_slope(sf::Vector2f s1, sf::Vector2f s2)
{
  float x_dist = static_cast<float>(s1.x - s2.x);
  float y_dist = static_cast<float>(s1.y - s2.y);
  return y_dist / x_dist;
}

inline int calc_vis_distance(int x1, int y1, int x2, int y2)
{
  return ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
}

inline float calc_vis_distance(sf::Vector2i s1, sf::Vector2i s2)
{
  float x_dist = static_cast<float>(s1.x - s2.x);
  float y_dist = static_cast<float>(s1.y - s2.y);
  return (x_dist * x_dist) + (y_dist * y_dist);
}

inline sf::Vector2i unit(Direction direction)
{
  switch (direction)
  {
    case Direction::Northwest:  return{ -1, -1 };
    case Direction::North:      return{ 0, -1 };
    case Direction::Northeast:  return{ 1, -1 };
    case Direction::West:       return{ -1, 0 };
    case Direction::Self:       return{ 0, 0 };
    case Direction::East:       return{ 1, 0 };
    case Direction::Southwest:  return{ -1, 1 };
    case Direction::South:      return{ 0, 1 };
    case Direction::Southeast:  return{ 1, 1 };
    default:                    return{ 0, 0 };
  }
}

inline sf::Vector2f halfunit(Direction direction)
{
  switch (direction)
  {
    case Direction::Northwest:  return{ -0.5f, -0.5f };
    case Direction::North:      return{ 0.0f, -0.5f };
    case Direction::Northeast:  return{ 0.5f, -0.5f };
    case Direction::West:       return{ -0.5f,  0.0f };
    case Direction::Self:       return{ 0.0f,  0.0f };
    case Direction::East:       return{ 0.5f,  0.0f };
    case Direction::Southwest:  return{ -0.5f,  0.5f };
    case Direction::South:      return{ 0.0f,  0.5f };
    case Direction::Southeast:  return{ 0.5f,  0.5f };
    default:                    return{ 0.0f,  0.0f };
  }
}

inline sf::Vector2f to_v2f(sf::Vector2i vec)
{
  return sf::Vector2f{ static_cast<float>(vec.x), static_cast<float>(vec.y) };
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

  if ((x_diff == 0) && (y_diff == 0)) return 1;

  float h_diff = sqrt((x_diff * x_diff) + (y_diff * y_diff));

  switch (direction)
  {
    case Direction::Self:
    case Direction::Up:
    case Direction::Down:
      return 1;
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

/// Average two colors together.
inline sf::Color average(sf::Color first, sf::Color second)
{
  sf::Color result;
  result.r = (first.r + second.r) >> 1;
  result.g = (first.g + second.g) >> 1;
  result.b = (first.b + second.b) >> 1;
  result.a = (first.a + second.a) >> 1;
  return result;
}

/// Average four colors together.
inline sf::Color average(sf::Color first, sf::Color second, sf::Color third, sf::Color fourth)
{
  sf::Color result;
  result.r = (first.r + second.r + third.r + fourth.r) >> 2;
  result.g = (first.g + second.g + third.g + fourth.g) >> 2;
  result.b = (first.b + second.b + third.b + fourth.b) >> 2;
  result.a = (first.a + second.a + third.a + fourth.a) >> 2;
  return result;
}

/// Return whether two sets of coordinates are adjacent to each other.
/// In this context, adjacent includes corners as well as edges.
/// (e.g. A tile at (4, 4) and a tile at (5, 5) are considered adjacent.)
/// A tile is also considered adjacent to itself.
inline bool adjacent(sf::Vector2i first, sf::Vector2i second)
{
  return (iabs(first.x - second.x) <= 1) && (iabs(first.y - second.y) <= 1);
}

#endif // _MATHUTILS_H_
