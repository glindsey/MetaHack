#pragma once

/// Definition of a three-dimensional vector.
template <typename T>
class Vec3
{
public:
  Vec3()
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

  inline bool operator==(Vec3 const& lhs, Vec3 const& rhs)
  {
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
  }


  inline bool operator!=(Vec3 const& lhs, Vec3 const& rhs)
  {
    return !(lhs == rhs);
  }

  T x, y, z;
};


using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<unsigned int>;
