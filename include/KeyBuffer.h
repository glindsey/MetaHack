#ifndef KEYBUFFER_H
#define KEYBUFFER_H

#include <SFML/Graphics.hpp>
#include <memory>

#include "EventHandler.h"

/// This class encapsulates the idea of an input buffer which is added to or
/// removed from using SFML KeyPressed events.
class KeyBuffer
{
  public:
    KeyBuffer();
    virtual ~KeyBuffer();

    /// Process a key event.
    EventResult handle_key_press(sf::Event::KeyEvent& key);

    /// Get the current cursor position.
    unsigned int get_cursor_position() const;

    /// Set the current cursor position.
    void set_cursor_position(unsigned int position);

    /// Get the current buffer contents.
    std::string const& get_buffer() const;

    /// Set the current buffer contents, putting cursor at the end.
    void set_buffer(std::string buf);

    /// Clear the current buffer contents.
    void clear_buffer();

    /// Render onto a target.
    /// @param target Target to render onto.
    /// @param coords Coordinates to render at.
    /// @param frame Current frame (used for cursor flash).
    /// @param font Font to use.
    /// @param font_size Font size to use.
    /// @param fg_color Foreground color to use.
    void render(sf::RenderTarget& target,
                sf::Vector2f coords,
                unsigned int frame,
                sf::Font const& font,
                unsigned int font_size,
                sf::Color const& fg_color);

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // KEYBUFFER_H
