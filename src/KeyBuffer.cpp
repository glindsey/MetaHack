#include "stdafx.h"

#include "KeyBuffer.h"

struct KeyBuffer::Impl
{
  /// Buffer holding the string being composed.
  StringDisplay buffer;

  /// Cursor location within string.
  unsigned int cursor_position;

  /// If true, we are replacing.  If false, we are inserting.
  bool replacing;

  /// If true, command is ready to be processed.
  bool enter;

  /// Set character at cursor location, or insert/replace if string is not long enough.
  void set_character(wchar_t const c)
  {
    unsigned int position = cursor_position;

    if (position >= buffer.length())
    {
      buffer += c;
    }
    else
    {
      if (replacing)
      {
        buffer[position] = c;
      }
      else
      {
        buffer.insert(buffer.begin() + position, c);
      }
    }
    cursor_position = position + 1;
  }

  /// Delete character at cursor location.
  void del_character()
  {
    unsigned int position = cursor_position;

    if (position >= buffer.length())
    {
      return;
    }

    buffer.erase(position, 1);
    cursor_position = position;
  }

  /// Move cursor left.
  bool left_cursor()
  {
    if (cursor_position == 0)
    {
      return false;
    }
    --cursor_position;
    return true;
  }

  /// Move cursor right.
  bool right_cursor()
  {
    if (cursor_position == buffer.length())
    {
      return false;
    }
    ++cursor_position;
    return true;
  }
};

KeyBuffer::KeyBuffer() :
  pImpl(NEW Impl())
{
  this->clear_buffer();
  pImpl->replacing = false;
}

KeyBuffer::~KeyBuffer()
{
  //dtor
}

#define INSERT(x, y)                                               \
{                                                                  \
  pImpl->set_character((event.shift ? x : y));                     \
  result = metagui::Event::Result::Handled;                        \
  break;                                                           \
}

metagui::Event::Result KeyBuffer::handle_key_press(metagui::EventKeyPressed& event)
{
  metagui::Event::Result result = metagui::Event::Result::Acknowledged;

  if (!event.control && !event.alt)
  {
    switch (event.code)
    {
      case sf::Keyboard::Key::Tilde:      INSERT(L'~', L'`');
      case sf::Keyboard::Key::Num1:       INSERT(L'!', L'1');
      case sf::Keyboard::Key::Num2:       INSERT(L'@', L'2');
      case sf::Keyboard::Key::Num3:       INSERT(L'#', L'3');
      case sf::Keyboard::Key::Num4:       INSERT(L'$', L'4');
      case sf::Keyboard::Key::Num5:       INSERT(L'%', L'5');
      case sf::Keyboard::Key::Num6:       INSERT(L'^', L'6');
      case sf::Keyboard::Key::Num7:       INSERT(L'&', L'7');
      case sf::Keyboard::Key::Num8:       INSERT(L'*', L'8');
      case sf::Keyboard::Key::Num9:       INSERT(L'(', L'9');
      case sf::Keyboard::Key::Num0:       INSERT(L')', L'0');
      case sf::Keyboard::Key::Dash:       INSERT(L'_', L'-');
      case sf::Keyboard::Key::Equal:      INSERT(L'+', L'=');
      case sf::Keyboard::Key::Q:          INSERT(L'Q', L'q');
      case sf::Keyboard::Key::W:          INSERT(L'W', L'w');
      case sf::Keyboard::Key::E:          INSERT(L'E', L'e');
      case sf::Keyboard::Key::R:          INSERT(L'R', L'r');
      case sf::Keyboard::Key::T:          INSERT(L'T', L't');
      case sf::Keyboard::Key::Y:          INSERT(L'Y', L'y');
      case sf::Keyboard::Key::U:          INSERT(L'U', L'u');
      case sf::Keyboard::Key::I:          INSERT(L'I', L'i');
      case sf::Keyboard::Key::O:          INSERT(L'O', L'o');
      case sf::Keyboard::Key::P:          INSERT(L'P', L'p');
      case sf::Keyboard::Key::LBracket:   INSERT(L'{', L'[');
      case sf::Keyboard::Key::RBracket:   INSERT(L'}', L']');
      case sf::Keyboard::Key::BackSlash:  INSERT(L'|', L'\\');
      case sf::Keyboard::Key::A:          INSERT(L'A', L'a');
      case sf::Keyboard::Key::S:          INSERT(L'S', L's');
      case sf::Keyboard::Key::D:          INSERT(L'D', L'd');
      case sf::Keyboard::Key::F:          INSERT(L'F', L'f');
      case sf::Keyboard::Key::G:          INSERT(L'G', L'g');
      case sf::Keyboard::Key::H:          INSERT(L'H', L'h');
      case sf::Keyboard::Key::J:          INSERT(L'J', L'j');
      case sf::Keyboard::Key::K:          INSERT(L'K', L'k');
      case sf::Keyboard::Key::L:          INSERT(L'L', L'l');
      case sf::Keyboard::Key::SemiColon:  INSERT(L':', L';');
      case sf::Keyboard::Key::Quote:      INSERT(L'"', L'\'');
      case sf::Keyboard::Key::Z:          INSERT(L'Z', L'z');
      case sf::Keyboard::Key::X:          INSERT(L'X', L'x');
      case sf::Keyboard::Key::C:          INSERT(L'C', L'c');
      case sf::Keyboard::Key::V:          INSERT(L'V', L'v');
      case sf::Keyboard::Key::B:          INSERT(L'B', L'b');
      case sf::Keyboard::Key::N:          INSERT(L'N', L'n');
      case sf::Keyboard::Key::M:          INSERT(L'M', L'm');
      case sf::Keyboard::Key::Comma:      INSERT(L'<', L',');
      case sf::Keyboard::Key::Period:     INSERT(L'>', L'.');
      case sf::Keyboard::Key::Slash:      INSERT(L'?', L'/');
      case sf::Keyboard::Key::Space:      INSERT(L' ', L' ');
      case sf::Keyboard::Key::Left:
        pImpl->left_cursor();
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Right:
        pImpl->right_cursor();
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::BackSpace:
        if (pImpl->left_cursor())
        {
          pImpl->del_character();
        }
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Insert:
        pImpl->replacing = !(pImpl->replacing);
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Delete:
        pImpl->del_character();
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Home:
        pImpl->cursor_position = 0;
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::End:
        pImpl->cursor_position = pImpl->buffer.length();
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Divide:     INSERT(L'/', L'/');
      case sf::Keyboard::Key::Multiply:   INSERT(L'*', L'*');
      case sf::Keyboard::Key::Subtract:   INSERT(L'-', L'-');
      case sf::Keyboard::Key::Add:        INSERT(L'+', L'+');
      case sf::Keyboard::Key::Numpad0:    INSERT(L'0', L'0');
      case sf::Keyboard::Key::Numpad1:    INSERT(L'1', L'1');
      case sf::Keyboard::Key::Numpad2:    INSERT(L'2', L'2');
      case sf::Keyboard::Key::Numpad3:    INSERT(L'3', L'3');
      case sf::Keyboard::Key::Numpad4:    INSERT(L'4', L'4');
      case sf::Keyboard::Key::Numpad5:    INSERT(L'5', L'5');
      case sf::Keyboard::Key::Numpad6:    INSERT(L'6', L'6');
      case sf::Keyboard::Key::Numpad7:    INSERT(L'7', L'7');
      case sf::Keyboard::Key::Numpad8:    INSERT(L'8', L'8');
      case sf::Keyboard::Key::Numpad9:    INSERT(L'9', L'9');
      case sf::Keyboard::Key::Return:
        pImpl->enter = true;
        result = metagui::Event::Result::Handled;
        break;
      default: break;
    }
  }

  return result;
}

unsigned int KeyBuffer::get_cursor_position() const
{
  return pImpl->cursor_position;
}

void KeyBuffer::set_cursor_position(unsigned int position)
{
  pImpl->cursor_position = std::min(pImpl->buffer.length(), position);
}

StringDisplay const& KeyBuffer::get_buffer() const
{
  return pImpl->buffer;
}

void KeyBuffer::set_buffer(StringDisplay buf)
{
  pImpl->buffer = buf;
  pImpl->cursor_position = buf.length();
}

void KeyBuffer::clear_buffer()
{
  pImpl->buffer.clear();
  pImpl->cursor_position = 0;
  pImpl->enter = false;
}

bool KeyBuffer::get_enter()
{
  return pImpl->enter;
}

void KeyBuffer::render(sf::RenderTexture& texture,
                       Vec2f coords,
                       unsigned int frame,
                       sf::Font const& font,
                       unsigned int font_size,
                       sf::Color const& fg_color)
{
  sf::Text render_text;
  float x_position = coords.x;

  render_text.setFont(font);
  render_text.setCharacterSize(font_size);
  render_text.setColor(fg_color);

  // *** PROMPT ***************************************************************
  render_text.setString(L"> ");
  render_text.setPosition(x_position, coords.y);
  render_text.setStyle(sf::Text::Style::Bold);
  texture.draw(render_text);

  x_position += render_text.getLocalBounds().width;

  // *** RENDER TEXT **********************************************************
  render_text.setString(pImpl->buffer);
  render_text.setPosition(x_position, coords.y);
  render_text.setStyle(sf::Text::Style::Regular);
  texture.draw(render_text);

  // *** CURSOR ***************************************************************
  Vec2f cursor_coords;
  Vec2f cursor_size;
  sf::Color cursor_color = fg_color;

  if (pImpl->replacing)
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

  cursor_coords = render_text.findCharacterPos(pImpl->cursor_position);

  if (pImpl->replacing)
  {
    sf::Glyph glyph = font.getGlyph(pImpl->buffer[pImpl->cursor_position],
                                    font_size, false);
    cursor_size = Vec2f(glyph.bounds.width, font_height);
  }
  else
  {
    cursor_size = Vec2f(2, font_height);
  }

  sf::RectangleShape cursor_rect;
  cursor_rect.setPosition(cursor_coords);
  cursor_rect.setSize(cursor_size);
  cursor_rect.setFillColor(cursor_color);
  texture.draw(cursor_rect);

  texture.display();
}