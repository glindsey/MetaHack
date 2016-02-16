#ifndef APP_H
#define APP_H

#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>

#include "EventHandler.h"
#include "Lua.h"

// Forward declarations
class MessageLog;
class MessageLogView;
class StateMachine;
class TileSheet;

class App : public EventHandler, public boost::noncopyable
{
public:
  explicit App(sf::RenderWindow& app_window);
  virtual ~App();

  void run();
  EventResult handle_event(sf::Event& event);
  sf::RenderWindow& get_window();
  bool has_window_focus();

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

  /// Get the current App instance.
  /// If no App instance currently exists, throws an exception.
  static App& instance();

protected:
private:
  sf::RenderWindow& m_app_window;
  std::unique_ptr<StateMachine> m_state_machine;
  bool m_is_running;
  bool m_has_window_focus;

  /// The SFGUI instance.
  std::unique_ptr<sfg::SFGUI> m_sfgui;

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

  static int s_frame_counter;

  /// A static pointer to the existing App instance.
  static App* s_p_instance;

  // === LUA FUNCTIONS ========================================================
  static int LUA_get_frame_counter(lua_State* L);

  /// Lua function to redirect printout to the message log.
  static int LUA_redirect_print(lua_State* L);

  /// Lua function to add a message to the message log.
  static int LUA_add(lua_State* L);
};

// Here are a few macros to save on typing.
#define the_default_font          App::instance().get_default_font()
#define the_default_bold_font     App::instance().get_default_bold_font()
#define the_default_mono_font     App::instance().get_default_mono_font()
#define the_default_unicode_font  App::instance().get_default_unicode_font()
#define the_shader                App::instance().get_shader()
#define the_RNG                   App::instance().get_rng()
#define the_message_log           App::instance().get_message_log()

#endif // APP_H
