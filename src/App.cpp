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

// Global declarations
std::unique_ptr<sf::RenderWindow> app_window_;
std::unique_ptr<sf::Font> default_font_;
std::unique_ptr<sf::Font> default_bold_font_;
std::unique_ptr<sf::Font> default_mono_font_;
std::unique_ptr<sf::Shader> shader_;
std::unique_ptr<App> app_;
std::unique_ptr<boost::random::mt19937> rng_;
std::unique_ptr<TileSheet> tile_sheet_;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

sf::IntRect calc_message_log_dimensions()
{
  sf::IntRect messageLogDims;
  messageLogDims.width = the_window.getSize().x - 6;
  messageLogDims.height = static_cast<int>(the_window.getSize().y * 0.2f) - 6;
  messageLogDims.left = 3;
  messageLogDims.top = 3;
  return messageLogDims;
}

int main()
{
  try
  {
    // Create the random number generator.
    rng_.reset(new boost::random::mt19937(static_cast<unsigned int>(std::time(0))));

    // Check to make sure shaders are available.
    if (!sf::Shader::isAvailable())
    {
      throw std::exception("Shaders are not available on this platform");
    }

    // Create and open the main window.
    app_window_.reset(new sf::RenderWindow(sf::VideoMode(1066, 600), "Magicule Saga"));

    // Create the default fonts.
    /// @todo Font names should be moved into ConfigSettings.
    default_font_.reset(new sf::Font());
    if (default_font_->loadFromFile("resources/fonts/berylium rg.ttf") == false)
    {
      throw std::exception("Could not load the default font (berylium rg.ttf)");
    }

    default_bold_font_.reset(new sf::Font());
    if (default_bold_font_->loadFromFile("resources/fonts/berylium bd.ttf") == false)
    {
      throw std::exception("Could not load the default bold font (berylium bd.ttf)");
    }

    default_mono_font_.reset(new sf::Font());
    if (default_mono_font_->loadFromFile("resources/fonts/DejaVuSansMono.ttf") == false)
    {
      throw std::exception("Could not load the default monospace font (DejaVuSansMono.ttf)");
    }

    // Create the shader program.
    shader_.reset(new sf::Shader());
    if (shader_->loadFromFile("resources/shaders/default.vert",
      "resources/shaders/default.frag") == false)
    {
      throw std::exception("Could not load the shaders");
    }

    // Create the message log.
    MessageLog::create(calc_message_log_dimensions());

    // Create the tile sheet.
    tile_sheet_.reset(new TileSheet());
    tile_sheet_->load("resources/graphics/tilesheet.png");

    // Run a Lua test.
    the_lua_instance.do_file("test.lua");

    // Populate Lua enums.
    ActionResult_add_to_lua();
    Gender_add_to_lua();

    // Create and run the app instance.
    app_.reset(new App());
    app_->run();

    // Close the app window.
    app_window_->close();
  }
  catch (std::exception& e)
  {
    FATAL_ERROR("Caught top-level exception: %s", e.what());
  }

  return EXIT_SUCCESS;
}

struct App::Impl
{
    std::unique_ptr<StateMachine> state_machine;
    bool is_running;
    bool has_window_focus;
};

App::App()
  : pImpl(new Impl())
{
  // Create the main state machine.
  pImpl->state_machine.reset(new StateMachine("state_machine"));

  StateMachine* sm = pImpl->state_machine.get();
  // Add states to the state machine.
  pImpl->state_machine->add_state(new AppStateSplashScreen(sm));
  pImpl->state_machine->add_state(new AppStateMainMenu(sm));
  pImpl->state_machine->add_state(new AppStateGameMode(sm));

  // Switch to initial state.
  // DEBUG: Go right to game mode for now.
  //pImpl->state_machine->change_to("AppStateSplashScreen");
  pImpl->state_machine->change_to("AppStateGameMode");

  // Set "window has focus" boolean to true.
  pImpl->has_window_focus = true;
}

App::~App()
{
}

EventResult App::handle_event(sf::Event& event)
{
  EventResult result = EventResult::Ignored;

  switch (event.type)
  {
  case sf::Event::EventType::GainedFocus:
    {
      pImpl->has_window_focus = true;
      result = EventResult::Handled;
      break;
    }

  case sf::Event::EventType::LostFocus:
    {
      pImpl->has_window_focus = false;
      result = EventResult::Handled;
      break;
    }

  case sf::Event::EventType::Resized:
    {
      app_window_->setView(sf::View(sf::FloatRect(0, 0,
                                                 static_cast<float>(event.size.width),
                                                 static_cast<float>(event.size.height))));
      the_message_log.set_dimensions(calc_message_log_dimensions());

      result = EventResult::Acknowledged;
      break;
    }

  case sf::Event::EventType::Closed:
    {
      pImpl->is_running = false;
      app_window_->setView(sf::View(sf::FloatRect(0, 0,
												  static_cast<float>(event.size.width),
												  static_cast<float>(event.size.height))));
      result = EventResult::Handled;
      break;
    }

  case sf::Event::EventType::KeyPressed:
    {
      switch (event.key.code)
      {
      case sf::Keyboard::Key::Escape:
        pImpl->is_running = false;
        result = EventResult::Handled;
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
      result = pImpl->state_machine->handle_event(event);
  }

  return result;
}

bool App::has_window_focus()
{
  return pImpl->has_window_focus;
}

void App::run()
{
  int frame_counter = 0;
  static sf::Clock clock;

  // Set running boolean.
  pImpl->is_running = true;

  clock.restart();

  // Start the loop
  while (pImpl->is_running)
  {
    // Process events
    sf::Event event;
    while (app_window_->pollEvent(event))
    {
      handle_event(event);
    }

    pImpl->state_machine->execute();

    // Limit frame rate to 62.5 fps.
    if (clock.getElapsedTime().asMilliseconds() > 16)
    {
      clock.restart();
      the_window.clear();
      pImpl->state_machine->render(the_window, frame_counter);
      the_window.display();
      ++frame_counter;
    }
  }
}

int App::get_rand(int minimum, int maximum)
{
   uniform_int_dist dist(minimum, maximum);
   return dist(the_RNG);
}
