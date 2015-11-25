#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <boost/noncopyable.hpp>
#include <SFML/Graphics.hpp>

enum EventResult
{
  Pending,      ///< The event has not been handled by any handler yet; only for variable initialization.
  Handled,      ///< The event was handled and should not be passed on
  Acknowledged, ///< The event was handled, but should still be passed on
  Ignored,      ///< The event was ignored, and should be passed on
  Unknown       ///< The event type is unknown; further action is at handler's discretion
};

// Interface class indicating that an object handles SFML events.
class EventHandler
{
public:
  virtual ~EventHandler() {}
  virtual EventResult handle_event(sf::Event& event) = 0;

protected:
private:
};

#endif // EVENTHANDLER_H
