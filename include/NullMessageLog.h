#pragma once

#include <deque>

#include "IMessageLog.h"
#include "NullKeyBuffer.h"

/// Implementation of a null message log.
class NullMessageLog : public IMessageLog
{
public:
  NullMessageLog() {}

  virtual ~NullMessageLog() {}

  virtual void add(std::string message) {}

  virtual unsigned int get_message_queue_size()
  {
    return 0;
  }

  /// Get a reference to the message queue.
  virtual std::deque<std::string>& get_message_queue() { return message_queue; }

  /// Get the key buffer used for entering debug commands.
  virtual IKeyBuffer& get_key_buffer() { return key_buffer; }

protected:
private:
  std::deque<std::string> message_queue;
  NullKeyBuffer key_buffer;

};