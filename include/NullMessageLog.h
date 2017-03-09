#pragma once

#include <deque>

#include "IMessageLog.h"

/// Implementation of a null message log.
class NullMessageLog : public IMessageLog
{
public:
  NullMessageLog() {}

  virtual ~NullMessageLog() {}

  virtual void add(std::string message) override {}

  virtual unsigned int getMessageQueueSize() override
  {
    return 0;
  }

  virtual std::deque<std::string>& getMessageQueue() override { return message_queue; }

protected:
private:
  std::deque<std::string> message_queue;

};