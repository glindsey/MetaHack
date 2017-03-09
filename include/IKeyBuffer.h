#pragma once

#include "stdafx.h"

#include "GUIEvent.h"
#include "Subject.h"

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

  virtual ~IKeyBuffer() {}

  /// Process a key event.
  virtual metagui::Event::Result handle_key_press(metagui::EventKeyPressed& event) = 0;

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
                      Vec2f coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      sf::Color const& fg_color) = 0;

  virtual std::unordered_set<EventID> registeredEvents() const override final;

};