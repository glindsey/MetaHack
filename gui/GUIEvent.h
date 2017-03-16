#pragma once

#include "stdafx.h"

namespace metagui
{
  /// Superclass for MetaGUI events.
  struct Event
  {
  public:
    /// Enum class for results that events can return.
    enum class Result
    {
      Pending,      ///< The event has not been handled by any handler yet; only for variable initialization.
      Handled,      ///< The event was handled and is not be passed on to children or siblings
      Acknowledged, ///< The event is passed down to children
      Ignored,      ///< The event is not passed down to children (but is passed to siblings)
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

  struct EventDragFinished : public Event
  {
    EventDragFinished(sf::Mouse::Button button_, IntegerVec2 current_location_)
      :
      button(button_),
      current_location(current_location_)
    {}

    sf::Mouse::Button const button;
    IntegerVec2 const current_location;
  };

  struct EventDragStarted : public Event
  {
    EventDragStarted(sf::Mouse::Button button_, IntegerVec2 start_location_)
      :
      button(button_),
      start_location(start_location_)
    {}

    sf::Mouse::Button const button;
    IntegerVec2 const start_location;

    /// Number of pixels you have to move before it is considered "dragging" the object.
  };

  struct EventDragging : public Event
  {
    EventDragging(sf::Mouse::Button button_, IntegerVec2 current_location_)
      :
      button(button_),
      current_location(current_location_)
    {}

    sf::Mouse::Button const button;
    IntegerVec2 const current_location;

    /// Number of pixels you have to move before it is considered "dragging" the object.
    static unsigned int const drag_threshold = 16;
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
    bool const alt;
    bool const control;
    bool const shift;
    bool const system;
  };

  struct EventMouseDown : public Event
  {
    EventMouseDown(sf::Mouse::Button button_, IntegerVec2 location_)
      :
      button(button_),
      location(location_)
    {}

    sf::Mouse::Button const button;
    IntegerVec2 const location;
  };

  struct EventResized : public Event
  {
    EventResized(sf::Event::SizeEvent size)
      :
      new_size{ size.width, size.height }
    {}

    Vec2u const new_size;
  };

  /// Using declaration for an EventDelegate.
  template< class T >
  using EventDelegate = std::function< Event::Result(Event&) >;
}; // end namespace metagui

namespace std
{
  /// Stream operator override for metagui::Event::Result.
  inline ostream& operator<<(ostream& out, metagui::Event::Result const& result)
  {
    switch (result)
    {
      case metagui::Event::Result::Pending:
        out << "Pending";
        break;
      case metagui::Event::Result::Handled:
        out << "Handled";
        break;
      case metagui::Event::Result::Acknowledged:
        out << "Acknowledged";
        break;
      case metagui::Event::Result::Ignored:
        out << "Ignored";
        break;
      case metagui::Event::Result::Unknown:
        out << "Unknown";
        break;
      default:
        out << "(" << static_cast<unsigned int>(result) << "?)";
        break;
    }

    return out;
  }
}; // end namespace std