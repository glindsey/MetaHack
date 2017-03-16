#ifndef DIRECTION_H
#define DIRECTION_H

#include "stdafx.h"

#include "MathUtils.h"

/// Directional vector in 3-D space.
/// A std::vector or Vec3i could be used here, but I'd like to keep
/// game directional information separated by class for code legibility.
/// (Plus, I can enforce restrictions, such as the fact that this must be a
///  unit vector.)
class Direction final
{
public:
  Direction();
  Direction(int x, int y, int z);
  Direction(int x, int y);
  Direction(IntegerVec2 vec);
  Direction(Vec3i vec);

  // Explicit defaults not specifically needed, but here for completeness
  Direction(Direction const&) = default;
  Direction(Direction&&) = default;
  Direction& operator=(Direction const&) = default;
  Direction& operator=(Direction&&) = default;
  virtual ~Direction() = default;

  // Equality operators
  bool Direction::operator==(Direction const& other) const;
  bool Direction::operator!=(Direction const& other) const;

  // Getters
  bool exists() const;
  int x() const;
  int y() const;
  int z() const;
  RealVec2 half() const;

  // Casts
  explicit operator IntegerVec2() const;
  explicit operator RealVec2() const;
  explicit operator Vec3i() const;
  explicit operator Vec3f() const;

  // Other methods
  unsigned int get_map_index() const;

  // Static methods
  static Direction get_approx(int xSrc, int ySrc, int xDst, int yDst);
  static float calculate_light_factor(IntegerVec2 source, IntegerVec2 target, Direction direction);

  // Static directions used for common directions
  static Direction const None;
  static Direction const Self;
  static Direction const North;
  static Direction const Northeast;
  static Direction const East;
  static Direction const Southeast;
  static Direction const South;
  static Direction const Southwest;
  static Direction const West;
  static Direction const Northwest;
  static Direction const Up;
  static Direction const Down;

private:
  bool m_exists;
  int m_x;
  int m_y;
  int m_z;

  // Half-units are memoized to save on computation time.
  // There's no need to re-do the division every time half() is called.
  float m_halfx;
  float m_halfy;
  float m_halfz;
};

inline std::ostream& operator<<(std::ostream& os, Direction const& d)
{
  if (d == Direction::None) os << "none";
  else if (d == Direction::Self) os << "self";
  else if (d == Direction::North) os << "north";
  else if (d == Direction::Northeast) os << "northeast";
  else if (d == Direction::East) os << "east";
  else if (d == Direction::Southeast) os << "southeast";
  else if (d == Direction::South) os << "south";
  else if (d == Direction::Southwest) os << "southwest";
  else if (d == Direction::West) os << "west";
  else if (d == Direction::Northwest) os << "northwest";
  else
  {
    os << "??? (" << d.x() << ", " << d.y() << ", " << d.z() << ")";
  }

  return os;
}

inline std::wostream& operator<<(std::wostream& os, Direction const& d)
{
  if (d == Direction::None) os << "none";
  else if (d == Direction::Self) os << "self";
  else if (d == Direction::North) os << "north";
  else if (d == Direction::Northeast) os << "northeast";
  else if (d == Direction::East) os << "east";
  else if (d == Direction::Southeast) os << "southeast";
  else if (d == Direction::South) os << "south";
  else if (d == Direction::Southwest) os << "southwest";
  else if (d == Direction::West) os << "west";
  else if (d == Direction::Northwest) os << "northwest";
  else
  {
    os << "??? (" << d.x() << ", " << d.y() << ", " << d.z() << ")";
  }

  return os;
}

#endif // DIRECTION_H
