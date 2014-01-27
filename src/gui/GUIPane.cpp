#include "gui/GUIPane.h"

#include "App.h"
#include "ConfigSettings.h"

struct GUIPane::Impl
{
  Impl()
    : focus(false) {}

  /// Boolean indicating whether this area has the focus.
  bool focus;

  /// Pane dimensions.
  sf::IntRect dims;

  /// Background texture.
  std::unique_ptr<sf::RenderTexture> bg_texture;

  /// Background shape.
  sf::RectangleShape bg_shape;
};

GUIPane::GUIPane(sf::IntRect dimensions) :
  impl(new Impl())
{
  set_dimensions(dimensions);
}

GUIPane::~GUIPane()
{
  //dtor
}

void GUIPane::set_focus(bool focus)
{
  impl->focus = focus;
}

bool GUIPane::get_focus()
{
  return impl->focus;
}

sf::IntRect GUIPane::get_dimensions()
{
  return impl->dims;
}

void GUIPane::set_dimensions(sf::IntRect rect)
{
  impl->dims = rect;
  impl->bg_texture.reset(new sf::RenderTexture());
  impl->bg_texture->create(rect.width, rect.height);
}

EventResult GUIPane::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

bool GUIPane::render(sf::RenderTarget& target, int frame)
{
  int line_spacing_y = the_default_font.getLineSpacing(Settings.text_default_size);

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  impl->bg_texture->clear(Settings.window_bg_color);

  // Call the possibly-overridden render_contents.
  sf::String title_string = render_contents(frame);

  // Render to the texture.
  impl->bg_texture->display();

  /// @todo Set an icon for the pane, if one exists.

  // IF the pane has a title...
  if (!title_string.isEmpty())
  {
    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_text.setString(title_string);
    title_text.setFont(the_default_bold_font);
    title_text.setCharacterSize(Settings.text_default_size);

    title_rect.setFillColor(Settings.window_bg_color);
    title_rect.setOutlineColor(impl->focus ?
                              Settings.window_focused_border_color :
                              Settings.window_border_color);
    title_rect.setOutlineThickness(Settings.window_border_width);
    title_rect.setPosition(sf::Vector2f(0, 0));
    title_rect.setSize(sf::Vector2f(impl->dims.width,
                                    line_spacing_y + (text_offset_y * 2)));

    impl->bg_texture->draw(title_rect);

    title_text.setColor(Settings.text_color);
    title_text.setPosition(sf::Vector2f(text_offset_x + line_spacing_y,
                                        text_offset_y));
    impl->bg_texture->draw(title_text);

    // Render to the texture.
    impl->bg_texture->display();
  }

  // Draw the border.
  impl->bg_shape.setPosition(sf::Vector2f(impl->dims.left,
                                          impl->dims.top));
  impl->bg_shape.setSize(sf::Vector2f(impl->dims.width,
                                      impl->dims.height));
  impl->bg_shape.setTexture(&(impl->bg_texture->getTexture()));
  impl->bg_shape.setTextureRect(sf::IntRect(0, 0,
                                           impl->dims.width,
                                           impl->dims.height));
  impl->bg_shape.setOutlineColor(impl->focus ?
                                 Settings.window_focused_border_color :
                                 Settings.window_border_color);
  impl->bg_shape.setOutlineThickness(Settings.window_border_width);

  target.setView(sf::View(sf::FloatRect(0, 0,
                          target.getSize().x,
                          target.getSize().y)));

  target.draw(impl->bg_shape);

  return true;
}

std::string GUIPane::render_contents(int frame)
{
  return "Null Pane";
}

// === PROTECTED METHODS ======================================================
sf::RenderTexture& GUIPane::get_bg_texture()
{
  return *(impl->bg_texture.get());
}
