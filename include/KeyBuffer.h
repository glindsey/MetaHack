#ifndef KEYBUFFER_H
#define KEYBUFFER_H

#include "stdafx.h"

#include "IKeyBuffer.h"

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
                      Vec2f coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      sf::Color const& fg_color) override;

protected:
private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // KEYBUFFER_H
