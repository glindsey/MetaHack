#pragma once

#include "SFML/Graphics.hpp"

/// Definition of a three-dimensional vector.
template <typename T>
class Vec3
{
public:
  Vec3()
  {}

  Vec3(T x_, T y_, T z_)
    :
    x{ x_ }, y{ y_ }, z{ z_ }
  {}


  virtual ~Vec3() = default;
  Vec3(Vec3 const& other) = default;
  Vec3(Vec3&& other) = default;
  Vec3& operator=(Vec3 const& other) = default;
  Vec3& operator=(Vec3&& other) = default;

  Vec3& operator+=(Vec3 const& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vec3& operator-=(Vec3 const& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  friend Vec3 operator+(Vec3 lhs, Vec3 const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend Vec3 operator-(Vec3 lhs, Vec3 const& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend bool operator==(Vec3 const& lhs, Vec3 const& rhs)
  {
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
  }


  friend bool operator!=(Vec3 const& lhs, Vec3 const& rhs)
  {
    return !(lhs == rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, Vec3 const& obj)
  {
    os << "(" << obj.x << ", " << obj.y << ", " << obj.z ")";
    return os;
  }


  T x, y, z;
};


class Vec3f
  :
  public Vec3<float>
{
public:
  Vec3f()
    :
    Vec3()
  {}

  Vec3f(float x_, float y_, float z_)
    :
    Vec3(x_, y_, z_)
  {}

  virtual ~Vec3f() = default;
  Vec3f(Vec3f const& other) = default;
  Vec3f(Vec3f&& other) = default;
  Vec3f& operator=(Vec3f const& other) = default;
  Vec3f& operator=(Vec3f&& other) = default;

  operator sf::Vector3f() const
  {
    return{ x, y, z };
  }
};

class Vec3i
  :
  public Vec3<int>
{
public:
  Vec3i()
    :
    Vec3()
  {}

  Vec3i(int x_, int y_, int z_)
    :
    Vec3(x_, y_, z_)
  {}

  virtual ~Vec3i() = default;
  Vec3i(Vec3i const& other) = default;
  Vec3i(Vec3i&& other) = default;
  Vec3i& operator=(Vec3i const& other) = default;
  Vec3i& operator=(Vec3i&& other) = default;

  operator sf::Vector3i() const
  {
    return{ x, y, z };
  }
};