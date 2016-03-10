#ifndef DIRECTION_H
#define DIRECTION_H

#include "stdafx.h"

#include "LuaObject.h"
#include "MathUtils.h"

/// Directional vector in 3-D space.
/// A std::vector or sf::Vector3i could be used here, but I'd like to keep
/// game directional information separated by class for code legibility.
/// (Plus, I can enforce restrictions, such as the fact that this must be a
///  unit vector.)
class Direction final
{
public:
  Direction()
    :
    m_exists{ false },
    m_x{ 0 },
    m_y{ 0 },
    m_z{ 0 },
    m_halfx{ 0.0f },
    m_halfy{ 0.0f },
    m_halfz{ 0.0f }
  {}

  Direction(int x, int y, int z = 0)
    :
    m_exists{ true },
    m_x{ boost::math::sign(x) },
    m_y{ boost::math::sign(y) },
    m_z{ boost::math::sign(z) },
    m_halfx{ (float)x / 2.0f },
    m_halfy{ (float)y / 2.0f },
    m_halfz{ (float)z / 2.0f }
  {}

  // Explicit defaults not specifically needed, but here for completeness
  Direction(Direction const&) = default;
  Direction(Direction&&) = default;
  Direction& operator=(Direction const&) = default;
  Direction& operator=(Direction&&) = default;
  virtual ~Direction() = default;

  // Equality operators
  inline bool Direction::operator==(Direction const& other) const
  {
    return ((this->m_exists == other.m_exists) &&
            (this->m_x == other.m_x) &&
            (this->m_y == other.m_y) &&
            (this->m_z == other.m_z));
  }

  inline bool Direction::operator!=(Direction const& other) const
  {
    return !(*this == other);
  }

  // Getters
  inline bool exists() const
  {
    return m_exists;
  }

  inline int x() const
  {
    return m_x;
  }

  inline int y() const
  {
    return m_y;
  }

  inline int z() const
  {
    return m_z;
  }

  // Casts
  inline explicit operator sf::Vector2i() const
  {
    return sf::Vector2i(m_x, m_y);
  }

  inline explicit operator sf::Vector2f() const
  {
    return sf::Vector2f((float)m_x, (float)m_y);
  }

  inline sf::Vector2f half() const
  {
    return sf::Vector2f(m_halfx, m_halfy);
  }

  inline explicit operator sf::Vector3i() const
  {
    return sf::Vector3i(m_x, m_y, m_z);
  }

  inline explicit operator sf::Vector3f() const
  {
    return sf::Vector3f((float)m_x, (float)m_y, (float)m_z);
  }

  inline unsigned int get_map_index() const
  {
    if (*this == Direction::None) return 1;
    if (*this == Direction::North) return 2;
    if (*this == Direction::Northeast) return 3;
    if (*this == Direction::East) return 4;
    if (*this == Direction::Southeast) return 5;
    if (*this == Direction::South) return 6;
    if (*this == Direction::Southwest) return 7;
    if (*this == Direction::West) return 8;
    if (*this == Direction::Northwest) return 9;
    if (*this == Direction::Up) return 10;
    if (*this == Direction::Down) return 11;
    if (*this == Direction::Self) return 12;

    return 0;
  }

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

/// Determine light factor based on light source, wall location, and which wall
/// the light is hitting.
inline float calculate_light_factor(sf::Vector2i source, sf::Vector2i target, Direction direction)
{
  float x_diff = abs((float)(source.x - target.x));
  float y_diff = abs((float)(source.y - target.y));

  if ((x_diff == 0) && (y_diff == 0)) return 1;

  float h_diff = sqrt((x_diff * x_diff) + (y_diff * y_diff));

  if ((direction == Direction::Self) ||
      (direction == Direction::Up) ||
      (direction == Direction::Down))
  {
    return 1;
  }

  if (direction == Direction::North)
  {
    return (source.y < target.y) ? (y_diff / h_diff) : 0;
  }

  if (direction == Direction::South)
  {
    return (source.y > target.y) ? (y_diff / h_diff) : 0;
  }

  if (direction == Direction::West)
  {
    return (source.x < target.x) ? (x_diff / h_diff) : 0;
  }

  if (direction == Direction::East)
  {
    return (source.x > target.x) ? (x_diff / h_diff) : 0;
  }

  throw std::runtime_error("Invalid direction " + str(direction) +
                           " passed to calculate_light_factor");
}

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

#if 0
enum class Direction
{
  None,
  North,
  Northeast,
  East,
  Southeast,
  South,
  Southwest,
  West,
  Northwest,
  Up,
  Down,
  Self,
  Count
};

inline void Direction_add_to_lua(Lua* lua_instance)
{
  lua_instance->add_enum("Direction",
                         "None", Direction::None,
                         "North", Direction::North,
                         "Northeast", Direction::Northeast,
                         "East", Direction::East,
                         "Southeast", Direction::Southeast,
                         "South", Direction::South,
                         "Southwest", Direction::Southwest,
                         "West", Direction::West,
                         "Northwest", Direction::Northwest,
                         "Up", Direction::Up,
                         "Down", Direction::Down,
                         "Self", Direction::Self,
                         "Count", Direction::Count,
                         0
                         );
}

inline Direction update_direction(Direction current_direction,
                                  Direction new_direction)
{
  switch (new_direction)
  {
    case Direction::North:
    case Direction::East:
    case Direction::South:
    case Direction::West:
      return new_direction;
    case Direction::Northeast:
      if (current_direction == Direction::North) return Direction::North;
      if (current_direction == Direction::East) return Direction::East;
      if (current_direction == Direction::South) return Direction::East;
      if (current_direction == Direction::West) return Direction::North;
      return current_direction;
    case Direction::Southeast:
      if (current_direction == Direction::North) return Direction::East;
      if (current_direction == Direction::East) return Direction::East;
      if (current_direction == Direction::South) return Direction::South;
      if (current_direction == Direction::West) return Direction::South;
      return current_direction;
    case Direction::Southwest:
      if (current_direction == Direction::North) return Direction::West;
      if (current_direction == Direction::East) return Direction::South;
      if (current_direction == Direction::South) return Direction::South;
      if (current_direction == Direction::West) return Direction::West;
      return current_direction;
    case Direction::Northwest:
      if (current_direction == Direction::North) return Direction::North;
      if (current_direction == Direction::East) return Direction::North;
      if (current_direction == Direction::South) return Direction::West;
      if (current_direction == Direction::West) return Direction::West;
      return current_direction;
    default:
      return current_direction;
  }
}

inline int get_appropriate_4way_tile(Direction direction)
{
  switch (direction)
  {
    case Direction::North: return 0;
    case Direction::East: return 1;
    case Direction::South: return 2;
    case Direction::West: return 3;
    default: return 2; // Default to facing player if direction isn't supported
  }
}

inline sf::Vector2i get_vector(Direction direction)
{
  switch (direction)
  {
    case Direction::North: return sf::Vector2i(0, -1);
    case Direction::Northeast: return sf::Vector2i(1, -1);
    case Direction::East: return sf::Vector2i(1, 0);
    case Direction::Southeast: return sf::Vector2i(1, 1);
    case Direction::South: return sf::Vector2i(0, 1);
    case Direction::Southwest: return sf::Vector2i(-1, 1);
    case Direction::West: return sf::Vector2i(-1, 0);
    case Direction::Northwest: return sf::Vector2i(-1, -1);
    default: return sf::Vector2i(0, 0);
  }
}
#endif

#endif // DIRECTION_H
