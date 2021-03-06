#pragma once

#include "utilities/MathUtils.h"

/// Enum used for storing directional indices, i.e. when a Direction needs
/// to be used as a map key.
enum class DirectionIndex
{
  None, 
  Self, 
  Center, 
  North, 
  Northeast, 
  East, 
  Southeast, 
  South, 
  Southwest, 
  West, 
  Northwest, 
  Up, 
  Down
};

/// Directional vector in 3-D space.
/// A std::vector or IntVec3 could be used here, but I'd like to keep
/// game directional information separated by class for code legibility.
/// (Plus, I can enforce restrictions, such as the fact that this must be a
///  unit vector.)
class Direction final
{
public:
  Direction();
  Direction(int x, int y, int z);
  Direction(int x, int y);
  Direction(IntVec2 vec);
  Direction(IntVec3 vec);
  Direction(DirectionIndex dir);

  // Explicit defaults not specifically needed, but here for completeness
  Direction(Direction const&) = default;
  Direction(Direction&&) = default;
  Direction& operator=(Direction const&) = default;
  Direction& operator=(Direction&&) = default;
  virtual ~Direction() = default;

  friend void to_json(json& j, Direction const& direction);
  friend void from_json(json const& j, Direction& direction);

  // Equality operators
  bool operator==(Direction const& other) const;
  bool operator!=(Direction const& other) const;

  // Getters
  bool exists() const;
  int x() const;
  int y() const;
  int z() const;
  RealVec2 half() const;

  // Casts
  explicit operator IntVec2() const;
  explicit operator RealVec2() const;
  explicit operator IntVec3() const;
  explicit operator Vec3f() const;

  // Other methods
  operator DirectionIndex() const;

  unsigned int get_map_index() const 
  {
    return static_cast<unsigned int>(DirectionIndex(*this));
  }

  // Static methods
  static Direction get_approx(int xSrc, int ySrc, int xDst, int yDst);
  static float calculate_light_factor(IntVec2 source, IntVec2 target, Direction direction);

  // Static directions used for common directions
  static Direction const None;
  static Direction const Self;
  static Direction const Center;    ///< Alias for Direction::Self
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

  /// A vector of the four cardinal directions, starting with north and proceeding clockwise.
  static std::vector<Direction> const CardinalDirections;

  /// A vector of the four diagonal directions, starting with northwest and proceeding clockwise.
  static std::vector<Direction> const DiagonalDirections;

  /// A vector of the eight compass directions, starting with northwest and proceeding clockwise.
  static std::vector<Direction> const CompassDirections;

private:
  /// Map of Direction to DirectionIndex.
  static std::map<Direction, DirectionIndex> const m_dirToIndex;

  /// Map of DirectionIndex to Direction.
  static std::map<DirectionIndex, Direction> const m_indexToDir;

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
