#include "Event.h"

#include "Object.h"

/// Event implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
const EventID Event::id = (EventID)typeid(Event).hash_code();

EventID Event::getId() const
{
  return id;
}

void Event::serialize(std::ostream& os) const
{
  Serializable::serialize(os);
  os << " | subject: <";
  subject->serialize(os);
  os << ">";
};
