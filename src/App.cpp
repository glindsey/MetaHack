#include "stdafx.h"

#include "App.h"

#include "AppStateGameMode.h"
#include "AppStateMainMenu.h"
#include "AppStateSplashScreen.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "MessageLog.h"
#include "MessageLogView.h"
#include "StateMachine.h"

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

App::App(sf::RenderWindow& app_window)
  :
  m_app_window{ app_window },
  m_app_texture{ NEW sf::RenderTexture() },
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
    throw std::runtime_error("Tried to create more than one App instance");
  }

  // Register the App logger.
  el::Loggers::getLogger("App");

  // Create the app texture for off-screen composition.
  m_app_texture->create(m_app_window.getSize().x, m_app_window.getSize().y);

  // Create the GUI desktop.
  m_gui_desktop.reset(NEW metagui::Desktop("Desktop", m_app_window.getSize()));

  // Create the random number generator and seed it with the current time.
  m_rng.reset(NEW boost::random::mt19937(static_cast<unsigned int>(std::time(0))));

  // Create the default fonts.
  m_default_font.reset(NEW sf::Font());
  FileName font_name = "resources/fonts/" + Settings.get<std::string>("font_name_default") + ".ttf";
  if (m_default_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default font";
  }

  m_default_bold_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_bold") + ".ttf";
  if (m_default_bold_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default bold font";
  }

  m_default_mono_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_mono") + ".ttf";
  if (m_default_mono_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default monospace font";
  }

  m_default_unicode_font.reset(NEW sf::Font());
  font_name = "resources/fonts/" + Settings.get<std::string>("font_name_unicode") + ".ttf";
  if (m_default_unicode_font->loadFromFile(font_name) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default Unicode font";
  }

  // Create the shader program.
  m_shader.reset(NEW sf::Shader());
  if (m_shader->loadFromFile("resources/shaders/default.vert",
                             "resources/shaders/default.frag") == false)
  {
    CLOG(FATAL, "App") << "Could not load the default shaders";
  }

  // Create the message log.
  m_message_log.reset(NEW MessageLog());

  // Register Lua functions.
  the_lua_instance.register_function("app_get_frame_counter", App::LUA_get_frame_counter);
  //the_lua_instance.register_function("print", App::LUA_redirect_print);
  the_lua_instance.register_function("messageLog_add", App::LUA_add);

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
  m_app_window.close();
  s_p_instance = nullptr;
}

SFMLEventResult App::handle_sfml_event(sf::Event& event)
{
  SFMLEventResult result = SFMLEventResult::Ignored;

  switch (event.type)
  {
    case sf::Event::EventType::GainedFocus:
    {
      m_has_window_focus = true;
      result = SFMLEventResult::Handled;
      break;
    }

    case sf::Event::EventType::LostFocus:
    {
      m_has_window_focus = false;
      result = SFMLEventResult::Handled;
      break;
    }

    case sf::Event::EventType::Resized:
    {
      m_app_texture.reset(NEW sf::RenderTexture());
      m_app_texture->create(event.size.width, event.size.height);
      m_app_window.setView(sf::View(
        sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

      result = SFMLEventResult::Acknowledged;
      break;
    }

    case sf::Event::EventType::Closed:
    {
      m_is_running = false;
      result = SFMLEventResult::Handled;
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
            result = SFMLEventResult::Handled;
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

  if (result != SFMLEventResult::Handled)
  {
    result = m_state_machine->handle_sfml_event(event);
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
    while (m_app_window.pollEvent(event))
    {
      handle_sfml_event(event);
    }

    m_state_machine->execute();

    // Limit frame rate to 60 fps.
    if (frame_clock.getElapsedTime().asMicroseconds() > 16667)
    {
      frame_clock.restart();
      m_app_window.clear();
      m_app_texture->clear(sf::Color::Red);

      m_state_machine->render(*m_app_texture, s_frame_counter);

      m_app_texture->display();
      sf::Sprite sprite(m_app_texture->getTexture());
      m_app_window.draw(sprite);

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
    CLOG(WARNING, "App") << "Expected 1 argument, got " << num_args;
  }
  else
  {
    std::string str = lua_tostring(L, 1);
    the_message_log.add(str);
  }

  return 0;
}