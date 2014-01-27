#ifndef GETLETTERKEY_H_INCLUDED
#define GETLETTERKEY_H_INCLUDED

#include <SFML/Graphics.hpp>

#include "Direction.h"

/// Return a number associated with the letter key pressed, or -1 if the key
/// pressed was not a letter.
/// This function is designed to make it easier to process keys corresponding
/// to inventory slots.  It returns a number where A = 1, B = 2, et cetera, if
/// a letter key is pressed.  In addition, in order to screen for "@" meaning
/// the player, it returns 0 if the 2/@ key was pressed with shift held down.
/// If the key pressed was none of these, it returns -1.
inline int get_letter_key(sf::Event::KeyEvent& key)
{
  switch (key.code)
  {
    case sf::Keyboard::Key::A:  return  1;
    case sf::Keyboard::Key::B:  return  2;
    case sf::Keyboard::Key::C:  return  3;
    case sf::Keyboard::Key::D:  return  4;
    case sf::Keyboard::Key::E:  return  5;
    case sf::Keyboard::Key::F:  return  6;
    case sf::Keyboard::Key::G:  return  7;
    case sf::Keyboard::Key::H:  return  8;
    case sf::Keyboard::Key::I:  return  9;
    case sf::Keyboard::Key::J:  return 10;
    case sf::Keyboard::Key::K:  return 11;
    case sf::Keyboard::Key::L:  return 12;
    case sf::Keyboard::Key::M:  return 13;
    case sf::Keyboard::Key::N:  return 14;
    case sf::Keyboard::Key::O:  return 15;
    case sf::Keyboard::Key::P:  return 16;
    case sf::Keyboard::Key::Q:  return 17;
    case sf::Keyboard::Key::R:  return 18;
    case sf::Keyboard::Key::S:  return 19;
    case sf::Keyboard::Key::T:  return 20;
    case sf::Keyboard::Key::U:  return 21;
    case sf::Keyboard::Key::V:  return 22;
    case sf::Keyboard::Key::W:  return 23;
    case sf::Keyboard::Key::X:  return 24;
    case sf::Keyboard::Key::Y:  return 25;
    case sf::Keyboard::Key::Z:  return 26;
    case sf::Keyboard::Key::Num2:
      if (key.shift)
      {
        return 0;
      }
      else
      {
        return -1;
      }
    default:  return -1;
      break;
  } // end switch (key.code)
}

/// Return the direction associated with a keypress, or None if the key does
/// not correspond to a direction.
Direction get_direction_key(sf::Event::KeyEvent& key)
{
  switch (key.code)
  {
  case sf::Keyboard::Key::Up:       return Direction::North;
  case sf::Keyboard::Key::PageUp:   return Direction::Northeast;
  case sf::Keyboard::Key::Right:    return Direction::East;
  case sf::Keyboard::Key::PageDown: return Direction::Southeast;
  case sf::Keyboard::Key::Down:     return Direction::South;
  case sf::Keyboard::Key::End:      return Direction::Southwest;
  case sf::Keyboard::Key::Left:     return Direction::West;
  case sf::Keyboard::Key::Home:     return Direction::Northwest;
  case sf::Keyboard::Key::Period:
    if (!key.shift)
    {
      return Direction::Self;
    }
    else
    {
      return Direction::Up;
    }
  case sf::Keyboard::Key::Comma:
    if (!key.shift)
    {
      return Direction::None;
    }
    else
    {
      return Direction::Down;
    }
  default:                          return Direction::None;
  } // end switch (key.code)
}
#endif // GETLETTERKEY_H_INCLUDED
