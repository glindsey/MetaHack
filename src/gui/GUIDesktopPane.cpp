#include "stdafx.h"

#include "gui/GUIDesktopPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  DesktopPane::DesktopPane(std::string name, sf::Vector2u size)
    :
    Pane(name, sf::Vector2i(0, 0), size)
  {}

  DesktopPane::~DesktopPane()
  {}

  // === PROTECTED METHODS ======================================================

  EventResult DesktopPane::handle_event_before_children_(sf::Event & event)
  {
    EventResult result = EventResult::Ignored;
    switch (event.type)
    {
      case sf::Event::EventType::Resized:
        set_size({ event.size.width, event.size.height });
        result = EventResult::Acknowledged;
        break;

      default:
        break;
    }

    return result;
  }

  void DesktopPane::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
#if 0
    sf::Vector2u size = get_size();

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
#endif
  }
}; // end namespace metagui