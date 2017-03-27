#include "stdafx.h"
#include "keybuffer/IKeyBuffer.h"

std::unordered_set<EventID> IKeyBuffer::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  events.insert(
  {
    EventBufferChanged::id(),
    EventCursorMoved::id()
  });
  return events;
}
