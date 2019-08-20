#include "Event.h"

#include "Object.h"

/// Event implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
const EventID Event::id = (EventID)typeid(Event).hash_code();

EventID Event::getId() const
{
  return id;
}

void Event::printToStream(std::ostream& os) const
{
  Printable::printToStream(os);
  os << " | subject: <";
  subject->printToStream(os);
  os << ">";
};
