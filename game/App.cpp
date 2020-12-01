#include "stdafx.h"

#include "game/App.h"

#include "config/Bible.h"
#include "config/Paths.h"
#include "config/Settings.h"
#include "config/Strings.h"
#include "events/UIEvents.h"
#include "game/AppStateGameMode.h"
#include "game/AppStateMainMenu.h"
#include "game/AppStateSplashScreen.h"
#include "game/AppVC.h"
#include "state_machine/StateMachine.h"
#include "tilesheet/TileSheet.h"
#include "types/Color.h"
#include "game_windows/MessageLogView.h"

// Global declarations
App* App::s_instance;

sf::IntRect calc_message_log_dimensions(sf::RenderWindow& window)
{
  sf::IntRect messageLogDims;
  auto& config = Config::settings();

  int inventory_area_width = config.get("inventory-area-width");
  int messagelog_area_height = config.get("messagelog-area-height");
  messageLogDims.width = window.getSize().x - (inventory_area_width + 24);
  messageLogDims.height = messagelog_area_height - 10;
  messageLogDims.left = 12;
  messageLogDims.top = 5;
  return messageLogDims;
}

App::App(sf::RenderWindow& appWindow, sfg::SFGUI& sfgui, sfg::Desktop& desktop)
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
  m_sfgui{ sfgui },
  m_desktop{ desktop },
  m_isRunning{ false },
  m_hasWindowFocus{ false }
{
  // Set the static instance pointer.
  if (!s_instance)
  {
    s_instance = this;
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
  SET_UP_LOGGER("Director",           false);
  SET_UP_LOGGER("Entity",             false);
  SET_UP_LOGGER("EntityFactory",      false);
  SET_UP_LOGGER("EventSystem",        false);
  SET_UP_LOGGER("Game",               true);
  SET_UP_LOGGER("GameRules",          false);
  SET_UP_LOGGER("GameState",          false);
  SET_UP_LOGGER("Geometry",           false);
  SET_UP_LOGGER("GUI",                true);
  SET_UP_LOGGER("Inventory",          false);
  SET_UP_LOGGER("InventoryArea",      true);
  SET_UP_LOGGER("InventorySelection", false);
  SET_UP_LOGGER("Lighting",           false);
  SET_UP_LOGGER("Lua",                false);
  SET_UP_LOGGER("Map",                false);
  SET_UP_LOGGER("MapFactory",         false);
  SET_UP_LOGGER("MapGenerator",       false);
  SET_UP_LOGGER("Object",             true);
  SET_UP_LOGGER("Narrator",           false);
  SET_UP_LOGGER("PlayerHandler",      false);
  SET_UP_LOGGER("Property",           false);
  SET_UP_LOGGER("PropertyDictionary", false);
  SET_UP_LOGGER("SenseSight",         false);
  SET_UP_LOGGER("StateMachine",       false);
  SET_UP_LOGGER("Strings",            false);
  SET_UP_LOGGER("StringTransforms",   false);
  SET_UP_LOGGER("Systems",            true);
  SET_UP_LOGGER("TileSheet",          false);
  SET_UP_LOGGER("Types",              false);
  SET_UP_LOGGER("Utilities",          false);

  auto& config = Config::settings();
  auto& paths = Config::paths();
  auto& resourcesPath = paths.resources();

  // Create the view controller.
  m_vc.reset(NEW AppVC(paths, config, appWindow));

  // Create the app state machine.
  m_stateMachine.reset(NEW StateMachine("app_state_machine", this)),

  // Create the string dictionary, and try to load the default translation file.
  /// @todo Change this so language can be specified.
  Config::strings().loadFile(resourcesPath + "/strings.en");

  // Get the state machine.
  StateMachine& sm = *m_stateMachine;

  // Add states to the state machine.
  sm.add_state(NEW AppStateSplashScreen(sm, appWindow, sfgui, desktop));
  sm.add_state(NEW AppStateMainMenu(sm, appWindow, sfgui, desktop));
  sm.add_state(NEW AppStateGameMode(sm, appWindow, sfgui, desktop));

  // Switch to initial state.
  // DEBUG: Go right to game mode for now.
  //sm->change_to("AppStateSplashScreen");
  sm.change_to("AppStateGameMode");

  // Set "window has focus" boolean to true.
  m_hasWindowFocus = true;
}

App::~App()
{
  m_stateMachine.reset();
  m_vc->window().close();
  s_instance = nullptr;
}

void App::handleSFMLEvent(sf::Event& sfmlEvent)
{
  switch (sfmlEvent.type)
  {
    case sf::Event::EventType::GainedFocus:
    {
      m_hasWindowFocus = true;
      EventAppWindowFocusChanged event(true);
      broadcast(event);
      break;
    }

    case sf::Event::EventType::LostFocus:
    {
      m_hasWindowFocus = false;
      EventAppWindowFocusChanged event(false);
      broadcast(event);
      break;
    }

    case sf::Event::EventType::Resized:
    {
      m_vc->setTexture(NEW sf::RenderTexture());
      m_vc->texture().create(sfmlEvent.size.width, sfmlEvent.size.height);
      m_vc->window().setView(sf::View(
        sf::FloatRect(0, 0,
                      static_cast<float>(sfmlEvent.size.width),
                      static_cast<float>(sfmlEvent.size.height))));
      EventAppWindowResized event({ sfmlEvent.size.width, sfmlEvent.size.height });
      broadcast(event);
      break;
    }

    case sf::Event::EventType::Closed:
    {
      m_isRunning = false;
      EventAppWindowClosed event;
      broadcast(event);
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
            m_isRunning = false;
            EventAppQuitRequested event;
            broadcast(event);
            do_key_broadcast = false;
          }
          break;

        default:
          break;
      }

      if (do_key_broadcast)
      {
        UIEvents::EventKeyPressed event(sfmlEvent.key.code,
                                        sfmlEvent.key.alt,
                                        sfmlEvent.key.control,
                                        sfmlEvent.key.shift,
                                        sfmlEvent.key.system);
        broadcast(event);
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
      UIEvents::EventMouseWheelMoved event(sfmlEvent.mouseWheel.delta,
                                           sfmlEvent.mouseWheel.x,
                                           sfmlEvent.mouseWheel.y);
      broadcast(event);
      break;
    }

    default:
      break;
  }
}

sf::RenderWindow& App::renderWindow()
{
  return m_vc->window();
}

bool App::hasWindowFocus()
{
  return m_hasWindowFocus;
}

AppVC& App::vc()
{
  return *m_vc;
}

int App::frameCounter() const
{
  return m_frameCounter;
}

App& App::instance()
{
  if (s_instance)
  {
    return *s_instance;
  }
  else
  {
    throw std::runtime_error("App instance was requested, but it does not exist");
  }
}

sf::Font& App::the_default_font()
{
  return instance().vc().fontDefault();
}

sf::Font& App::the_default_bold_font()
{
  return instance().vc().fontDefaultBold();
}

sf::Font& App::the_default_mono_font()
{
  return instance().vc().fontDefaultMono();
}

sf::Font& App::the_default_unicode_font()
{
  return instance().vc().fontDefaultUnicode();
}

sf::Shader& App::the_shader()
{
  return instance().vc().shader();
}

TileSheet& App::the_tilesheet()
{
  return instance().vc().tileSheet();
}

void App::run()
{
  static sf::Clock frameClock;
  static sf::Clock guiClock;

  // Set running boolean.
  m_isRunning = true;

  frameClock.restart();
  guiClock.restart();

  // Start the loop
  while (m_isRunning)
  {
    // Process events
    sf::Event event;
    while (m_vc->window().pollEvent(event))
    {
      m_desktop.HandleEvent(event);

      handleSFMLEvent(event);
    }

    m_stateMachine->execute();

    m_vc->drawEverything(m_desktop, m_sfgui, *m_stateMachine);
  }
}
