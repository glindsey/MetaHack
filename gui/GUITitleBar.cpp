#include "stdafx.h"

#include "GUITitleBar.h"

#include "App.h"
#include "IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    Object(name, Vec2i(0, 0))
  {}

  TitleBar::~TitleBar()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void TitleBar::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = Service<IConfigSettings>::get();

    if (getParent() == nullptr)
    {
      return;
    }

    auto parent_size = getParent()->getSize();
    auto titlebar_text = getParent()->getText();

    float line_spacing_y = the_default_font.getLineSpacing(config.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_x = config.get<float>("window_text_offset_x");
    float text_offset_y = config.get<float>("window_text_offset_y");
    float border_width = config.get<float>("window_border_width");

    Vec2f rect_position{ border_width, border_width };
    Vec2f rect_size{ static_cast<float>(parent_size.x - (border_width * 2)),
      static_cast<float>(line_spacing_y + (text_offset_y * 2) - (border_width * 2)) };
    Vec2f text_position{ text_offset_x + line_spacing_y, text_offset_y };

    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_rect.setFillColor(config.get<sf::Color>("window_bg_color"));
    title_rect.setOutlineColor(getParent()->getFocus() ?
                               config.get<sf::Color>("window_focused_border_color") :
                               config.get<sf::Color>("window_border_color"));
    title_rect.setOutlineThickness(border_width);
    title_rect.setPosition(rect_position);
    title_rect.setSize(rect_size);

    texture.draw(title_rect);

    if (titlebar_text.empty() == false)
    {
      title_text.setString(titlebar_text);
      title_text.setFont(the_default_bold_font);
      title_text.setCharacterSize(config.get<unsigned int>("text_default_size"));

      title_text.setColor(config.get<sf::Color>("text_color"));
      title_text.setPosition(text_position);
      texture.draw(title_text);
    }

    texture.display();
  }

  void TitleBar::handleParentSizeChanged_(Vec2u parent_size)
  {
    auto& config = Service<IConfigSettings>::get();
    float line_spacing_y = the_default_font.getLineSpacing(config.get<unsigned int>("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_y = config.get<float>("window_text_offset_y");

    Vec2u our_size{ parent_size.x, static_cast<unsigned int>(line_spacing_y + (text_offset_y * 2)) };

    setSize(our_size);
  }
}; // end namespace metagui