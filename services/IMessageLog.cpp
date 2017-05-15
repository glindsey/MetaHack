#include "stdafx.h"

#include "utilities/New.h"
#include "services/NullMessageLog.h"

IMessageLog::IMessageLog() :
  Subject({
  EventMessageAdded::id })
{}

IMessageLog::~IMessageLog()
{}

IMessageLog* IMessageLog::getDefault()
{
  return NEW NullMessageLog();
}