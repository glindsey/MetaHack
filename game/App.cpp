#include "stdafx.h"

#include "game/App.h"

#include "events/UIEvents.h"
#include "game/AppStateGameMode.h"
#include "game/AppStateMainMenu.h"
#include "game/AppStateSplashScreen.h"
#include "Service.h"
#include "services/FallbackConfigSettings.h"
#include "services/FileSystemGameRules.h"
#include "services/MessageLog.h"
#include "services/StringDictionary.h"
#include "state_machine/StateMachine.h"
#include "tilesheet/TileSheet.h"
#include "types/Color.h"
#include "game_windows/MessageLogView.h"

// Global declarations
std::unique_ptr<App> app_;
int App::s_frame_counter = 0;

App* App::s_p_instance;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

sf::IntRect calc_message_log_dimensions(sf::RenderWindow& window)
{
  sf::IntRect messageLogDims;
  auto& config = Service<IConfigSettings>::get();

  int inventory_area_width = config.get("inventory-area-width");
  int messagelog_area_height = config.get("messagelog-area-height");
  messageLogDims.width = window.getSize().x - (inventory_area_width + 24);
  messageLogDims.height = messagelog_area_height - 10;
  messageLogDims.left = 12;
  messageLogDims.top = 5;
  return messageLogDims;
}

App::App(sf::RenderWindow& app_window)
  :
  Object({
  EventAppQuitRequested::id,
  EventAppWindowClosed::id,
  EventAppWindowFocusChanged::id,
  EventAppWindowResized::id,
  UIEvents::EventKeyPressed::id,
  UIEvents::EventMouseDown::id,
  UIEvents::EventMouseUp::id,
  UIEvents::EventMouseMoved::id,
  UIEvents::EventMouseLeft::id,
  UIEvents::EventMouseWheelMoved::id }),
  m_appWindow{ app_window },
  m_app_texture{ NEW sf::RenderTexture() },
  m_is_running{ false },
  m_has_window_focus{ false }
{
  // Set the static instance pointer.
  if (!s_p_instance)
  {
    s_p_instance = this;
  }
  else
  {
    throw std::runtime_error("Tried to create more than one App instance");
  }

  // Register loggers.
  SET_UP_LOGGER("App",                false);
  SET_UP_LOGGER("Action",             false);
  SET_UP_LOGGER("Component",          false);
  SET_UP_LOGGER("ConfigSettings",     false);
  SET_UP_LOGGER("Entity",             false);
  SET_UP_LOGGER("EntityPool",         false);
  SET_UP_LOGGER("EventSystem",        false);
  SET_UP_LOGGER("Game",               false);
  SET_UP_LOGGER("GameRules",          false);
  SET_UP_LOGGER("GameState",          false);
  SET_UP_LOGGER("GUI",                false);
  SET_UP_LOGGER("Inventory",          false);
  SET_UP_LOGGER("InventoryArea",      false);
  SET_UP_LOGGER("InventorySelection", false);
  SET_UP_LOGGER("Lighting",           false);
  SET_UP_LOGGER("Lua",                false);
  SET_UP_LOGGER("Map",                false);
  SET_UP_LOGGER("MapFactory",         false);
  SET_UP_LOGGER("MapGenerator",       false);
  SET_UP_LOGGER("Narrator",           false);
  SET_UP_LOGGER("PlayerHandler",      false);
  SET_UP_LOGGER("Property",           false);
  SET_UP_LOGGER("PropertyDictionary", false);
  SET_UP_LOGGER("SenseSight",         false);
  SET_UP_LOGGER("StateMachine",       false);
  SET_UP_LOGGER("StringDictionary",   false);
  SET_UP_LOGGER("StringTransforms",   false);
  SET_UP_LOGGER("TileSheet",          false);
  SET_UP_LOGGER("Types",              false);
  SET_UP_LOGGER("Utilities",          false);


  // Load config settings.
  Service<IConfigSettings>::provide(NEW FallbackConfigSettings());

  auto& config = Service<IConfigSettings>::get();

  // Create the app state machine.
  m_state_machine.reset(NEW StateMachine("app_state_machine", this)),

  // Create the app texture for off-screen composition.
  m_app_texture->create(m_appWindow.getSize().x, m_appWindow.getSize().y);

  // Create the GUI desktop.
  m_gui_desktop.reset(NEW metagui::Desktop(*this, "Desktop", m_appWindow.getSize()));

  // Create the random number generator and seed it with the current time.
  m_rng.reset(NEW boost::random::mt19937(static_cast<unsigned int>(std::time(0))));

  // Create the default fonts.
  m_default_font.reset(NEW sf::Font());
  std::string defaultFont = config.get("font-name-default");
  FileName font_name = "resources/font/" + defaultFont + ".ttf";
  if (m_default_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default font";
  }

  m_default_bold_font.reset(NEW sf::Font());
  std::string defaultBoldFont = config.get("font-name-bold");
  font_name = "resources/font/" + defaultBoldFont + ".ttf";
  if (m_default_bold_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default bold font";
  }

  m_default_mono_font.reset(NEW sf::Font());
  std::string defaultMonoFont = config.get("font-name-mono");
  font_name = "resources/font/" + defaultMonoFont + ".ttf";
  if (m_default_mono_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default monospace font";
  }

  m_default_unicode_font.reset(NEW sf::Font());
  std::string defaultUnicodeFont = config.get("font-name-unicode");
  font_name = "resources/font/" + defaultUnicodeFont + ".ttf";
  if (m_default_unicode_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default Unicode font";
  }

  // Create the shader program.
  m_shader.reset(NEW sf::Shader());
  if (m_shader->loadFromFile("resources/shader/default.vert",
                             "resources/shader/default.frag") == false)
  {
    CLOG(FATAL, "App") << "Could not load the default shaders";
  }

  // Create the game rules provider.
  Service<IGameRules>::provide(NEW FileSystemGameRules());

  // Create the message log.
  Service<IMessageLog>::provide(NEW MessageLog());
  m_message_log.reset(NEW MessageLog());

  // Create the string dictionary, and try to load the default translation file.
  /// @todo Change this so language can be specified.
  Service<IStringDictionary>::provide(NEW StringDictionary("resources/strings.en"));

  /// @note Standard map views provider creation has been moved to AppStateGameMode.

  // Get the state machine.
  StateMachine& sm = *m_state_machine;

  // Add states to the state machine.
  sm.add_state(NEW AppStateSplashScreen(sm, app_window));
  sm.add_state(NEW AppStateMainMenu(sm, app_window));
  sm.add_state(NEW AppStateGameMode(sm, app_window));

  // Switch to initial state.
  // DEBUG: Go right to game mode for now.
  //sm->change_to("AppStateSplashScreen");
  sm.change_to("AppStateGameMode");

  // Set "window has focus" boolean to true.
  m_has_window_focus = true;
}

App::~App()
{
  m_state_machine.reset();
  m_gui_desktop.reset();
  m_appWindow.close();
  s_p_instance = nullptr;
}

void App::handle_sfml_event(sf::Event& sfmlEvent)
{
  switch (sfmlEvent.type)
  {
    case sf::Event::EventType::GainedFocus:
    {
      m_has_window_focus = true;
      broadcast(EventAppWindowFocusChanged({ true }));
      break;
    }

    case sf::Event::EventType::LostFocus:
    {
      m_has_window_focus = false;
      broadcast(EventAppWindowFocusChanged({ false }));
      break;
    }

    case sf::Event::EventType::Resized:
    {
      m_app_texture.reset(NEW sf::RenderTexture());
      m_app_texture->create(sfmlEvent.size.width, sfmlEvent.size.height);
      m_appWindow.setView(sf::View(
        sf::FloatRect(0, 0, 
                      static_cast<float>(sfmlEvent.size.width), 
                      static_cast<float>(sfmlEvent.size.height))));

      broadcast(EventAppWindowResized({ sfmlEvent.size.width, sfmlEvent.size.height }));
      break;
    }

    case sf::Event::EventType::Closed:
    {
      m_is_running = false;
      broadcast(EventAppWindowClosed());
      break;
    }

    case sf::Event::EventType::KeyPressed:
    {
      bool do_key_broadcast = true;

      switch (sfmlEvent.key.code)
      {
        case sf::Keyboard::Key::Q:
          if (sfmlEvent.key.alt && sfmlEvent.key.control)
          {
            m_is_running = false;
            broadcast(EventAppQuitRequested());
            do_key_broadcast = false;
          }
          break;

        default:
          break;
      }

      if (do_key_broadcast)
      {
        broadcast(UIEvents::EventKeyPressed(sfmlEvent.key.code,
                                            sfmlEvent.key.alt,
                                            sfmlEvent.key.control,
                                            sfmlEvent.key.shift,
                                            sfmlEvent.key.system));
      }

      break;
    }

    case sf::Event::EventType::MouseButtonPressed:
    {
      IntVec2 point{ sfmlEvent.mouseButton.x, sfmlEvent.mouseButton.y };
      UIEvents::EventMouseDown event{ sfmlEvent.mouseButton.button, point };
      broadcast(event);
    }
    break;

    case sf::Event::EventType::MouseButtonReleased:
    {
      IntVec2 point{ sfmlEvent.mouseButton.x, sfmlEvent.mouseButton.y };
      UIEvents::EventMouseUp event{ sfmlEvent.mouseButton.button, point };
      broadcast(event);
    }
    break;

    case sf::Event::EventType::MouseMoved:
    {
      IntVec2 point{ sfmlEvent.mouseMove.x, sfmlEvent.mouseMove.y };
      UIEvents::EventMouseMoved event{ point };
      broadcast(event);
    }
    break;

    case sf::Event::EventType::MouseLeft:
    {
      UIEvents::EventMouseLeft event{};
      broadcast(event);
    }
    break;

    case sf::Event::EventType::MouseWheelMoved:
    {
      broadcast(UIEvents::EventMouseWheelMoved(sfmlEvent.mouseWheel.delta,
                                               sfmlEvent.mouseWheel.x,
                                               sfmlEvent.mouseWheel.y));
      break;
    }

    default:
      break;
  }
}

sf::RenderWindow& App::get_window()
{
  return m_appWindow;
}

bool App::has_window_focus()
{
  return m_has_window_focus;
}

boost::random::mt19937 & App::get_rng()
{
  return *m_rng;
}

sf::Font & App::get_default_font()
{
  return *m_default_font;
}

sf::Font & App::get_default_bold_font()
{
  return *m_default_bold_font;
}

sf::Font & App::get_default_mono_font()
{
  return *m_default_mono_font;
}

sf::Font & App::get_default_unicode_font()
{
  return *m_default_unicode_font;
}

sf::Shader & App::get_shader()
{
  return *m_shader;
}

MessageLog & App::get_message_log()
{
  return *m_message_log;
}

metagui::Desktop & App::get_gui_desktop()
{
  return *m_gui_desktop;
}

TileSheet & App::get_tilesheet()
{
  return *m_tileSheet;
}

App & App::instance()
{
  if (s_p_instance)
  {
    return *s_p_instance;
  }
  else
  {
    throw std::runtime_error("App instance was requested, but it does not exist");
  }
}

void App::run()
{
  static sf::Clock frame_clock;

  // Set running boolean.
  m_is_running = true;

  frame_clock.restart();

  // Start the loop
  while (m_is_running)
  {
    // Process events
    sf::Event event;
    while (m_appWindow.pollEvent(event))
    {
      handle_sfml_event(event);
    }

    m_state_machine->execute();

    // Limit frame rate to 60 fps.
    if (frame_clock.getElapsedTime().asMicroseconds() > 16667)
    {
      frame_clock.restart();
      m_appWindow.clear();
      m_app_texture->clear(Color::Red);

      m_state_machine->render(*m_app_texture, s_frame_counter);

      m_app_texture->display();
      sf::Sprite sprite(m_app_texture->getTexture());
      m_appWindow.draw(sprite);

      m_appWindow.display();
      ++s_frame_counter;
    }
  }
}

int App::LUA_get_frame_counter(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 0)
  {
    CLOG(WARNING, "App") << "expected 0 arguments, got " << num_args;
    return 0;
  }

  lua_pushinteger(L, s_frame_counter);

  return 1;
}

int App::LUA_redirect_print(lua_State* L)
{
  int nargs = lua_gettop(L);

  for (int i = 1; i <= nargs; i++)
  {
    if (lua_isstring(L, i))
    {
      std::string str = lua_tostring(L, i);
      Service<IMessageLog>::get().add(str);
    }
    else
    {
      /* Do something with non-strings if you like */
    }
  }

  return 0;
}

int App::LUA_add(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "App") << "Expected 1 argument, got " << num_args;
  }
  else
  {
    std::string str = lua_tostring(L, 1);
    Service<IMessageLog>::get().add(str);
  }

  return 0;
}

int App::LUA_get_config(lua_State* L)
{
  int num_args = lua_gettop(L);
  auto& config = Service<IConfigSettings>::get();

  if (num_args != 1)
  {
    CLOG(WARNING, "ConfigSettings") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  const char* key = lua_tostring(L, 1);

  /// @todo Re-implement me
  //if (!config.contains(key))
  //{
    lua_pushnil(L);
    return 1;
  //}
  //else
  //{
  //  auto result = config.get(key);
  //  int args = instance().m_lua->push_value(result);
  //  return args;
  //}
}