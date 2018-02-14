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

  Vec3(sf::Vector3<T> vec)
    :
    x{ vec.x }, y{ vec.y }, z{ vec.z }
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
    os << "(" << obj.x << ", " << obj.y << ", " << obj.z << ")";
    return os;
  }

  operator sf::Vector3<T>() const
  {
    return{ x, y, z };
  }

  T x, y, z;
};

using Vec3f = Vec3<float>;
using IntVec3 = Vec3<int>;
