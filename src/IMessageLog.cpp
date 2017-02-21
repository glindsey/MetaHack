#include "stdafx.h"

#include "New.h"
#include "NullMessageLog.h"

IMessageLog::~IMessageLog()
{}

IMessageLog* IMessageLog::getNull()
{
  return NEW NullMessageLog();
}
