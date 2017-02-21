#pragma once

#include "stdafx.h"

#include "GUIEvent.h"
#include "IKeyBuffer.h"

/// Implementation of a null key buffer.
class NullKeyBuffer : public IKeyBuffer
{
public:
  virtual ~NullKeyBuffer() {}

  /// Process a key event.
  virtual metagui::Event::Result handle_key_press(metagui::EventKeyPressed& event)
  {
    return metagui::Event::Result::Ignored;
  }

  /// Get the current cursor position.
  virtual size_t get_cursor_position() const
  {
    return 0;
  }

  /// Set the current cursor position.
  virtual void set_cursor_position(size_t position) {}

  /// Get the current buffer contents.
  virtual std::string const& get_buffer() const { return null_buffer; }

  /// Set the current buffer contents, putting cursor at the end.
  virtual void set_buffer(std::string buf) {}

  /// Clear the current buffer contents.
  virtual void clear_buffer() {}

  /// Get whether the buffer is ready for processing.
  virtual bool get_enter() 
  { 
    return false; 
  }

  /// Render onto a target.
  /// @param target Target to render onto.
  /// @param coords Coordinates to render at.
  /// @param frame Current frame (used for cursor flash).
  /// @param font Font to use.
  /// @param font_size Font size to use.
  /// @param fg_color Foreground color to use.
  /// @todo Split this out into a view.
  virtual void render(sf::RenderTexture& texture,
                      Vec2f coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      sf::Color const& fg_color)
  {}

private:
  std::string null_buffer;
};