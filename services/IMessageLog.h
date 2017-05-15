#pragma once

#include <deque>

#include "Subject.h"

/// Interface for a game message log.
class IMessageLog : public Subject
{
public:
  struct EventMessageAdded : public ConcreteEvent<EventMessageAdded>
  {
    EventMessageAdded(std::string message_)
      :
      message{ message_ }
    {}

    std::string const message;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | string: \"" << message << "\"";
    }
  };

  IMessageLog();
  virtual ~IMessageLog() = 0;

  /// Return a Null Object that implements this interface.
  static IMessageLog* getDefault();

  /// Add a message to the message log.
  /// The message added is automatically capitalized if it isn't already.
  virtual void add(std::string message) = 0;

  /// Get the maximum size of the message queue.
  virtual unsigned int getMessageQueueSize() = 0;

  /// Get a reference to the message queue.
  virtual std::deque<std::string>& getMessageQueue() = 0;

protected:
private:

};