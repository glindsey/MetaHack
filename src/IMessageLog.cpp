#include "stdafx.h"

#include "New.h"
#include "NullMessageLog.h"

IMessageLog::~IMessageLog()
{}

IMessageLog* IMessageLog::getNull()
{
  return NEW NullMessageLog();
}

std::unordered_set<EventID> IMessageLog::registeredEvents() const
{
  return std::unordered_set<EventID>(
  {
    EventMessageAdded::id
  });
}

