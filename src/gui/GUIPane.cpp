#include "gui/GUIPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

struct GUIPane::Impl
{
  Impl()
    :
    focus(false)
  {}

  /// Boolean indicating whether this area has the focus.
  bool focus;

  /// Border shape.
  sf::RectangleShape border_shape;
};

GUIPane::GUIPane(sf::IntRect dimensions) :
  GUIObject("pane", dimensions),
  pImpl{ NEW Impl() }
{}

GUIPane::~GUIPane()
{
  //dtor
}

EventResult GUIPane::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

// === PROTECTED METHODS ======================================================

bool GUIPane::_render_self(sf::RenderTexture& texture, int frame)
{
  sf::Vector2i size = get_size();

  float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear the target.
  texture.clear(Settings.get<sf::Color>("window_bg_color"));

  // Render the contents, if any.
  std::string title = _render_contents(texture, frame);

  // IF the pane has a title...
  if (!title.empty())
  {
    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_text.setString(title);
    title_text.setFont(the_default_bold_font);
    title_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));

    title_rect.setFillColor(Settings.get<sf::Color>("window_bg_color"));
    title_rect.setOutlineColor(pImpl->focus ?
                               Settings.get<sf::Color>("window_focused_border_color") :
                               Settings.get<sf::Color>("window_border_color"));
    title_rect.setOutlineThickness(Settings.get<float>("window_border_width"));
    title_rect.setPosition({ 0, 0 });
    title_rect.setSize(sf::Vector2f(static_cast<float>(size.x),
                                    static_cast<float>(line_spacing_y + (text_offset_y * 2))));

    texture.draw(title_rect);

    title_text.setColor(Settings.get<sf::Color>("text_color"));
    title_text.setPosition(sf::Vector2f(text_offset_x + line_spacing_y,
                                        text_offset_y));
    texture.draw(title_text);
  }

  // Draw the border.
  float border_width = Settings.get<float>("window_border_width");
  pImpl->border_shape.setPosition(sf::Vector2f(border_width, border_width));
  pImpl->border_shape.setSize(sf::Vector2f(static_cast<float>(size.x - (2 * border_width)), static_cast<float>(size.y - (2 * border_width))));
  pImpl->border_shape.setFillColor(sf::Color::Transparent);
  pImpl->border_shape.setOutlineColor(
    pImpl->focus ?
    Settings.get<sf::Color>("window_focused_border_color") :
    Settings.get<sf::Color>("window_border_color"));
  pImpl->border_shape.setOutlineThickness(border_width);

  //texture.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));

  texture.draw(pImpl->border_shape);

  return true;
}

std::string GUIPane::_render_contents(sf::RenderTexture& texture, int frame)
{
  return "";
}