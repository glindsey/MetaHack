#include "objects/GameLog.h"

#include <cctype>
#include <deque>

#include "game/App.h"

GameLog::GameLog()
  :
  Object({ GameLog::EventMessageAdded::id }, "GameLog")
{
  m_messageQueueSize = 250;  ///< @todo Move to ConfigSettings
}

GameLog::~GameLog()
{
  //dtor
}

void GameLog::add(std::string message)
{
  message[0] = toupper(message[0], std::locale());

  m_messageQueue.push_front(message);

  while (m_messageQueue.size() > m_messageQueueSize)
  {
    m_messageQueue.pop_back();
  }

  EventMessageAdded event(message);
  broadcast(event);
}

unsigned int GameLog::getMessageQueueSize()
{
  return m_messageQueueSize;
}

std::deque<std::string>& GameLog::getMessageQueue()
{
  return m_messageQueue;
}
