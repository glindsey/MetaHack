#pragma once

#include <deque>

#include "IKeyBuffer.h"

/// Interface for a game message log.
class IMessageLog
{
public:
  virtual ~IMessageLog() = 0;

  /// Return a Null Object that implements this interface.
  static IMessageLog* getNull();

  /// Add a message to the message log.
  /// The message added is automatically capitalized if it isn't already.
  virtual void add(std::string message) = 0;

  /// Get the maximum size of the message queue.
  virtual unsigned int get_message_queue_size() = 0;

  /// Get a reference to the message queue.
  virtual std::deque<std::string>& get_message_queue() = 0;

  /// Get the key buffer used for entering debug commands.
  virtual IKeyBuffer& get_key_buffer() = 0;

protected:
private:

};