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
    texture.clear(sf::Color::Magenta);
  }

}; // end namespace metagui