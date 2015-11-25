#include "gui/GUILabel.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

struct GUILabel::Impl
{
  Impl(std::function<std::string()> string_function_)
    :
    focus{ false },
    bg_texture{ NEW sf::RenderTexture() },
    string_function{ string_function_ }
  {
    bg_texture->create(dims.width, dims.height);
  }

  /// Boolean indicating whether this area has the focus.
  bool focus;

  /// Pane dimensions.
  sf::IntRect dims;

  /// Background texture.
  std::unique_ptr<sf::RenderTexture> bg_texture;

  /// Function to get the text to render.
  std::function<std::string()> string_function;

  /// Background shape.
  sf::RectangleShape bg_shape;
};

GUILabel::GUILabel(sf::IntRect dimensions, std::function<std::string()> string_function)
  :
  GUIObject(dimensions),
  pImpl{ NEW Impl(string_function) }
{
  set_dimensions(dimensions);
}

GUILabel::~GUILabel()
{
  //dtor
}

std::function<std::string()> GUILabel::get_string_function()
{
  return pImpl->string_function;
}

void GUILabel::set_string_function(std::function<std::string()> string_function)
{
  pImpl->string_function = string_function;
}

EventResult GUILabel::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

// === PROTECTED METHODS ======================================================
bool GUILabel::_render_self(sf::RenderTarget& target, int frame)
{
  float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear background texture.
  pImpl->bg_texture->clear(Settings.get<sf::Color>("window_bg_color"));

  /// @todo Flesh this out; right now it is EXTREMELY rudimentary
  std::string str = pImpl->string_function();
  if (!str.empty())
  {
    sf::Text text{ str, the_default_font, Settings.get<unsigned int>("text_default_size") };
    text.setColor(Settings.get<sf::Color>("text_color"));
    pImpl->bg_texture->draw(text);
  }

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