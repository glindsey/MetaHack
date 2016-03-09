#pragma once

#include "stdafx.h"

namespace metagui
{
  /// Superclass for MetaGUI events.
  /// This is not the most elegant implementation, but it works well enough and
  /// is similar to the SFML implementation of sf::Event.
  class Event
  {
  public:
    /// Enum class for results that events can return.
    enum class Result
    {
      Pending,      ///< The event has not been handled by any handler yet; only for variable initialization.
      Handled,      ///< The event was handled and should not be passed on
      Acknowledged, ///< The event was handled, but should still be passed on
      Ignored,      ///< The event was ignored, and should be passed on
      Unknown       ///< The event type is unknown; further action is at handler's discretion
    };

    enum class Type
    {
      Unknown,
      MemberCount
    };

    /// Virtual destructor.
    virtual ~Event() {}

  protected:
    Event() {}
  };

  struct EventKeyPressed : public Event
  {
    EventKeyPressed(sf::Event::KeyEvent& event)
      :
      code(event.code),
      alt(event.alt),
      control(event.control),
      shift(event.shift),
      system(event.system)
    {}

    sf::Keyboard::Key const code;
    bool alt;
    bool control;
    bool shift;
    bool system;
  };

  struct EventResized : public Event
  {
    EventResized(sf::Event::SizeEvent size)
      :
      new_size({ size.width, size.height })
    {}

    sf::Vector2u const new_size;
  };

  /// Using declaration for an EventDelegate.
  template< class T >
  using EventDelegate = std::function< Event::Result(Event&) >;
}; // end namespace metagui
