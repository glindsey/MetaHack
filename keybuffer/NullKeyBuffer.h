#pragma once

#include "stdafx.h"

#include "keybuffer/IKeyBuffer.h"

/// Implementation of a null key buffer.
class NullKeyBuffer : public IKeyBuffer
{
public:
  virtual ~NullKeyBuffer() {}

  virtual bool handle_key_press(UIEvents::EventKeyPressed const& event) override
  {
    return false;
  }

  virtual size_t get_cursor_position() const override
  {
    return 0;
  }

  virtual void set_cursor_position(size_t position) override {}

  virtual std::string const& get_buffer() const override { return null_buffer; }

  virtual void set_buffer(std::string buf) override {}

  virtual void clear_buffer() override {}

  virtual bool get_enter() override
  { 
    return false; 
  }

  virtual void render(sf::RenderTexture& texture,
                      RealVec2 coords,
                      unsigned int frame,
                      sf::Font const& font,
                      unsigned int font_size,
                      Color const& fg_color) override
  {}

private:
  std::string null_buffer;
};