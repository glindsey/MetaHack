#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "EventHandler.h"
#include "Renderable.h"

/// A class that keeps track of game messages, and is renderable on-screen.
class MessageLog : public EventHandler, public Renderable
{
  public:
    MessageLog(sf::IntRect dimensions);
    virtual ~MessageLog();

    void set_focus(bool focus);
    bool get_focus();

    sf::IntRect get_dimensions();
    void set_dimensions(sf::IntRect dimensions);

    /// Add a message to the message log.
    /// The message added is automatically capitalized if it isn't already.
    void add(std::string message);

    virtual EventResult handle_event(sf::Event& event);

    virtual bool render(sf::RenderTarget& target, int frame);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // MESSAGELOG_H
