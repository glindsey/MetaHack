#include "stdafx.h"

#include "GUITitleBar.h"

#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"
#include "types/Color.h"

namespace metagui
{
  TitleBar::TitleBar(std::string name)
    :
    GUIObject(name, {})
  {
  }

  TitleBar::~TitleBar()
  {
    //dtor
  }

  // === PROTECTED METHODS ======================================================
  void TitleBar::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
    auto& config = S<IConfigSettings>();

    if (getParent() == nullptr)
    {
      return;
    }

    auto parent_size = getParent()->getSize();
    auto titlebar_text = getParent()->getText();

    float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));

    // Text offsets relative to the background rectangle.
    RealVec2 text_offset = config.get("window-text-offset");
    float border_width = config.get("window-border-width");

    RealVec2 rect_position{ border_width, border_width };
    RealVec2 rect_size{ static_cast<float>(parent_size.x - (border_width * 2)),
      static_cast<float>(line_spacing_y + (text_offset.y * 2) - (border_width * 2)) };
    RealVec2 text_position{ text_offset.x + line_spacing_y, text_offset.y };

    // Draw the title in the upper-left corner.
    sf::RectangleShape title_rect;
    sf::Text title_text;

    auto bg_color = config.get("window-bg-color").get<Color>();
    auto focused_color = config.get("window-focused-border-color").get<Color>();
    auto unfocused_color = config.get("window-border-color").get<Color>();

    title_rect.setFillColor(bg_color);
    title_rect.setOutlineColor(getParent()->getFocus() ? focused_color : unfocused_color);
    title_rect.setOutlineThickness(border_width);
    title_rect.setPosition(rect_position);
    title_rect.setSize(rect_size);

    texture.draw(title_rect);

    if (titlebar_text.empty() == false)
    {
      Color text_color = config.get("text-color");

      title_text.setString(titlebar_text);
      title_text.setFont(the_default_bold_font);
      title_text.setCharacterSize(config.get("text-default-size"));

      title_text.setColor(text_color);
      title_text.setPosition(text_position);
      texture.draw(title_text);
    }

    texture.display();
  }

  bool TitleBar::onEvent_V(Event const& event)
  {
    auto id = event.getId();

    if (id == EventResized::id)
    {
      if (event.subject == getParent())
      {
        auto& castEvent = static_cast<EventResized const&>(event);
        auto& config = S<IConfigSettings>();
        float line_spacing_y = the_default_font.getLineSpacing(config.get("text-default-size"));

        // Text offsets relative to the background rectangle.
        RealVec2 text_offset = config.get("window-text-offset");

        UintVec2 our_size
        { 
          castEvent.newSize.x, 
          static_cast<unsigned int>(line_spacing_y + (text_offset.y * 2)) 
        };

        setSize(our_size);
      }
    }

    return false;
  }

}; // end namespace metagui