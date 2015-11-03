#include "App.h"

#include <boost/random/uniform_int_distribution.hpp>
#include <ctime>
#include <SFML/Graphics.hpp>

#include "AppStateGameMode.h"
#include "AppStateMainMenu.h"
#include "AppStateSplashScreen.h"
#include "ErrorHandler.h"
#include "MessageLog.h"
#include "StateMachine.h"
#include "ThingManager.h"
#include "TileSheet.h"

#include <stdlib.h>
#include <crtdbg.h>

// Global declarations
std::unique_ptr<sf::Font> default_font_;
std::unique_ptr<sf::Font> default_bold_font_;
std::unique_ptr<sf::Font> default_mono_font_;
std::unique_ptr<sf::Font> default_unicode_font_;
std::unique_ptr<sf::Shader> shader_;
std::unique_ptr<App> app_;
std::unique_ptr<boost::random::mt19937> rng_;
int App::s_frame_counter = 0;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

sf::IntRect calc_message_log_dimensions(sf::RenderWindow& window)
{
  sf::IntRect messageLogDims;
  messageLogDims.width = window.getSize().x - 6;
  messageLogDims.height = static_cast<int>(window.getSize().y * 0.2f) - 6;
  messageLogDims.left = 3;
  messageLogDims.top = 3;
  return messageLogDims;
}

int main()
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 

  // Scoping everything here so it is properly destroyed before checking for memory leaks.
  {
    std::unique_ptr<sf::RenderWindow> app_window;
    std::unique_ptr<App> app;

    try
    {
      // Create the random number generator.
      rng_.reset(NEW boost::random::mt19937(static_cast<unsigned int>(std::time(0))));

      // Check to make sure shaders are available.
      if (!sf::Shader::isAvailable())
      {
        throw std::exception("Shaders are not available on this platform");
      }

      // Create and open the main window.
      app_window.reset(NEW sf::RenderWindow(sf::VideoMode(1066, 600), "Magicule Saga"));

      // Create the default fonts.
      /// @todo Font names should be moved into ConfigSettings.
      default_font_.reset(NEW sf::Font());
      if (default_font_->loadFromFile("resources/fonts/berylium rg.ttf") == false)
      {
        throw std::exception("Could not load the default font (berylium rg.ttf)");
      }

      default_bold_font_.reset(NEW sf::Font());
      if (default_bold_font_->loadFromFile("resources/fonts/berylium bd.ttf") == false)
      {
        throw std::exception("Could not load the default bold font (berylium bd.ttf)");
      }

      default_mono_font_.reset(NEW sf::Font());
      if (default_mono_font_->loadFromFile("resources/fonts/DejaVuSansMono.ttf") == false)
      {
        throw std::exception("Could not load the default monospace font (DejaVuSansMono.ttf)");
      }

      default_unicode_font_.reset(NEW sf::Font());
      if (default_unicode_font_->loadFromFile("resources/fonts/FreeMono.ttf") == false)
      {
        throw std::exception("Could not load the default Unicode font (FreeMono.ttf)");
      }

      // Create the shader program.
      shader_.reset(NEW sf::Shader());
      if (shader_->loadFromFile("resources/shaders/default.vert",
        "resources/shaders/default.frag") == false)
      {
        throw std::exception("Could not load the shaders");
      }

      // Create the message log.
      MessageLog::create(calc_message_log_dimensions(*(app_window.get())));

      // Create and run the app instance.
      app.reset(NEW App(*(app_window.get())));
      app->run();
    }
    catch (std::exception& e)
    {
      FATAL_ERROR("Caught top-level exception: %s", e.what());
    }
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
  // Register Lua functions.
  the_lua_instance.register_function("app_get_frame_counter", App::LUA_get_frame_counter);

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
      the_message_log.set_dimensions(calc_message_log_dimensions(m_app_window));

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
