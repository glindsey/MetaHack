#include "stdafx.h"

#include "GUIDesktop.h"

#include "design_patterns/Object.h"
#include "game/App.h"
#include "services/IConfigSettings.h"
#include "services/Service.h"

namespace metagui
{
  Desktop::Desktop(Object& event_parent,
                   std::string name, 
                   UintVec2 size) :
    GUIObject{ 
    name, 
    {
      EventDragStarted::id,
      EventDragging::id,
      EventDragFinished::id
    },
    IntVec2(0, 0), 
    size },
    m_event_parent{ event_parent }
  {
    App::instance().addObserver(*this, EventID::All);
  }

  Desktop::~Desktop()
  {
    App::instance().removeObserver(*this, EventID::All);
  }

  // === PROTECTED METHODS ======================================================

  bool Desktop::onEvent_V(Event const& event)
  {
    auto id = event.getId();
    if (id == EventResized::id)
    {
      if (event.subject == getParent())
      {
        auto& castEvent = static_cast<EventResized const&>(event);
        setSize(castEvent.newSize);
      }
    }
    else if (id == UIEvents::EventMouseDown::id)
    {
      auto& castEvent = static_cast<UIEvents::EventMouseDown const&>(event);
      auto& button{ m_button_info[static_cast<unsigned int>(castEvent.button)] };
      button.pressed = true;
      button.location = castEvent.location;
      button.elapsed.restart();
    }
    else if (id == UIEvents::EventMouseUp::id)
    {
      auto& castEvent = static_cast<UIEvents::EventMouseUp const&>(event);
      auto& button{ m_button_info[static_cast<unsigned int>(castEvent.button)] };
      button.pressed = false;
      button.location = castEvent.location;
      button.elapsed.restart();

      if (button.dragging)
      {
        button.dragging = false;
        EventDragFinished event{ castEvent.button, castEvent.location };
        broadcast(event);
      }
    }
    else if (id == UIEvents::EventMouseMoved::id)
    {
      auto& castEvent = static_cast<UIEvents::EventMouseMoved const&>(event);
      m_mouseLocation = castEvent.location;

      /// @todo Handle entities like dragging, resizing
      for (unsigned int index = 0; index < sf::Mouse::ButtonCount; ++index)
      {
        auto& button_info = m_button_info[index];

        if ((button_info.dragging == true) ||
          (button_info.pressed &&
           distance(castEvent.location, button_info.location) > EventDragging::dragThreshold))
        {
          if (button_info.dragging != true)
          {
            button_info.dragging = true;
            EventDragStarted event{ static_cast<sf::Mouse::Button>(index), button_info.location };
            broadcast(event);
          }

          EventDragging event{ static_cast<sf::Mouse::Button>(index), castEvent.location };
          broadcast(event);
        }
      }
    }
    else if (id == UIEvents::EventMouseLeft::id)
    {
      for (unsigned int index = 0; index < sf::Mouse::ButtonCount; ++index)
      {
        auto& button_info = m_button_info[index];

        if (button_info.pressed)
        {
          button_info.pressed = false;
          // Don't update location, just keep it as last seen
          button_info.elapsed.restart();

          if (button_info.dragging)
          {
            button_info.dragging = false;
            EventDragFinished event{ static_cast<sf::Mouse::Button>(index), button_info.location };
            broadcast(event);
          }
        }
      }
    }

    return false;
  }

  void Desktop::drawPreChildren_(sf::RenderTexture& texture, int frame)
  {
  }

}; // end namespace metagui