#ifndef DIRECTION_H
#define DIRECTION_H

#include <SFML/Graphics.hpp>

#include "Lua.h"

#include <iostream>
#include <sstream>

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

inline std::ostream& operator<<(std::ostream& os, Direction const& d)
{
  switch (d)
  {
    case Direction::None:       os << "none"; break;
    case Direction::North:      os << "north"; break;
    case Direction::Northeast:  os << "northeast"; break;
    case Direction::East:       os << "east"; break;
    case Direction::Southeast:  os << "southeast"; break;
    case Direction::South:      os << "south"; break;
    case Direction::Southwest:  os << "southwest"; break;
    case Direction::West:       os << "west"; break;
    case Direction::Northwest:  os << "northwest"; break;
    case Direction::Up:         os << "up"; break;
    case Direction::Down:       os << "down"; break;
    case Direction::Self:       os << "self"; break;
    case Direction::Count:      os << "count"; break;
    default:                    os << "???"; break;
  }

  return os;
}

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

#endif // DIRECTION_H
