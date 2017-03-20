#include "stdafx.h"

#include "GUITitleBar.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    Object(name, IntVec2(0, 0))
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

    float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size").as<uint32_t>());

    // Text offsets relative to the background rectangle.
    float text_offset_x = config.get("window_text_offset_x").as<float>();
    float text_offset_y = config.get("window_text_offset_y").as<float>();
    float border_width = config.get("window_border_width").as<float>();

    RealVec2 rect_position{ border_width, border_width };
    RealVec2 rect_size{ static_cast<float>(parent_size.x - (border_width * 2)),
      static_cast<float>(line_spacing_y + (text_offset_y * 2) - (border_width * 2)) };
    RealVec2 text_position{ text_offset_x + line_spacing_y, text_offset_y };

    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    title_rect.setFillColor(config.get("window_bg_color").as<Color>());
    title_rect.setOutlineColor(getParent()->getFocus() ?
                               config.get("window_focused_border_color").as<Color>() :
                               config.get("window_border_color").as<Color>());
    title_rect.setOutlineThickness(border_width);
    title_rect.setPosition(rect_position);
    title_rect.setSize(rect_size);

    texture.draw(title_rect);

    if (titlebar_text.empty() == false)
    {
      title_text.setString(titlebar_text);
      title_text.setFont(the_default_bold_font);
      title_text.setCharacterSize(config.get("text_default_size").as<uint32_t>());

      title_text.setColor(config.get("text_color").as<Color>());
      title_text.setPosition(text_position);
      texture.draw(title_text);
    }

    texture.display();
  }

  void TitleBar::handleParentSizeChanged_(UintVec2 parent_size)
  {
    auto& config = Service<IConfigSettings>::get();
    float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size").as<uint32_t>());

    // Text offsets relative to the background rectangle.
    float text_offset_y = config.get("window_text_offset_y").as<float>();

    UintVec2 our_size{ parent_size.x, static_cast<unsigned int>(line_spacing_y + (text_offset_y * 2)) };

    setSize(our_size);
  }
}; // end namespace metagui