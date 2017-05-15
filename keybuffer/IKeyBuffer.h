#pragma once

#include "stdafx.h"

#include "gui/GUIObject.h"
#include "Subject.h"

// Forward declarations
class Color;

/// Interface for an input buffer.
class IKeyBuffer : public Subject
{
public:
  struct EventBufferChanged : public ConcreteEvent<EventBufferChanged>
  {
    EventBufferChanged(std::string buffer_)
      :
      buffer{ buffer_ }
    {}

    std::string const buffer;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | buffer: \"" << buffer << "\"";
    }
  };

  struct EventCursorMoved : public ConcreteEvent<EventCursorMoved>
  {
    EventCursorMoved(size_t position_)
      :
      position{ position_ }
    {}

    size_t const position;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | position: \"" << position << "\"";
    }
  };

  IKeyBuffer() : Subject({
    EventBufferChanged::id,
    EventCursorMoved::id })
  {}

  virtual ~IKeyBuffer() {}

  /// Process a key event.
  virtual bool handle_key_press(UIEvents::EventKeyPressed const& event) = 0;

  /// Get the current cursor position.
  virtual size_t get_cursor_position() const = 0;

  /// Set the current cursor position.
  virtual void set_cursor_position(size_t position) = 0;

  /// Get the current buffer contents.
  virtual std::string const& get_buffer() const = 0;

  /// Set the current buffer contents, putting cursor at the end.
  virtual void set_buffer(std::string buf) = 0;

  /// Clear the current buffer contents.
  virtual void clear_buffer() = 0;

  /// Get whether the buffer is ready for processing.
  virtual bool get_enter() = 0;

  /// Render onto a target.
  /// @param target Target to render onto.
  /// @param coords Coordinates to render at.
  /// @param frame Current frame (used for cursor flash).
  /// @param font Font to use.
  /// @param font_size Font size to use.
  /// @param fg_color Foreground color to use.
  /// @todo Split this out into a view.
  virtual void render(sf::RenderTexture& texture,
                      RealVec2 coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      Color const& fg_color) = 0;
  
};