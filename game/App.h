#ifndef APP_H
#define APP_H

#include "stdafx.h"

#include "Subject.h"
#include "lua/LuaObject.h"
#include "types/ISFMLEventHandler.h"

#include "GUIDesktop.h"

// Forward declarations
class ConfigSettings;
class Lua;
class MessageLog;
class MessageLogView;
class StateMachine;
class StringDictionary;
class TileSheet;

class App : public ISFMLEventHandler, public Subject
{
public:
  struct EventAppQuitRequested : public ConcreteEvent<EventAppQuitRequested>
  {
    EventAppQuitRequested()
    {}

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
    }
  };

  struct EventAppWindowResized : public ConcreteEvent<EventAppWindowResized>
  {
    EventAppWindowResized(UintVec2 new_size_)
      :
      new_size(new_size_)
    {}

    UintVec2 const new_size;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | new size:" << new_size;
    }
  };

  struct EventAppWindowClosed : public ConcreteEvent<EventAppWindowClosed>
  {
    EventAppWindowClosed()
    {}

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
    }
  };

  struct EventAppWindowFocusChanged : public ConcreteEvent<EventAppWindowFocusChanged>
  {
    EventAppWindowFocusChanged(bool focused_)
      :
      focused{ focused_ }
    {}

    bool const focused;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | focused: " << (focused ? "true" : "false");
    }
  };

  struct EventKeyPressed : public ConcreteEvent<EventKeyPressed>
  {
    EventKeyPressed(sf::Keyboard::Key code_, bool alt_, bool control_, bool shift_, bool system_)
      :
      code{ code_ },
      alt{ alt_ },
      control{ control_ },
      shift{ shift_ },
      system{ system_ }
    {}

    sf::Keyboard::Key const code;
    bool const alt;
    bool const control;
    bool const shift;
    bool const system;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | code: " << code <<
        " | alt: " << (alt ? "true" : "false") <<
        " | control: " << (control ? "true" : "false") <<
        " | shift: " << (shift ? "true" : "false") <<
        " | system: " << (system ? "true" : "false");
    }
  };

  struct EventMouseWheelMoved : public ConcreteEvent<EventMouseWheelMoved>  
  {
    EventMouseWheelMoved(int delta_, int x_, int y_)
      : delta{ delta_ }, x{ x_ }, y{ y_ }
    {}

    int const delta;
    int const x, y;

    void serialize(std::ostream& os) const
    {
      os << " | delta: " << delta <<
        " | x: " << x <<
        " | y: " << y;
    }
  };

  explicit App(sf::RenderWindow& app_window);
  App(App const&) = delete;
  App(App&&) = delete;
  App& operator=(App const&) = delete;
  App& operator=(App&&) = delete;
  virtual ~App();

  void run();

  SFMLEventResult handle_sfml_event(sf::Event& event);

  sf::RenderWindow& get_window();

  bool has_window_focus();

  /// Get the Lua state object.
  Lua& get_lua();

  /// Get the random number generator.
  boost::random::mt19937& get_rng();

  /// Get the default font.
  sf::Font& get_default_font();

  /// Get the default bold font.
  sf::Font& get_default_bold_font();

  ///Get the default monospace font.
  sf::Font& get_default_mono_font();

  /// Get the default Unicode font.
  sf::Font& get_default_unicode_font();

  /// Get the shader.
  sf::Shader& get_shader();

  /// Get the MessageLog instance.
  MessageLog& get_message_log();

  /// Get the Desktop instance.
  metagui::Desktop& get_gui_desktop();

  /// Get the TileSheet instance.
  TileSheet& get_tilesheet();

  /// Get the current App instance.
  /// If no App instance currently exists, throws an exception.
  static App& instance();

  virtual std::unordered_set<EventID> registeredEvents() const override;

protected:

private:
  sf::RenderWindow& m_app_window;

  /// Pointer to off-screen buffer for drawing composition.
  std::unique_ptr<sf::RenderTexture> m_app_texture;

  /// The GUI desktop.
  std::unique_ptr<metagui::Desktop> m_gui_desktop;

  /// The state machine.
  std::unique_ptr<StateMachine> m_state_machine;
  bool m_is_running;
  bool m_has_window_focus;

  /// The Lua state object.
  std::unique_ptr<Lua> m_lua;

  /// The RNG instance.
  std::unique_ptr<boost::random::mt19937> m_rng;

  /// The default font instance.
  std::unique_ptr<sf::Font> m_default_font;

  /// The default bold font instance.
  std::unique_ptr<sf::Font> m_default_bold_font;

  /// The default monospace font instance.
  std::unique_ptr<sf::Font> m_default_mono_font;

  /// The default Unicode font instance.
  std::unique_ptr<sf::Font> m_default_unicode_font;

  /// The shader instance.
  std::unique_ptr<sf::Shader> m_shader;

  /// The message log.
  std::unique_ptr<MessageLog> m_message_log;

  /// The tilesheet.
  std::unique_ptr<TileSheet> m_tile_sheet;

  static int s_frame_counter;

  /// A static pointer to the existing App instance.
  static App* s_p_instance;

  // === LUA FUNCTIONS ========================================================
  static int LUA_get_frame_counter(lua_State* L);

  /// Lua function to redirect printout to the message log.
  static int LUA_redirect_print(lua_State* L);

  /// Lua function to add a message to the message log.
  static int LUA_add(lua_State* L);

  /// Lua function to access config settings.
  static int LUA_get_config(lua_State* L);

  /// @todo Add Lua functions to get/set dictionary values.
};

// Here are a few macros to save on typing.
#define the_lua_instance          (App::instance().get_lua())
#define the_lua_state             (App::instance().get_lua().state())
#define the_default_font          (App::instance().get_default_font())
#define the_default_bold_font     (App::instance().get_default_bold_font())
#define the_default_mono_font     (App::instance().get_default_mono_font())
#define the_default_unicode_font  (App::instance().get_default_unicode_font())
#define the_desktop               (App::instance().get_gui_desktop())
#define the_shader                (App::instance().get_shader())
#define the_RNG                   (App::instance().get_rng())

#define tr(x)                     (Service<IStringDictionary>::get().get(x))
#define putMsg(x)                 do { Service<IMessageLog>::get().add(x); } while(0)
#define putTr(x)                  do { putMsg(makeTr(x)); } while(0)

#endif // APP_H