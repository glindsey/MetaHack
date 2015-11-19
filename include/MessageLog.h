#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "gui/GUIPane.h"
#include "KeyBuffer.h"
#include "Lua.h"

/// A class that keeps track of game messages, and is renderable on-screen.
class MessageLog : public GUIPane
{
  public:
    virtual ~MessageLog();

    /// Create the message log, if it doesn't exist.
    /// If it does, throw an exception.
    static MessageLog& create(sf::IntRect dimensions);

    /// Get the message log instance, if it exists.
    /// If it does not, throw an exception. (We cannot create it on the fly.)
    static MessageLog& instance();

    /// Add a message to the message log.
    /// The message added is automatically capitalized if it isn't already.
    void add(std::string message);

    /// Get the key buffer used for entering debug commands.
    KeyBuffer& get_key_buffer();

    virtual EventResult handle_event(sf::Event& event) override;

  protected:
    virtual std::string _render_contents(sf::RenderTarget& target, int frame) override;

  private:
    MessageLog(sf::IntRect dimensions);

    void initialize();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    /// Static instance.
    static std::unique_ptr<MessageLog> instance_;

    /// Lua function to redirect printout to the message log.
    static int LUA_redirect_print(lua_State* L);

    /// Lua function to add a message to the message log.
    static int LUA_add(lua_State* L);
};

#define the_message_log   (MessageLog::instance())

#endif // MESSAGELOG_H
