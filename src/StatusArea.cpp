#include "StatusArea.h"

#include "App.h"
#include "ConfigSettings.h"
#include "Entity.h"
#include "ThingFactory.h"

struct StatusArea::Impl
{
  bool focus;
  unsigned int font_size;
  sf::IntRect dims;
  sf::RectangleShape area_bg_shape;
};

StatusArea::StatusArea(sf::IntRect dimensions)
  : impl(new Impl())
{
  impl->focus = false;
  impl->font_size = 14;
  impl->dims = dimensions;
}

StatusArea::~StatusArea()
{
  //dtor
}

void StatusArea::set_focus(bool focus)
{
  impl->focus = focus;
}

bool StatusArea::get_focus()
{
  return impl->focus;
}

sf::IntRect StatusArea::get_dimensions()
{
  return impl->dims;
}

void StatusArea::set_dimensions(sf::IntRect rect)
{
  impl->dims = rect;
}

EventResult StatusArea::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

bool StatusArea::render(sf::RenderTarget& target, int frame)
{
  // Draw the rectangle.
  impl->area_bg_shape.setPosition(sf::Vector2f(impl->dims.left, impl->dims.top));
  impl->area_bg_shape.setSize(sf::Vector2f(impl->dims.width, impl->dims.height));
  impl->area_bg_shape.setFillColor(Settings.window_bg_color);
  impl->area_bg_shape.setOutlineColor(impl->focus ?
                                      Settings.window_focused_border_color :
                                      Settings.window_border_color);
  impl->area_bg_shape.setOutlineThickness(Settings.window_border_width);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->area_bg_shape);

  int lineSpacing = the_default_font.getLineSpacing(impl->font_size);
  // Text offsets relative to the background rectangle.
  float xTextOffset = 3;
  float yTextOffset = 3;
  sf::Text renderText;
  renderText.setFont(the_default_font);
  renderText.setCharacterSize(lineSpacing);
  renderText.setColor(Settings.text_color);
  renderText.setPosition(impl->dims.left + xTextOffset,
                         impl->dims.top + yTextOffset);

  std::string name = TF.get_player().get_proper_name();
  name[0] = std::toupper(name[0]);

  std::string type = TF.get_player().get_description();
  type[0] = std::toupper(type[0]);

  renderText.setString(name + " the " + type);
  target.draw(renderText);

  return true;
}
