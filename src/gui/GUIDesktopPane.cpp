#include "stdafx.h"

#include "gui/GUIDesktopPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  DesktopPane::DesktopPane(std::string name, sf::Vector2u size)
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
        sf::Mouse::Button button{ sfml_event.mouseButton.button };

        m_button_info[static_cast<unsigned int>(button)].pressed = true;
        m_button_info[static_cast<unsigned int>(button)].location = point;
        m_button_info[static_cast<unsigned int>(button)].elapsed.restart();

        /// @todo Handle click, double-click, etc.
        sfml_result = SFMLEventResult::Handled;
      }
      break;

      case sf::Event::EventType::MouseButtonReleased:
      {
        sf::Vector2i point{ sfml_event.mouseButton.x, sfml_event.mouseButton.y };
        sf::Mouse::Button button{ sfml_event.mouseButton.button };

        m_button_info[static_cast<unsigned int>(button)].pressed = false;
        m_button_info[static_cast<unsigned int>(button)].location = point;
        m_button_info[static_cast<unsigned int>(button)].elapsed.restart();

        /// @todo Handle click, double-click, etc.
        sfml_result = SFMLEventResult::Handled;
      }
      break;

      case sf::Event::EventType::MouseMoved:
      {
        sf::Vector2i point{ sfml_event.mouseMove.x, sfml_event.mouseMove.y };
        set_contains_mouse(this->contains_point(point));

        /// @todo Handle things like dragging, resizing
        sfml_result = SFMLEventResult::Handled;
      }
      break;

      case sf::Event::EventType::MouseLeft:
      {
        set_contains_mouse(false);

        for (auto& button : m_button_info)
        {
          button.pressed = false;
          button.location = { -1, -1 }; /// @todo Maybe fill in with last-known mouse coords?
          button.elapsed.restart();
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