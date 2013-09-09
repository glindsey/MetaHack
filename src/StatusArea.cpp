#include "StatusArea.h"

#include <boost/lexical_cast.hpp>

#include "App.h"
#include "ConfigSettings.h"
#include "Entity.h"
#include "ThingFactory.h"

struct StatusArea::Impl
{
  bool focus;
  sf::IntRect dims;
  sf::RectangleShape area_bg_shape;
};

StatusArea::StatusArea(sf::IntRect dimensions)
  : impl(new Impl())
{
  impl->focus = false;
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
  Entity& player = TF.get_player();

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

  int line_spacing = the_default_font.getLineSpacing(Settings.text_default_size);

  // Text offsets relative to the background rectangle.
  sf::Text render_text;
  render_text.setFont(the_default_font);
  render_text.setCharacterSize(Settings.text_default_size);
  render_text.setColor(Settings.text_color);
  render_text.setPosition(impl->dims.left + 3, impl->dims.top + 3);

  std::string name = player.get_proper_name();
  name[0] = std::toupper(name[0]);

  std::string type = player.get_description();
  type[0] = std::toupper(type[0]);

  render_text.setString(name + " the " + type);
  target.draw(render_text);

  render_text.setFont(the_default_mono_font);
  render_text.setPosition(impl->dims.left + 3, impl->dims.top + 23);
  render_text.setString("HP");
  target.draw(render_text);

  int hp = player.get_attributes().get(Attribute::HP);
  int max_hp = player.get_attributes().get(Attribute::MaxHP);

  float hp_percentage = static_cast<float>(hp) / static_cast<float>(max_hp);

  if (hp_percentage > 0.6)
  {
    render_text.setColor(Settings.text_color);
  }
  else if (hp_percentage > 0.3)
  {
    render_text.setColor(Settings.text_warning_color);
  }
  else
  {
    render_text.setColor(Settings.text_danger_color);
  }

  std::string hp_string = boost::lexical_cast<std::string>(hp) +
                    "/" + boost::lexical_cast<std::string>(max_hp);

  render_text.setPosition(impl->dims.left + 33, impl->dims.top + 23);
  render_text.setString(hp_string);
  target.draw(render_text);

  return true;
}
