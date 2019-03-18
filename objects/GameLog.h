#pragma once

#include <boost/noncopyable.hpp>
#include <deque>

#include "Object.h"

/// Definition of the game message log.
class GameLog : public Object
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

  GameLog();
  virtual ~GameLog();

  /// Add a message to the game log.
  /// The message added is automatically capitalized if it isn't already.
  void add(std::string message);

  /// Get the maximum size of the message queue.
  unsigned int getMessageQueueSize();

  /// Get a direct reference to the message queue.
  std::deque<std::string>& getMessageQueue();

protected:
private:
  /// Maximum number of history lines saved before they start falling off
  /// the back end of the queue.
  unsigned int m_messageQueueSize;

  /// Queue of previous messages.
  std::deque<std::string> m_messageQueue;

};
