#include "stdafx.h"

#include "gui/GUIDesktopPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

GUIDesktopPane::GUIDesktopPane(std::string name, sf::Vector2i location, sf::Vector2i size)
  :
  GUIPane(name, location, size)
{}

GUIDesktopPane::GUIDesktopPane(std::string name, sf::IntRect dimensions)
  :
  GUIPane(name, dimensions)
{}

GUIDesktopPane::~GUIDesktopPane()
{}

// === PROTECTED METHODS ======================================================

void GUIDesktopPane::render_self_before_children_(sf::RenderTexture& texture, int frame)
{
  sf::Vector2i size = get_size();

  float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

  // Text offsets relative to the background rectangle.
  float text_offset_x = 3;
  float text_offset_y = 3;

  // Clear the target.
  texture.clear(Settings.get<sf::Color>("window_bg_color"));

  // IF the pane has a caption...
  if (!get_text().empty())
  {
    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_text.setString(get_text());
    title_text.setFont(the_default_bold_font);
    title_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));

    title_rect.setFillColor(Settings.get<sf::Color>("window_bg_color"));
    title_rect.setOutlineColor(get_focus() ?
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

}