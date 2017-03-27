#include "stdafx.h"

#include "utilities/New.h"
#include "services/NullMessageLog.h"

IMessageLog::~IMessageLog()
{}

IMessageLog* IMessageLog::getDefault()
{
  return NEW NullMessageLog();
}

std::unordered_set<EventID> IMessageLog::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  events.insert(
  {
    EventMessageAdded::id()
  });
  return events;
}

