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

  T x, y;
};

class Vec2f 
  : 
  public Vec2<float>
{
public:
  Vec2f()
    :
    Vec2()
  {}

  Vec2f(float x_, float y_)
    :
    Vec2(x_, y_)
  {}

  virtual ~Vec2f() = default;
  Vec2f(Vec2f const& other) = default;
  Vec2f(Vec2f&& other) = default;
  Vec2f& operator=(Vec2f const& other) = default;
  Vec2f& operator=(Vec2f&& other) = default;

  operator sf::Vector2f() const
  {
    return{ x, y };
  }
};

class Vec2i 
  : 
  public Vec2<int>
{
public:
  Vec2i()
    :
    Vec2()
  {}

  Vec2i(int x_, int y_)
    :
    Vec2(x_, y_)
  {}

  virtual ~Vec2i() = default;
  Vec2i(Vec2i const& other) = default;
  Vec2i(Vec2i&& other) = default;
  Vec2i& operator=(Vec2i const& other) = default;
  Vec2i& operator=(Vec2i&& other) = default;

  operator sf::Vector2i() const
  {
    return{ x, y };
  }
};

class Vec2u 
  : 
  public Vec2<unsigned int>
{
public:
  Vec2u()
    :
    Vec2()
  {}

  Vec2u(unsigned int x_, unsigned int y_)
    :
    Vec2(x_, y_)
  {}

  virtual ~Vec2u() = default;
  Vec2u(Vec2u const& other) = default;
  Vec2u(Vec2u&& other) = default;
  Vec2u& operator=(Vec2u const& other) = default;
  Vec2u& operator=(Vec2u&& other) = default;

  operator sf::Vector2u() const
  {
    return{ x, y };
  }
};
