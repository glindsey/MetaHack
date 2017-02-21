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

  virtual unsigned int get_message_queue_size() override
  {
    return 0;
  }

  virtual std::deque<std::string>& get_message_queue() override { return message_queue; }

protected:
private:
  std::deque<std::string> message_queue;

};