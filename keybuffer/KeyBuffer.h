#ifndef KEYBUFFER_H
#define KEYBUFFER_H

#include "stdafx.h"

#include "keybuffer/IKeyBuffer.h"

/// This class encapsulates the idea of an input buffer which is added to or
/// removed from using SFML KeyPressed events.
class KeyBuffer : public IKeyBuffer
{
public:
  KeyBuffer();
  virtual ~KeyBuffer();

  virtual metagui::Event::Result KeyBuffer::handle_key_press(metagui::EventKeyPressed& event) override;
  virtual size_t get_cursor_position() const override;
  virtual void set_cursor_position(size_t position) override;
  virtual std::string const& get_buffer() const override;
  virtual void set_buffer(std::string buf) override;
  virtual void clear_buffer() override;
  virtual bool get_enter() override;
  virtual void render(sf::RenderTexture& texture,
                      RealVec2 coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      sf::Color const& fg_color) override;

protected:
  /// Helper function to insert a character depending on shift.
  metagui::Event::Result insertCharacter(metagui::EventKeyPressed& event, char shifted, char unshifted);

  /// Set character at cursor location, or insert/replace if string is not long enough.
  void set_character(char const c);

  /// Delete character at cursor location.
  void del_character();

  /// Move cursor left.
  bool left_cursor();

  /// Move cursor right.
  bool right_cursor();

private:
  /// Buffer holding the string being composed.
  std::string m_buffer;

  /// Cursor location within string.
  size_t m_cursorPosition;

  /// If true, we are replacing.  If false, we are inserting.
  bool m_replacing;

  /// If true, command is ready to be processed.
  bool m_enter;
};

#endif // KEYBUFFER_H
