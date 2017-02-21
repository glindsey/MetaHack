#include "stdafx.h"

#include "MessageLog.h"

#include "App.h"
#include "ErrorHandler.h"

struct MessageLog::Impl
{
  /// Maximum number of history lines saved before they start falling off
  /// the back end of the queue.
  unsigned int message_queue_size;

  /// Queue of previous messages.
  std::deque<std::string> message_queue;
};

MessageLog::MessageLog()
  :
  pImpl(NEW Impl())
{
  pImpl->message_queue_size = 250;  ///< @todo Move to ConfigSettings
}

MessageLog::~MessageLog()
{
  //dtor
}

void MessageLog::add(std::string message)
{
  message[0] = toupper(message[0], std::locale());

  pImpl->message_queue.push_front(message);

  while (pImpl->message_queue.size() > pImpl->message_queue_size)
  {
    pImpl->message_queue.pop_back();
  }

  notifyObservers(Event::Updated);
}

unsigned int MessageLog::get_message_queue_size()
{
  return pImpl->message_queue_size;
}

std::deque<std::string>& MessageLog::get_message_queue()
{
  return pImpl->message_queue;
}