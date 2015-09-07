#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "gui/GUIPane.h"
#include "KeyBuffer.h"

/// A class that keeps track of game messages, and is renderable on-screen.
class MessageLog : public GUIPane
{
  public:
    MessageLog(sf::IntRect dimensions);
    virtual ~MessageLog();

    /// Add a message to the message log.
    /// The message added is automatically capitalized if it isn't already.
    void add(std::string message);

    /// Get the key buffer used for entering debug commands.
    KeyBuffer& get_key_buffer();

    virtual EventResult handle_event(sf::Event& event) override;

  protected:
    virtual std::string render_contents(int frame) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // MESSAGELOG_H
