#include "stdafx.h"

#include "gui/GUIDesktopPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  DesktopPane::DesktopPane(StringKey name, sf::Vector2u size)
    :
    Object(name, sf::Vector2i(0, 0), size)
  {}

  DesktopPane::~DesktopPane()
  {}

  SFMLEventResult DesktopPane::handle_sfml_event(sf::Event & sfml_event)
  {
    SFMLEventResult sfml_result = SFMLEventResult::Ignored;

    switch (sfml_event.type)
    {
      case sf::Event::EventType::KeyPressed:
      {
        EventKeyPressed event{ sfml_event.key };
        handle_gui_event(event);
        sfml_result = SFMLEventResult::Handled;
      }
      break;

      case sf::Event::EventType::Resized:
      {
        set_size({ sfml_event.size.width, sfml_event.size.height });
        EventResized event{ sfml_event.size };
        handle_gui_event(event);
        sfml_result = SFMLEventResult::Acknowledged;
      }
      break;

      case sf::Event::EventType::MouseButtonPressed:
      {
        sf::Vector2i point{ sfml_event.mouseButton.x, sfml_event.mouseButton.y };
        auto& button{ m_button_info[static_cast<unsigned int>(sfml_event.mouseButton.button)] };

        button.pressed = true;
        button.location = point;
        button.elapsed.restart();

        EventMouseDown event{ sfml_event.mouseButton.button, point };

        /// @todo Handle click, double-click, etc.
        sfml_result = SFMLEventResult::Acknowledged;
      }
      break;

      case sf::Event::EventType::MouseButtonReleased:
      {
        sf::Vector2i point{ sfml_event.mouseButton.x, sfml_event.mouseButton.y };
        auto& button{ m_button_info[static_cast<unsigned int>(sfml_event.mouseButton.button)] };

        button.pressed = false;
        button.location = point;
        button.elapsed.restart();

        if (button.dragging)
        {
          button.dragging = false;
          /// @todo Send EventDragFinished
        }

        /// @todo Handle click, double-click, etc.
        sfml_result = SFMLEventResult::Acknowledged;
      }
      break;

      case sf::Event::EventType::MouseMoved:
      {
        sf::Vector2i point{ sfml_event.mouseMove.x, sfml_event.mouseMove.y };
        set_contains_mouse(this->contains_point(point));
        m_mouse_location = point;

        /// @todo Handle things like dragging, resizing
        for (unsigned int index = 0; index < sf::Mouse::ButtonCount; ++index)
        {
          auto& button_info = m_button_info[index];

          if (button_info.pressed &&
              distance(point, button_info.location) > EventDragging::drag_threshold)
          {
            button_info.dragging = true;
            EventDragging event{ static_cast<sf::Mouse::Button>(index), button_info.location, point };
            handle_gui_event(event);

            sfml_result = SFMLEventResult::Handled;
          }
        }

        sfml_result = SFMLEventResult::Handled;
      }
      break;

      case sf::Event::EventType::MouseLeft:
      {
        set_contains_mouse(false);

        for (auto& button : m_button_info)
        {
          if (button.pressed)
          {
            button.pressed = false;
            // Don't update location, just keep it as last seen
            button.elapsed.restart();

            if (button.dragging)
            {
              button.dragging = false;
              /// @todo Send EventDragFinished
            }
          }
        }
        sfml_result = SFMLEventResult::Handled;
      }
      break;

      default:
        break;
    }

    return sfml_result;
  }

  // === PROTECTED METHODS ======================================================

  Event::Result DesktopPane::handle_event_before_children_(EventResized& event)
  {
    set_size({ event.new_size.x, event.new_size.y });
    return Event::Result::Acknowledged;
  }

  void DesktopPane::render_self_before_children_(sf::RenderTexture& texture, int frame)
  {
    texture.clear(sf::Color::Magenta);
  }
}; // end namespace metagui