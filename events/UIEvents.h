#pragma once

#include "design_patterns/Event.h"
#include "types/common.h"

namespace UIEvents
{
  struct EventKeyPressed : public ConcreteEvent<EventKeyPressed, EventID(0x80000000)>
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

  struct EventMouseDown : public ConcreteEvent<EventMouseDown, EventID(0x80000001)>
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

  struct EventMouseUp : public ConcreteEvent<EventMouseUp, EventID(0x80000002)>
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

  struct EventMouseMoved : public ConcreteEvent<EventMouseMoved, EventID(0x80000003)>
  {
    EventMouseMoved(IntVec2 location_)
      :
      location(location_)
    {}

    IntVec2 const location;

    /// @todo write serialize()
  };

  struct EventMouseLeft : public ConcreteEvent<EventMouseLeft, EventID(0x80000004)>
  {
    EventMouseLeft()
    {}

    /// @todo write serialize()
  };

  struct EventMouseWheelMoved : public ConcreteEvent<EventMouseWheelMoved, EventID(0x80000005)>
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

