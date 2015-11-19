#ifndef APP_H
#define APP_H

#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <memory>
#include <SFML/Graphics.hpp>

#include "EventHandler.h"
#include "Lua.h"

// Forward declarations
class MessageLog;
class StateMachine;
class TileSheet;

class App : public EventHandler, public boost::noncopyable
{
  public:
    App(sf::RenderWindow& app_window);
    virtual ~App();

    void run();
    EventResult handle_event(sf::Event& event);
    sf::RenderWindow& get_window();
    bool has_window_focus();

  protected:
  private:
    sf::RenderWindow& m_app_window;
    std::unique_ptr<StateMachine> m_state_machine;
    bool m_is_running;
    bool m_has_window_focus;

    static int s_frame_counter;
    static int LUA_get_frame_counter(lua_State* L);
};

/// Global default font instance
extern std::unique_ptr<sf::Font> default_font_;

/// Global default bold font instance
extern std::unique_ptr<sf::Font> default_bold_font_;

/// Global default monospace font instance
extern std::unique_ptr<sf::Font> default_mono_font_;

/// Global default Unicode font instance
extern std::unique_ptr<sf::Font> default_unicode_font_;

/// Global RNG instance
extern std::unique_ptr<boost::random::mt19937> rng_;

/// Global tile sheet instance
extern std::unique_ptr<TileSheet> tile_sheet_;

/// Global shader instance
extern std::unique_ptr<sf::Shader> shader_;

// Here are a few macros to save on typing.
#define the_default_font          (*(default_font_.get()))
#define the_default_bold_font     (*(default_bold_font_.get()))
#define the_default_mono_font     (*(default_mono_font_.get()))
#define the_default_unicode_font  (*(default_unicode_font_.get()))
#define the_shader                (*(shader_.get()))
#define the_RNG                   (*(rng_.get()))

#endif // APP_H
