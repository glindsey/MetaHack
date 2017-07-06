#include "stdafx.h"

#include "types/Direction.h"

// === STATIC INSTANCES =======================================================
Direction const Direction::None;
Direction const Direction::Self{ 0, 0 };
Direction const Direction::Center{ 0, 0 };
Direction const Direction::North{ 0, -1 };
Direction const Direction::Northeast{ 1, -1 };
Direction const Direction::East{ 1, 0 };
Direction const Direction::Southeast{ 1, 1 };
Direction const Direction::South{ 0, 1 };
Direction const Direction::Southwest{ -1, 1 };
Direction const Direction::West{ -1, 0 };
Direction const Direction::Northwest{ -1, -1 };
Direction const Direction::Up{ 0, 0, -1 };
Direction const Direction::Down{ 0, 0, 1 };

/// Map of Direction to DirectionIndex.
std::map<Direction, DirectionIndex> const Direction::m_dirToIndex
{
  { Direction::None,      DirectionIndex::None      },
  { Direction::Self,      DirectionIndex::Self      },
  { Direction::Center,    DirectionIndex::Center    },
  { Direction::North,     DirectionIndex::North     },
  { Direction::Northeast, DirectionIndex::Northeast },
  { Direction::East,      DirectionIndex::East      },
  { Direction::Southeast, DirectionIndex::Southeast },
  { Direction::South,     DirectionIndex::South     },
  { Direction::Southwest, DirectionIndex::Southwest },
  { Direction::West,      DirectionIndex::West      },
  { Direction::Northwest, DirectionIndex::Northwest },
  { Direction::Up,        DirectionIndex::Up        },
  { Direction::Down,      DirectionIndex::Down      }
};

/// Map of DirectionIndex to Direction.
std::map<DirectionIndex, Direction> const Direction::m_indexToDir
{
  { DirectionIndex::None,      Direction::None      },
  { DirectionIndex::Self,      Direction::Self      },
  { DirectionIndex::Center,    Direction::Center    },
  { DirectionIndex::North,     Direction::North     },
  { DirectionIndex::Northeast, Direction::Northeast },
  { DirectionIndex::East,      Direction::East      },
  { DirectionIndex::Southeast, Direction::Southeast },
  { DirectionIndex::South,     Direction::South     },
  { DirectionIndex::Southwest, Direction::Southwest },
  { DirectionIndex::West,      Direction::West      },
  { DirectionIndex::Northwest, Direction::Northwest },
  { DirectionIndex::Up,        Direction::Up        },
  { DirectionIndex::Down,      Direction::Down      }
};

/// A vector of the four cardinal directions, starting with north and proceeding clockwise.
std::vector<Direction> const Direction::CardinalDirections
{
  Direction::North, Direction::East, Direction::South, Direction::West
};

/// A vector of the four diagonal directions, starting with northwest and proceeding clockwise.
std::vector<Direction> const Direction::DiagonalDirections
{
  Direction::Northwest, Direction::Northeast, Direction::Southeast, Direction::Southwest
};

/// A vector of the eight compass directions, starting with northwest and proceeding clockwise.
std::vector<Direction> const Direction::CompassDirections
{
  Direction::Northwest, Direction::North, Direction::East, Direction::Southeast, Direction::South, Direction::Southwest, Direction::West
};

Direction::Direction()
  :
  m_exists{ false },
  m_x{ 0 },
  m_y{ 0 },
  m_z{ 0 },
  m_halfx{ 0.0f },
  m_halfy{ 0.0f },
  m_halfz{ 0.0f }
{}

Direction::Direction(int x, int y, int z)
  :
  m_exists{ true },
  m_x{ boost::math::sign(x) },
  m_y{ boost::math::sign(y) },
  m_z{ boost::math::sign(z) },
  m_halfx{ (float)x / 2.0f },
  m_halfy{ (float)y / 2.0f },
  m_halfz{ (float)z / 2.0f }
{}

Direction::Direction(int x, int y)
  :
  Direction(x, y, 0)
{}

Direction::Direction(IntVec2 vec)
  :
  Direction(vec.x, vec.y, 0)
{}

Direction::Direction(IntVec3 vec)
  :
  Direction(vec.x, vec.y, vec.z)
{}

Direction::Direction(DirectionIndex index)
{
  auto& dir = (m_indexToDir.count(index) != 0) ? m_indexToDir.at(index) : m_indexToDir.at(DirectionIndex::None);
  this->m_exists = dir.m_exists;
  this->m_x = dir.m_x;
  this->m_y = dir.m_y;
  this->m_z = dir.m_z;
}

// Equality operators
bool Direction::operator==(Direction const& other) const
{
  return
    (this->m_exists == other.m_exists) &&
    (this->m_x == other.m_x) &&
    (this->m_y == other.m_y) &&
    (this->m_z == other.m_z);
}

bool Direction::operator!=(Direction const& other) const
{
  return !(*this == other);
}

// Getters
bool Direction::exists() const
{
  return m_exists;
}

int Direction::x() const
{
  return m_x;
}

int Direction::y() const
{
  return m_y;
}

int Direction::z() const
{
  return m_z;
}

RealVec2 Direction::half() const
{
  return RealVec2(m_halfx, m_halfy);
}

// Casts
Direction::operator IntVec2() const
{
  return IntVec2(m_x, m_y);
}

Direction::operator RealVec2() const
{
  return RealVec2((float)m_x, (float)m_y);
}

Direction::operator IntVec3() const
{
  return IntVec3(m_x, m_y, m_z);
}

Direction::operator Vec3f() const
{
  return Vec3f((float)m_x, (float)m_y, (float)m_z);
}

// Other methods
Direction::operator DirectionIndex() const
{
  for (auto& pair : m_dirToIndex)
  {
    if (pair.first == *this) return pair.second;
  }

  return DirectionIndex::None;
}

Direction Direction::get_approx(int xSrc, int ySrc, int xDst, int yDst)
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
float Direction::calculate_light_factor(IntVec2 source, IntVec2 target, Direction direction)
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

void to_json(json& j, Direction const& direction)
{
  j = json::array({ "direction", direction.m_x, direction.m_y, direction.m_z });
}

void from_json(json const& j, Direction& direction)
{
  Assert("Types",
         j.is_array() && (j[0] == "direction") && (j.size() >= 4),
         "Attempted to create a Color out of an invalid JSON object");

  direction.m_exists = true;
  direction.m_x = boost::math::sign(j[1].get<int>());
  direction.m_y = boost::math::sign(j[2].get<int>());
  direction.m_z = boost::math::sign(j[3].get<int>());
  direction.m_halfx = j[1].get<float>() / 2.0f;
  direction.m_halfy = j[2].get<float>() / 2.0f;
  direction.m_halfz = j[3].get<float>() / 2.0f;
}
