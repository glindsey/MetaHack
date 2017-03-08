#include "stdafx.h"

#include "Event.h"

/// Event implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
const EventID Event::id = (EventID)typeid(Event).hash_code();

EventID Event::getId() const
{
  return id;
}

void Event::serialize(std::ostream& o) const
{
  Serializable::serialize(o);
  o << " | subject: <" << subject << ">";
};