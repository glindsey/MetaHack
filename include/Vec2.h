#pragma once

/// Definition of a two-dimensional vector.
template <typename T>
class Vec2
{
public:
  Vec2()
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

  inline bool operator==(Vec2 const& lhs, Vec2 const& rhs)
  {
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
  }

  inline bool operator!=(Vec2 const& lhs, Vec2 const& rhs)
  {
    return !(lhs == rhs);
  }

  T x, y;
};


using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;
