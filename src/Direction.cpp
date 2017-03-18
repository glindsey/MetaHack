#include "stdafx.h"

#include "Direction.h"

// === STATIC INSTANCES =======================================================
Direction const Direction::None;
Direction const Direction::Self{ 0, 0 };
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

Direction::Direction(Vec3i vec)
  :
  Direction(vec.x, vec.y, vec.z)
{}

// Equality operators
bool Direction::operator==(Direction const& other) const
{
  return ((this->m_exists == other.m_exists) &&
    (this->m_x == other.m_x) &&
          (this->m_y == other.m_y) &&
          (this->m_z == other.m_z));
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

Direction::operator Vec3i() const
{
  return Vec3i(m_x, m_y, m_z);
}

Direction::operator Vec3f() const
{
  return Vec3f((float)m_x, (float)m_y, (float)m_z);
}

// Other methods
unsigned int Direction::get_map_index() const
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