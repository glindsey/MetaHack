#include "stdafx.h"

#include "keybuffer/KeyBuffer.h"

KeyBuffer::KeyBuffer()
{
  clear_buffer();
  m_replacing = false;
}

KeyBuffer::~KeyBuffer()
{
  //dtor
}

metagui::GUIEvent::Result KeyBuffer::handle_key_press(metagui::GUIEventKeyPressed& event)
{
  metagui::GUIEvent::Result result = metagui::GUIEvent::Result::Acknowledged;

  if (!event.control && !event.alt)
  {
    switch (event.code)
    {
      case sf::Keyboard::Key::Tilde:      return insertCharacter(event, '~', '`');
      case sf::Keyboard::Key::Num1:       return insertCharacter(event, '!', '1');
      case sf::Keyboard::Key::Num2:       return insertCharacter(event, '@', '2');
      case sf::Keyboard::Key::Num3:       return insertCharacter(event, '#', '3');
      case sf::Keyboard::Key::Num4:       return insertCharacter(event, '$', '4');
      case sf::Keyboard::Key::Num5:       return insertCharacter(event, '%', '5');
      case sf::Keyboard::Key::Num6:       return insertCharacter(event, '^', '6');
      case sf::Keyboard::Key::Num7:       return insertCharacter(event, '&', '7');
      case sf::Keyboard::Key::Num8:       return insertCharacter(event, '*', '8');
      case sf::Keyboard::Key::Num9:       return insertCharacter(event, '(', '9');
      case sf::Keyboard::Key::Num0:       return insertCharacter(event, ')', '0');
      case sf::Keyboard::Key::Dash:       return insertCharacter(event, '_', '-');
      case sf::Keyboard::Key::Equal:      return insertCharacter(event, '+', '=');
      case sf::Keyboard::Key::Q:          return insertCharacter(event, 'Q', 'q');
      case sf::Keyboard::Key::W:          return insertCharacter(event, 'W', 'w');
      case sf::Keyboard::Key::E:          return insertCharacter(event, 'E', 'e');
      case sf::Keyboard::Key::R:          return insertCharacter(event, 'R', 'r');
      case sf::Keyboard::Key::T:          return insertCharacter(event, 'T', 't');
      case sf::Keyboard::Key::Y:          return insertCharacter(event, 'Y', 'y');
      case sf::Keyboard::Key::U:          return insertCharacter(event, 'U', 'u');
      case sf::Keyboard::Key::I:          return insertCharacter(event, 'I', 'i');
      case sf::Keyboard::Key::O:          return insertCharacter(event, 'O', 'o');
      case sf::Keyboard::Key::P:          return insertCharacter(event, 'P', 'p');
      case sf::Keyboard::Key::LBracket:   return insertCharacter(event, '{', '[');
      case sf::Keyboard::Key::RBracket:   return insertCharacter(event, '}', ']');
      case sf::Keyboard::Key::BackSlash:  return insertCharacter(event, '|', '\\');
      case sf::Keyboard::Key::A:          return insertCharacter(event, 'A', 'a');
      case sf::Keyboard::Key::S:          return insertCharacter(event, 'S', 's');
      case sf::Keyboard::Key::D:          return insertCharacter(event, 'D', 'd');
      case sf::Keyboard::Key::F:          return insertCharacter(event, 'F', 'f');
      case sf::Keyboard::Key::G:          return insertCharacter(event, 'G', 'g');
      case sf::Keyboard::Key::H:          return insertCharacter(event, 'H', 'h');
      case sf::Keyboard::Key::J:          return insertCharacter(event, 'J', 'j');
      case sf::Keyboard::Key::K:          return insertCharacter(event, 'K', 'k');
      case sf::Keyboard::Key::L:          return insertCharacter(event, 'L', 'l');
      case sf::Keyboard::Key::SemiColon:  return insertCharacter(event, ':', ';');
      case sf::Keyboard::Key::Quote:      return insertCharacter(event, '"', '\'');
      case sf::Keyboard::Key::Z:          return insertCharacter(event, 'Z', 'z');
      case sf::Keyboard::Key::X:          return insertCharacter(event, 'X', 'x');
      case sf::Keyboard::Key::C:          return insertCharacter(event, 'C', 'c');
      case sf::Keyboard::Key::V:          return insertCharacter(event, 'V', 'v');
      case sf::Keyboard::Key::B:          return insertCharacter(event, 'B', 'b');
      case sf::Keyboard::Key::N:          return insertCharacter(event, 'N', 'n');
      case sf::Keyboard::Key::M:          return insertCharacter(event, 'M', 'm');
      case sf::Keyboard::Key::Comma:      return insertCharacter(event, '<', ',');
      case sf::Keyboard::Key::Period:     return insertCharacter(event, '>', '.');
      case sf::Keyboard::Key::Slash:      return insertCharacter(event, '?', '/');
      case sf::Keyboard::Key::Space:      return insertCharacter(event, ' ', ' ');
      case sf::Keyboard::Key::Left:
        left_cursor();
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::Right:
        right_cursor();
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::BackSpace:
        if (left_cursor())
        {
          del_character();
        }
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::Insert:
        m_replacing = !(m_replacing);
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::Delete:
        del_character();
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::Home:
        m_cursorPosition = 0;
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::End:
        m_cursorPosition = m_buffer.length();
        result = metagui::GUIEvent::Result::Handled;
        break;
      case sf::Keyboard::Key::Divide:     return insertCharacter(event, '/', '/');
      case sf::Keyboard::Key::Multiply:   return insertCharacter(event, '*', '*');
      case sf::Keyboard::Key::Subtract:   return insertCharacter(event, '-', '-');
      case sf::Keyboard::Key::Add:        return insertCharacter(event, '+', '+');
      case sf::Keyboard::Key::Numpad0:    return insertCharacter(event, '0', '0');
      case sf::Keyboard::Key::Numpad1:    return insertCharacter(event, '1', '1');
      case sf::Keyboard::Key::Numpad2:    return insertCharacter(event, '2', '2');
      case sf::Keyboard::Key::Numpad3:    return insertCharacter(event, '3', '3');
      case sf::Keyboard::Key::Numpad4:    return insertCharacter(event, '4', '4');
      case sf::Keyboard::Key::Numpad5:    return insertCharacter(event, '5', '5');
      case sf::Keyboard::Key::Numpad6:    return insertCharacter(event, '6', '6');
      case sf::Keyboard::Key::Numpad7:    return insertCharacter(event, '7', '7');
      case sf::Keyboard::Key::Numpad8:    return insertCharacter(event, '8', '8');
      case sf::Keyboard::Key::Numpad9:    return insertCharacter(event, '9', '9');
      case sf::Keyboard::Key::Return:
        m_enter = true;
        result = metagui::GUIEvent::Result::Handled;
        break;
      default: break;
    }
  }

  return result;
}

size_t KeyBuffer::get_cursor_position() const
{
  return m_cursorPosition;
}

void KeyBuffer::set_cursor_position(size_t position)
{
  m_cursorPosition = std::min(m_buffer.length(), position);
  broadcast(EventCursorMoved(position));
}

std::string const& KeyBuffer::get_buffer() const
{
  return m_buffer;
}

void KeyBuffer::set_buffer(std::string buf)
{
  m_buffer = buf;
  broadcast(EventBufferChanged(m_buffer));
  set_cursor_position(buf.length());
}

void KeyBuffer::clear_buffer()
{
  m_buffer.clear();
  broadcast(EventBufferChanged(m_buffer));
  m_cursorPosition = 0;
  broadcast(EventCursorMoved(m_cursorPosition));
  m_enter = false;
}

bool KeyBuffer::get_enter()
{
  return m_enter;
}

/// @todo Move this into a view.
void KeyBuffer::render(sf::RenderTexture& texture,
                       RealVec2 coords,
                       unsigned int frame,
                       sf::Font const& font,
                       unsigned int font_size,
                       sf::Color const& fg_color)
{
  sf::Text render_text;
  auto x_position = coords.x;

  render_text.setFont(font);
  render_text.setCharacterSize(font_size);
  render_text.setColor(fg_color);

  // *** PROMPT ***************************************************************
  render_text.setString("> ");
  render_text.setPosition({ x_position, coords.y });
  render_text.setStyle(sf::Text::Style::Bold);
  texture.draw(render_text);

  x_position += render_text.getLocalBounds().width;

  // *** RENDER TEXT **********************************************************
  render_text.setString(m_buffer);
  render_text.setPosition({ x_position, coords.y });
  render_text.setStyle(sf::Text::Style::Regular);
  texture.draw(render_text);

  // *** CURSOR ***************************************************************
  RealVec2 cursor_coords;
  RealVec2 cursor_size;
  sf::Color cursor_color = fg_color;

  if (m_replacing)
  {
    cursor_color.r >>= 1;
    cursor_color.g >>= 1;
    cursor_color.b >>= 1;
    cursor_color.a >>= 1;
  }

  // Nice flashy cursor
  cursor_color.a *= static_cast<sf::Uint8>((21 - (frame % 21)) * 0.05f);

  // This little hack ensures that trailing spaces in the string are considered
  // when figuring out the cursor location.
  render_text.setStyle(sf::Text::Style::Underlined);

  float font_height = font.getLineSpacing(font_size);

  cursor_coords = render_text.findCharacterPos(m_cursorPosition);

  if (m_replacing)
  {
    sf::Glyph glyph = font.getGlyph(m_buffer[m_cursorPosition],
                                    font_size, false);
    cursor_size = RealVec2(glyph.bounds.width, font_height);
  }
  else
  {
    cursor_size = RealVec2(2, font_height);
  }

  sf::RectangleShape cursor_rect;
  cursor_rect.setPosition(cursor_coords);
  cursor_rect.setSize(cursor_size);
  cursor_rect.setFillColor(cursor_color);
  texture.draw(cursor_rect);

  texture.display();
}

metagui::GUIEvent::Result KeyBuffer::insertCharacter(metagui::GUIEventKeyPressed& event, char shifted, char unshifted)
{                                                                  
  set_character((event.shift ? shifted : unshifted));                     
  return metagui::GUIEvent::Result::Handled;
}


/// Set character at cursor location, or insert/replace if string is not long enough.
void KeyBuffer::set_character(char const c)
{
  auto position = m_cursorPosition;

  if (position >= m_buffer.length())
  {
    m_buffer += c;
  }
  else
  {
    if (m_replacing)
    {
      m_buffer[position] = c;
    }
    else
    {
      m_buffer.insert(m_buffer.begin() + position, c);
    }
  }
  m_cursorPosition = position + 1;

  broadcast(EventBufferChanged(m_buffer));
  broadcast(EventCursorMoved(m_cursorPosition));
}

/// Delete character at cursor location.
void KeyBuffer::del_character()
{
  auto position = m_cursorPosition;

  if (position >= m_buffer.length())
  {
    return;
  }

  m_buffer.erase(position, 1);
  m_cursorPosition = position;

  broadcast(EventBufferChanged(m_buffer));
  broadcast(EventCursorMoved(m_cursorPosition));
}

/// Move cursor left.
bool KeyBuffer::left_cursor()
{
  if (m_cursorPosition == 0)
  {
    return false;
  }
  --m_cursorPosition;
  broadcast(EventCursorMoved(m_cursorPosition));

  return true;
}

/// Move cursor right.
bool KeyBuffer::right_cursor()
{
  if (m_cursorPosition == m_buffer.length())
  {
    return false;
  }
  ++m_cursorPosition;
  broadcast(EventCursorMoved(m_cursorPosition));

  return true;
}
