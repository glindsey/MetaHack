#include "stdafx.h"

#include "gui/GUITitleBar.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    Object(name, sf::Vector2i(0, 0))
  {}

  TitleBar::~TitleBar()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void TitleBar::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
    if (get_parent() == nullptr)
    {
      return;
    }

    auto parent_size = get_parent()->get_size();
    auto titlebar_text = get_parent()->get_text();

    float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_x = Settings.get<float>("window_text_offset_x");
    float text_offset_y = Settings.get<float>("window_text_offset_y");
    float border_width = Settings.get<float>("window_border_width");

    sf::Vector2f rect_position{ border_width, border_width };
    sf::Vector2f rect_size{ static_cast<float>(parent_size.x - (border_width * 2)), 
      static_cast<float>(line_spacing_y + (text_offset_y * 2) - (border_width * 2)) };
    sf::Vector2f text_position{ text_offset_x + line_spacing_y, text_offset_y };

    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_rect.setFillColor(Settings.get<sf::Color>("window_bg_color"));
    title_rect.setOutlineColor(get_focus() ?
                                Settings.get<sf::Color>("window_focused_border_color") :
                                Settings.get<sf::Color>("window_border_color"));
    title_rect.setOutlineThickness(border_width);
    title_rect.setPosition(rect_position);
    title_rect.setSize(rect_size);

    texture.draw(title_rect);

    if (titlebar_text.empty() == false)
    {
      title_text.setString(titlebar_text);
      title_text.setFont(the_default_bold_font);
      title_text.setCharacterSize(Settings.get<unsigned int>("text_default_size"));

      title_text.setColor(Settings.get<sf::Color>("text_color"));
      title_text.setPosition(text_position);
      texture.draw(title_text);
    }

    texture.display();
  }

  void TitleBar::handle_parent_size_changed_(sf::Vector2u parent_size)
  {
    float line_spacing_y = the_default_font.getLineSpacing(Settings.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_y = Settings.get<float>("window_text_offset_y");

    sf::Vector2u our_size{ parent_size.x, static_cast<unsigned int>(line_spacing_y + (text_offset_y * 2)) };

    set_size(our_size);
  }

}; // end namespace metagui