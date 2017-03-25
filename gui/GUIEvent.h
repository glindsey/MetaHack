#pragma once

#include "stdafx.h"

namespace metagui
{
  /// Superclass for MetaGUI events.
  struct GUIEvent
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
    virtual ~GUIEvent() {}

  protected:
    GUIEvent() {}
  };

  struct GUIEventDragFinished : public GUIEvent
  {
    GUIEventDragFinished(sf::Mouse::Button button_, IntVec2 current_location_)
      :
      button(button_),
      current_location(current_location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const current_location;
  };

  struct GUIEventDragStarted : public GUIEvent
  {
    GUIEventDragStarted(sf::Mouse::Button button_, IntVec2 start_location_)
      :
      button(button_),
      start_location(start_location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const start_location;

    /// Number of pixels you have to move before it is considered "dragging" the object.
  };

  struct GUIEventDragging : public GUIEvent
  {
    GUIEventDragging(sf::Mouse::Button button_, IntVec2 current_location_)
      :
      button(button_),
      current_location(current_location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const current_location;

    /// Number of pixels you have to move before it is considered "dragging" the object.
    static unsigned int const drag_threshold = 16;
  };

  struct GUIEventKeyPressed : public GUIEvent
  {
    GUIEventKeyPressed(sf::Event::KeyEvent& event)
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

  struct GUIEventMouseDown : public GUIEvent
  {
    GUIEventMouseDown(sf::Mouse::Button button_, IntVec2 location_)
      :
      button(button_),
      location(location_)
    {}

    sf::Mouse::Button const button;
    IntVec2 const location;
  };

  struct GUIEventResized : public GUIEvent
  {
    GUIEventResized(sf::Event::SizeEvent size)
      :
      new_size{ size.width, size.height }
    {}

    UintVec2 const new_size;
  };

  /// Using declaration for an EventDelegate.
  template< class T >
  using GUIEventDelegate = std::function< GUIEvent::Result(GUIEvent&) >;
}; // end namespace metagui

namespace std
{
  /// Stream operator override for metagui::Event::Result.
  inline ostream& operator<<(ostream& out, metagui::GUIEvent::Result const& result)
  {
    switch (result)
    {
      case metagui::GUIEvent::Result::Pending:
        out << "Pending";
        break;
      case metagui::GUIEvent::Result::Handled:
        out << "Handled";
        break;
      case metagui::GUIEvent::Result::Acknowledged:
        out << "Acknowledged";
        break;
      case metagui::GUIEvent::Result::Ignored:
        out << "Ignored";
        break;
      case metagui::GUIEvent::Result::Unknown:
        out << "Unknown";
        break;
      default:
        out << "(" << static_cast<unsigned int>(result) << "?)";
        break;
    }

    return out;
  }
}; // end namespace std