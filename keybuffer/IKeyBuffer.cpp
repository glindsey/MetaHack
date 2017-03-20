#include "stdafx.h"
#include "keybuffer/IKeyBuffer.h"

std::unordered_set<EventID> IKeyBuffer::registeredEvents() const
{
  return std::unordered_set<EventID>(
  {
    EventBufferChanged::id,
    EventCursorMoved::id
  });
}
