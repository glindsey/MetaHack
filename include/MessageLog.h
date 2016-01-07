#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <deque>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "gui/GUIPane.h"
#include "KeyBuffer.h"
#include "Lua.h"

/// A class that keeps track of game messages, and is renderable on-screen.
class MessageLog
{
public:
  MessageLog();

  virtual ~MessageLog();

  /// Add a message to the message log.
  /// The message added is automatically capitalized if it isn't already.
  void add(std::string message);

  /// Get the maximum size of the message queue.
  unsigned int get_message_queue_size();

  /// Get a reference to the message queue.
  std::deque<std::string>& get_message_queue();

  /// Get the key buffer used for entering debug commands.
  KeyBuffer& get_key_buffer();

protected:

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // MESSAGELOG_H
