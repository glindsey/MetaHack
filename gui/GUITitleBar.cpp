#include "stdafx.h"

#include "GUITitleBar.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "Service.h"
#include "types/Color.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    GUIObject(name, IntVec2(0, 0))
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

    float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_x = config.get("window_text_offset_x");
    float text_offset_y = config.get("window_text_offset_y");
    float border_width = config.get("window_border_width");

    RealVec2 rect_position{ border_width, border_width };
    RealVec2 rect_size{ static_cast<float>(parent_size.x - (border_width * 2)),
      static_cast<float>(line_spacing_y + (text_offset_y * 2) - (border_width * 2)) };
    RealVec2 text_position{ text_offset_x + line_spacing_y, text_offset_y };

    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    auto bg_color = config.get("window_bg_color").get<Color>();
    auto focused_color = config.get("window_focused_border_color").get<Color>();
    auto unfocused_color = config.get("window_border_color").get<Color>();

    title_rect.setFillColor(bg_color);
    title_rect.setOutlineColor(getParent()->getFocus() ? focused_color : unfocused_color);
    title_rect.setOutlineThickness(border_width);
    title_rect.setPosition(rect_position);
    title_rect.setSize(rect_size);

    texture.draw(title_rect);

    if (titlebar_text.empty() == false)
    {
      json text_color = config.get("text_color");

      title_text.setString(titlebar_text);
      title_text.setFont(the_default_bold_font);
      title_text.setCharacterSize(config.get("text_default_size"));

      title_text.setColor(Color(text_color["r"], text_color["g"], text_color["b"], 255));
      title_text.setPosition(text_position);
      texture.draw(title_text);
    }

    texture.display();
  }

  void TitleBar::handleParentSizeChanged_(UintVec2 parent_size)
  {
    auto& config = Service<IConfigSettings>::get();
    float line_spacing_y = the_default_font.getLineSpacing(config.get("text_default_size"));

    // Text offsets relative to the background rectangle.
    float text_offset_y = config.get("window_text_offset_y");

    UintVec2 our_size{ parent_size.x, static_cast<unsigned int>(line_spacing_y + (text_offset_y * 2)) };

    setSize(our_size);
  }
}; // end namespace metagui