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

  Vec2(sf::Vector2<T> vec)
    :
    x{ vec.x }, y{ vec.y } 
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

  operator sf::Vector2<T>() const
  {
    return{ x, y };
  }

  T x, y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;