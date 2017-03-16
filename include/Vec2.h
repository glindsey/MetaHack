#pragma once

#include "SFML/Graphics.hpp"

/// Definition of a two-dimensional vector.
template <typename T>
class Vec2
{
public:
  Vec2()
  {}

  Vec2(T x_, T y_)
    :
    x{ x_ }, y{ y_ }
  {}

  template <typename U>
  Vec2(sf::Vector2<U> vec)
    :
    x{ static_cast<T>(vec.x) }, y{ static_cast<T>(vec.y) } 
  {}

  virtual ~Vec2() = default;
  Vec2(Vec2 const& other) = default;
  Vec2(Vec2&& other) = default;
  Vec2& operator=(Vec2 const& other) = default;
  Vec2& operator=(Vec2&& other) = default;

  Vec2& operator+=(Vec2 const& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  Vec2& operator-=(Vec2 const& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  friend Vec2 operator+(Vec2 lhs, Vec2 const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend Vec2 operator-(Vec2 lhs, Vec2 const& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend bool operator==(Vec2 const& lhs, Vec2 const& rhs)
  {
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
  }

  friend bool operator!=(Vec2 const& lhs, Vec2 const& rhs)
  {
    return !(lhs == rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, Vec2 const& obj)
  {
    os << "(" << obj.x << ", " << obj.y << ")";
    return os;
  }

  T r()
  {
    return static_cast<T>(sqrt((x * x) + (y * y)));
  }

  double theta()
  {
    return arctan(static_cast<double>(y) / static_cast<double>(x));
  }

  static T square_distance(Vec2 const& first, Vec2 const& second)
  {
    auto xDiff = (first.x > second.x) ? (first.x - second.x) : (second.x - first.x);
    auto yDiff = (first.y > second.y) ? (first.y - second.y) : (second.y - first.y);

    return (xDiff * xDiff) + (yDiff * yDiff);
  }

  static T distance(Vec2 const& first, Vec2 const& second)
  {
    return static_cast<T>(sqrt(square_distance(first, second)));
  }

  template <typename U>
  operator sf::Vector2<U>() const
  {
    return{ static_cast<U>(x), static_cast<U>(y) };
  }

  T x, y;
};

using RealVec2 = Vec2<float>;
using IntegerVec2 = Vec2<int32_t>;
using Vec2u = Vec2<unsigned int>;
