#pragma once

#include "design_patterns/Subject.h"
#include "design_patterns/Observer.h"

class Object : public Observer, public Subject
{
public:
  Object(std::unordered_set<EventID> const events) : Observer(), Subject(events) {}
  virtual ~Object() {}

  /// Forward an event on to observers.
  /// Make sure two objects aren't observing each other or you may end up
  /// with an endless loop!
  bool forward(Event const& event)
  {
    std::unique_ptr<Event> event_copy{ event.heapClone() };
    return broadcast(*event_copy);
  }
};