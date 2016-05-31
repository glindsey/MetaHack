#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "stdafx.h"

constexpr double PI = 3.14159265359;
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

inline sf::Vector2f to_v2f(sf::Vector2i vec)
{
  return sf::Vector2f{ static_cast<float>(vec.x), static_cast<float>(vec.y) };
}

inline unsigned char saturation_add(unsigned char const& a,
                                    unsigned char const& b)
{
  unsigned int temp;
  temp = (static_cast<unsigned int>(a) + static_cast<unsigned int>(b));
  return static_cast<unsigned char>((temp <= 255U) ? temp : 255U);
}

inline sf::Color saturation_add(sf::Color const& a,
                                sf::Color const& b)
{
  sf::Color temp;
  temp.r = saturation_add(a.r, b.r);
  temp.g = saturation_add(a.g, b.g);
  temp.b = saturation_add(a.b, b.b);
  temp.a = saturation_add(a.a, b.a);
  return temp;
}

/// Divide and round up.
inline unsigned int divide_and_round_up(unsigned int value, unsigned int multiple)
{
  return ((value / multiple) + ((value % multiple == 0) ? 0 : 1));
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
  return ((abs(first.x - second.x) <= 1) && (abs(first.y - second.y) <= 1));
}

#endif // _MATHUTILS_H_
