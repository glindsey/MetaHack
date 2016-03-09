#include "stdafx.h"

#include "KeyBuffer.h"

#include "New.h"

struct KeyBuffer::Impl
{
  /// Buffer holding the string being composed.
  std::string buffer;

  /// Cursor location within string.
  unsigned int cursor_position;

  /// If true, we are replacing.  If false, we are inserting.
  bool replacing;

  /// If true, command is ready to be processed.
  bool enter;

  /// Set character at cursor location, or insert/replace if string is not long enough.
  void set_character(char const c)
  {
    unsigned int position = cursor_position;

    if (position >= buffer.size())
    {
      buffer.append(1, c);
    }
    else
    {
      if (replacing)
      {
        buffer.replace(position, 1, 1, c);
      }
      else
      {
        buffer.insert(position, 1, c);
      }
    }
    cursor_position = position + 1;
  }

  /// Delete character at cursor location.
  void del_character()
  {
    unsigned int position = cursor_position;

    if (position >= buffer.size())
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
    if (cursor_position == buffer.size())
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
  metagui::Event::Result result = metagui::Event::Result::Ignored;

  if (!event.control && !event.alt)
  {
    switch (event.code)
    {
      case sf::Keyboard::Key::Tilde:      INSERT('~', '`');
      case sf::Keyboard::Key::Num1:       INSERT('!', '1');
      case sf::Keyboard::Key::Num2:       INSERT('@', '2');
      case sf::Keyboard::Key::Num3:       INSERT('#', '3');
      case sf::Keyboard::Key::Num4:       INSERT('$', '4');
      case sf::Keyboard::Key::Num5:       INSERT('%', '5');
      case sf::Keyboard::Key::Num6:       INSERT('^', '6');
      case sf::Keyboard::Key::Num7:       INSERT('&', '7');
      case sf::Keyboard::Key::Num8:       INSERT('*', '8');
      case sf::Keyboard::Key::Num9:       INSERT('(', '9');
      case sf::Keyboard::Key::Num0:       INSERT(')', '0');
      case sf::Keyboard::Key::Dash:       INSERT('_', '-');
      case sf::Keyboard::Key::Equal:      INSERT('+', '=');
      case sf::Keyboard::Key::Q:          INSERT('Q', 'q');
      case sf::Keyboard::Key::W:          INSERT('W', 'w');
      case sf::Keyboard::Key::E:          INSERT('E', 'e');
      case sf::Keyboard::Key::R:          INSERT('R', 'r');
      case sf::Keyboard::Key::T:          INSERT('T', 't');
      case sf::Keyboard::Key::Y:          INSERT('Y', 'y');
      case sf::Keyboard::Key::U:          INSERT('U', 'u');
      case sf::Keyboard::Key::I:          INSERT('I', 'i');
      case sf::Keyboard::Key::O:          INSERT('O', 'o');
      case sf::Keyboard::Key::P:          INSERT('P', 'p');
      case sf::Keyboard::Key::LBracket:   INSERT('{', '[');
      case sf::Keyboard::Key::RBracket:   INSERT('}', ']');
      case sf::Keyboard::Key::BackSlash:  INSERT('|', '\\');
      case sf::Keyboard::Key::A:          INSERT('A', 'a');
      case sf::Keyboard::Key::S:          INSERT('S', 's');
      case sf::Keyboard::Key::D:          INSERT('D', 'd');
      case sf::Keyboard::Key::F:          INSERT('F', 'f');
      case sf::Keyboard::Key::G:          INSERT('G', 'g');
      case sf::Keyboard::Key::H:          INSERT('H', 'h');
      case sf::Keyboard::Key::J:          INSERT('J', 'j');
      case sf::Keyboard::Key::K:          INSERT('K', 'k');
      case sf::Keyboard::Key::L:          INSERT('L', 'l');
      case sf::Keyboard::Key::SemiColon:  INSERT(':', ';');
      case sf::Keyboard::Key::Quote:      INSERT('"', '\'');
      case sf::Keyboard::Key::Z:          INSERT('Z', 'z');
      case sf::Keyboard::Key::X:          INSERT('X', 'x');
      case sf::Keyboard::Key::C:          INSERT('C', 'c');
      case sf::Keyboard::Key::V:          INSERT('V', 'v');
      case sf::Keyboard::Key::B:          INSERT('B', 'b');
      case sf::Keyboard::Key::N:          INSERT('N', 'n');
      case sf::Keyboard::Key::M:          INSERT('M', 'm');
      case sf::Keyboard::Key::Comma:      INSERT('<', ',');
      case sf::Keyboard::Key::Period:     INSERT('>', '.');
      case sf::Keyboard::Key::Slash:      INSERT('?', '/');
      case sf::Keyboard::Key::Space:      INSERT(' ', ' ');
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
        pImpl->cursor_position = pImpl->buffer.size();
        result = metagui::Event::Result::Handled;
        break;
      case sf::Keyboard::Key::Divide:     INSERT('/', '/');
      case sf::Keyboard::Key::Multiply:   INSERT('*', '*');
      case sf::Keyboard::Key::Subtract:   INSERT('-', '-');
      case sf::Keyboard::Key::Add:        INSERT('+', '+');
      case sf::Keyboard::Key::Numpad0:    INSERT('0', '0');
      case sf::Keyboard::Key::Numpad1:    INSERT('1', '1');
      case sf::Keyboard::Key::Numpad2:    INSERT('2', '2');
      case sf::Keyboard::Key::Numpad3:    INSERT('3', '3');
      case sf::Keyboard::Key::Numpad4:    INSERT('4', '4');
      case sf::Keyboard::Key::Numpad5:    INSERT('5', '5');
      case sf::Keyboard::Key::Numpad6:    INSERT('6', '6');
      case sf::Keyboard::Key::Numpad7:    INSERT('7', '7');
      case sf::Keyboard::Key::Numpad8:    INSERT('8', '8');
      case sf::Keyboard::Key::Numpad9:    INSERT('9', '9');
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
  pImpl->cursor_position = std::min(pImpl->buffer.size(), position);
}

std::string const& KeyBuffer::get_buffer() const
{
  return pImpl->buffer;
}

void KeyBuffer::set_buffer(std::string buf)
{
  pImpl->buffer = buf;
  pImpl->cursor_position = buf.size();
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
                       sf::Vector2f coords,
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
  render_text.setString("> ");
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
  sf::Vector2f cursor_coords;
  sf::Vector2f cursor_size;
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
    cursor_size = sf::Vector2f(glyph.bounds.width, font_height);
  }
  else
  {
    cursor_size = sf::Vector2f(2, font_height);
  }

  sf::RectangleShape cursor_rect;
  cursor_rect.setPosition(cursor_coords);
  cursor_rect.setSize(cursor_size);
  cursor_rect.setFillColor(cursor_color);
  texture.draw(cursor_rect);

  texture.display();
}