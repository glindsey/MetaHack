#pragma once

#include "design_patterns/Event.h"

namespace UIEvents
{
  struct EventKeyPressed : public ConcreteEvent<EventKeyPressed>
  {
    EventKeyPressed(sf::Keyboard::Key code_, bool alt_, bool control_, bool shift_, bool system_)
      :
      code{ code_ },
      alt{ alt_ },
      control{ control_ },
      shift{ shift_ },
      system{ system_ }
    {}

    sf::Keyboard::Key const code;
    bool const alt;
    bool const control;
    bool const shift;
    bool const system;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | code: " << code <<
        " | alt: " << (alt ? "true" : "false") <<
        " | control: " << (control ? "true" : "false") <<
        " | shift: " << (shift ? "true" : "false") <<
        " | system: " << (system ? "true" : "false");
    }
  };

  struct EventMouseDown : public ConcreteEvent<EventMouseDown>
  {
    EventMouseDown(sf::Mouse::Button button_, IntVec2 location_)
      :
      button(button_),
      location(location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const location;

    /// @todo write serialize()
  };

  struct EventMouseUp : public ConcreteEvent<EventMouseUp>
  {
    EventMouseUp(sf::Mouse::Button button_, IntVec2 location_)
      :
      button(button_),
      location(location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const location;

    /// @todo write serialize()
  };

  struct EventMouseMoved : public ConcreteEvent<EventMouseMoved>
  {
    EventMouseMoved(IntVec2 location_)
      :
      location(location_)
    {}

    IntVec2 const location;

    /// @todo write serialize()
  };

  struct EventMouseLeft : public ConcreteEvent<EventMouseLeft>
  {
    EventMouseLeft()
    {}

    /// @todo write serialize()
  };

  struct EventMouseWheelMoved : public ConcreteEvent<EventMouseWheelMoved>
  {
    EventMouseWheelMoved(int delta_, int x_, int y_)
      : delta{ delta_ }, x{ x_ }, y{ y_ }
    {}

    int const delta;
    int const x, y;

    void serialize(std::ostream& os) const
    {
      os << " | delta: " << delta <<
        " | x: " << x <<
        " | y: " << y;
    }
  };
} // end namespace

