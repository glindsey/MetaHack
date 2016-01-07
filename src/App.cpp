#include "App.h"

#include <boost/random/uniform_int_distribution.hpp>

#include "AppStateGameMode.h"
#include "AppStateMainMenu.h"
#include "AppStateSplashScreen.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "MessageLog.h"
#include "MessageLogView.h"
#include "New.h"
#include "StateMachine.h"

#include <stdlib.h>
#include <crtdbg.h>

// Global declarations
std::unique_ptr<App> app_;
int App::s_frame_counter = 0;

App* App::s_p_instance;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

sf::IntRect calc_message_log_dimensions(sf::RenderWindow& window)
{
  sf::IntRect messageLogDims;
  unsigned int inventory_area_width = Settings.get<unsigned int>("inventory_area_width");
  unsigned int messagelog_area_height = Settings.get<unsigned int>("messagelog_area_height");
  messageLogDims.width = window.getSize().x - (inventory_area_width + 24);
  messageLogDims.height = messagelog_area_height - 10;
  //messageLogDims.height = static_cast<int>(window.getSize().y * 0.25f) - 10;
  messageLogDims.left = 12;
  messageLogDims.top = 5;
  return messageLogDims;
}

int main()
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  // Scoping everything here so it is properly destroyed before checking for memory leaks.
  {
    std::unique_ptr<sf::RenderWindow> app_window;
    std::unique_ptr<App> app;

#ifdef NDEBUG
    try
#endif
    {
      // Check to make sure shaders are available.
      if (!sf::Shader::isAvailable())
      {
        throw std::exception("Shaders are not available on this platform");
      }

      // Create and open the main window.
      app_window.reset(NEW sf::RenderWindow(sf::VideoMode(1066, 600), "Magicule Saga"));

      // Create and run the app instance.
      app.reset(NEW App(*(app_window.get())));
      app->run();
    }
#ifdef NDEBUG
    catch (std::exception& e)
    {
      FATAL_ERROR("Caught top-level exception: %s", e.what());
    }
#endif
  }

  return EXIT_SUCCESS;
}

App::App(sf::RenderWindow& app_window)
  :
  m_app_window{ app_window },
  m_state_machine{ NEW StateMachine("app_state_machine") },
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
    throw std::exception("Tried to create more than one App instance");
  }

  // Initialize SFGUI.
  m_sfgui.reset(NEW sfg::SFGUI());

  // Create the random number generator and seed it with the current time.
  m_rng.reset(NEW boost::random::mt19937(static_cast<unsigned int>(std::time(0))));

  // Create the default fonts.
  m_default_font.reset(NEW sf::Font());
  std::string font_name = "resources/fonts/" + Settings.get<std::string>("font_name_default") + ".ttf";
  if (m_default_font->loadFromFile(font_name) == false)
  {
    throw std::exception("Could not load the default font");
  }

  m_default_bold_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_bold") + ".ttf";
  if (m_default_bold_font->loadFromFile(font_name) == false)
  {
    throw std::exception("Could not load the default bold font");
  }

  m_default_mono_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_mono") + ".ttf";
  if (m_default_mono_font->loadFromFile(font_name) == false)
  {
    throw std::exception("Could not load the default monospace font");
  }

  m_default_unicode_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_unicode") + ".ttf";
  if (m_default_unicode_font->loadFromFile(font_name) == false)
  {
    throw std::exception("Could not load the default Unicode font");
  }

  // Create the shader program.
  m_shader.reset(NEW sf::Shader());
  if (m_shader->loadFromFile("resources/shaders/default.vert",
                             "resources/shaders/default.frag") == false)
  {
    throw std::exception("Could not load the default shaders");
  }

  // Create the message log.
  m_message_log.reset(NEW MessageLog());

  // Create the message log view.
  m_message_log_view.reset(NEW MessageLogView(*(m_message_log.get()),
                                              calc_message_log_dimensions(m_app_window)));

  // Register Lua functions.
  the_lua_instance.register_function("app_get_frame_counter", App::LUA_get_frame_counter);
  //the_lua_instance.register_function("print", App::LUA_redirect_print);
  the_lua_instance.register_function("messageLog_add", App::LUA_add);

  // Get the state machine.
  StateMachine& sm = *(m_state_machine.get());

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
  m_app_window.close();
  s_p_instance = nullptr;
}

EventResult App::handle_event(sf::Event& event)
{
  EventResult result = EventResult::Ignored;

  switch (event.type)
  {
    case sf::Event::EventType::GainedFocus:
    {
      m_has_window_focus = true;
      result = EventResult::Handled;
      break;
    }

    case sf::Event::EventType::LostFocus:
    {
      m_has_window_focus = false;
      result = EventResult::Handled;
      break;
    }

    case sf::Event::EventType::Resized:
    {
      m_app_window.setView(sf::View(
        sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
      the_message_log_view.set_dimensions(calc_message_log_dimensions(m_app_window));

      result = EventResult::Acknowledged;
      break;
    }

    case sf::Event::EventType::Closed:
    {
      m_is_running = false;
      result = EventResult::Handled;
      break;
    }

    case sf::Event::EventType::KeyPressed:
    {
      switch (event.key.code)
      {
        case sf::Keyboard::Key::Q:
          if (event.key.alt && event.key.control)
          {
            m_is_running = false;
            result = EventResult::Handled;
          }
          break;

        default:
          break;
      }
      break;
    }

    default:
      break;
  }

  if (result != EventResult::Handled)
  {
    result = m_state_machine->handle_event(event);
  }

  return result;
}

sf::RenderWindow& App::get_window()
{
  return m_app_window;
}

bool App::has_window_focus()
{
  return m_has_window_focus;
}

boost::random::mt19937 & App::get_rng()
{
  return *(m_rng.get());
}

sf::Font & App::get_default_font()
{
  return *(m_default_font.get());
}

sf::Font & App::get_default_bold_font()
{
  return *(m_default_bold_font.get());
}

sf::Font & App::get_default_mono_font()
{
  return *(m_default_mono_font.get());
}

sf::Font & App::get_default_unicode_font()
{
  return *(m_default_unicode_font.get());
}

sf::Shader & App::get_shader()
{
  return *(m_shader.get());
}

MessageLog & App::get_message_log()
{
  return *(m_message_log.get());
}

MessageLogView & App::get_message_log_view()
{
  return *(m_message_log_view.get());
}

App & App::instance()
{
  if (s_p_instance)
  {
    return *s_p_instance;
  }
  else
  {
    throw std::exception("App instance was requested before it was created");
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
    while (m_app_window.pollEvent(event))
    {
      handle_event(event);
    }

    m_state_machine->execute();

    // Limit frame rate to 60 fps.
    if (frame_clock.getElapsedTime().asMicroseconds() > 16667)
    {
      frame_clock.restart();
      m_app_window.clear();
      m_state_machine->render(m_app_window, s_frame_counter);
      m_app_window.display();
      ++s_frame_counter;
    }
  }
}

int App::LUA_get_frame_counter(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 0)
  {
    MINOR_ERROR("expected 0 arguments, got %d", num_args);
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
      the_message_log.add(str);
    }
    else {
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
    MINOR_ERROR("Expected 1 argument, got %d", num_args);
  }
  else
  {
    std::string str = lua_tostring(L, 1);
    the_message_log.add(str);
  }

  return 0;
}