#include "gui/GUIObject.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

struct GUIObject::Impl
{
  Impl()
    : 
    focus{ false }
  {}

  /// Boolean indicating whether this object has the focus.
  bool focus;

  /// Pane dimensions.
  sf::IntRect dims;

  /// Background texture.
  std::unique_ptr<sf::RenderTexture> bg_texture;

  /// Background shape.
  sf::RectangleShape bg_shape;
};

GUIObject::GUIObject(sf::IntRect dimensions)
  :
  pImpl(NEW Impl())
{
  set_dimensions(dimensions);
}

GUIObject::~GUIObject()
{
  //dtor
}

void GUIObject::set_focus(bool focus)
{
  pImpl->focus = focus;
}

bool GUIObject::get_focus()
{
  return pImpl->focus;
}

sf::IntRect GUIObject::get_dimensions()
{
  return pImpl->dims;
}

void GUIObject::set_dimensions(sf::IntRect rect)
{
  pImpl->dims = rect;
  pImpl->bg_texture.reset(NEW sf::RenderTexture());
  pImpl->bg_texture->create(rect.width, rect.height);
}

bool GUIObject::render(sf::RenderTarget& target, int frame)
{
  sf::RenderTexture& texture = *(pImpl->bg_texture.get());

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  pImpl->bg_texture->clear();

  /// @todo Render all child objects to our bg texture.

  /// Render self to our bg texture.
  bool success = _render_self(texture, frame);

  texture.display();

  // Create the RectangleShape that will be drawn onto the target.
  pImpl->bg_shape.setPosition(sf::Vector2f(static_cast<float>(pImpl->dims.left), static_cast<float>(pImpl->dims.top)));
  pImpl->bg_shape.setSize(sf::Vector2f(static_cast<float>(pImpl->dims.width), static_cast<float>(pImpl->dims.height)));
  pImpl->bg_shape.setTexture(&(pImpl->bg_texture->getTexture()));
  pImpl->bg_shape.setTextureRect(sf::IntRect(0, 0, pImpl->dims.width, pImpl->dims.height));

  // Draw onto the target.
  target.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));
  target.draw(pImpl->bg_shape);

  return true;
}