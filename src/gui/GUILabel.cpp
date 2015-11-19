#include "gui/GUILabel.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

struct GUILabel::Impl
{
  Impl(std::weak_ptr<std::string> text_ptr_)
    : 
    focus{ false },
    text_ptr{ text_ptr_ }
  {}

  /// Boolean indicating whether this area has the focus.
  bool focus;

  /// Pane dimensions.
  sf::IntRect dims;

  /// Background texture.
  std::unique_ptr<sf::RenderTexture> bg_texture;

  /// Pointer to the text to render.
  std::weak_ptr<std::string> text_ptr;

  /// Background shape.
  sf::RectangleShape bg_shape;
};

GUILabel::GUILabel(sf::IntRect dimensions, std::weak_ptr<std::string> text_ptr)
  :
  pImpl(NEW Impl(text_ptr))
{
  set_dimensions(dimensions);
}

GUILabel::~GUILabel()
{
  //dtor
}

void GUILabel::set_focus(bool focus)
{
  pImpl->focus = focus;
}

bool GUILabel::get_focus()
{
  return pImpl->focus;
}

sf::IntRect GUILabel::get_dimensions()
{
  return pImpl->dims;
}

void GUILabel::set_dimensions(sf::IntRect rect)
{
  pImpl->dims = rect;
  pImpl->bg_texture.reset(NEW sf::RenderTexture());
  pImpl->bg_texture->create(rect.width, rect.height);
}

std::weak_ptr<std::string> const& GUILabel::get_text_pointer()
{
  return pImpl->text_ptr;
}

void GUILabel::set_text_pointer(std::shared_ptr<std::string> text_ptr)
{
  pImpl->text_ptr = text_ptr;
}

EventResult GUILabel::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

bool GUILabel::render(sf::RenderTarget& target, int frame)
{
  float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  pImpl->bg_texture->clear(Settings.get<sf::Color>("window_bg_color"));

  // Render to the texture.
  pImpl->bg_texture->display();

  // Create the RectangleShape that will be drawn onto the target.
  pImpl->bg_shape.setPosition(sf::Vector2f(static_cast<float>(pImpl->dims.left), static_cast<float>(pImpl->dims.top)));
  pImpl->bg_shape.setSize(sf::Vector2f(static_cast<float>(pImpl->dims.width), static_cast<float>(pImpl->dims.height)));
  pImpl->bg_shape.setTexture(&(pImpl->bg_texture->getTexture()));
  pImpl->bg_shape.setTextureRect(sf::IntRect(0, 0,
                                           pImpl->dims.width,
                                           pImpl->dims.height));

  // Draw onto the target.
  target.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));
  target.draw(pImpl->bg_shape);

  return true;
}

// === PROTECTED METHODS ======================================================
sf::RenderTexture& GUILabel::get_bg_texture()
{
  return *(pImpl->bg_texture.get());
}
