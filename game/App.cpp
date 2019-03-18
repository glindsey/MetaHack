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
#include "services/Service.h"
#include "services/MessageLog.h"
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
  m_appWindow{ appWindow },
  m_sfgui{ sfgui },
  m_desktop{ desktop },
  m_appTexture{ NEW sf::RenderTexture() },
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

  auto& resourcesPath = Config::paths().resources();

  // Create the app state machine.
  m_stateMachine.reset(NEW StateMachine("app_state_machine", this)),

  // Create the app texture for off-screen composition.
  m_appTexture->create(m_appWindow.getSize().x, m_appWindow.getSize().y);

  // Create the GUI desktop.
  m_guiDesktop.reset(NEW metagui::Desktop(*this, "Desktop", m_appWindow.getSize()));

  // Create the default fonts.
  m_fontDefault.reset(NEW sf::Font());
  std::string defaultFont = config.get("font-name-default");
  FileName fontName = resourcesPath + "/font/" + defaultFont + ".ttf";
  if (m_fontDefault->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default font";
  }

  m_fontDefaultBold.reset(NEW sf::Font());
  std::string defaultBoldFont = config.get("font-name-bold");
  fontName = resourcesPath + "/font/" + defaultBoldFont + ".ttf";
  if (m_fontDefaultBold->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default bold font";
  }

  m_fontDefaultMono.reset(NEW sf::Font());
  std::string defaultMonoFont = config.get("font-name-mono");
  fontName = resourcesPath + "/font/" + defaultMonoFont + ".ttf";
  if (m_fontDefaultMono->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default monospace font";
  }

  m_fontDefaultUnicode.reset(NEW sf::Font());
  std::string defaultUnicodeFont = config.get("font-name-unicode");
  fontName = resourcesPath + "/font/" + defaultUnicodeFont + ".ttf";
  if (m_fontDefaultUnicode->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default Unicode font";
  }

  // Create the shader program.
  m_shader.reset(NEW sf::Shader());
  if (m_shader->loadFromFile(resourcesPath + "/shader/default.vert",
                             resourcesPath + "/shader/default.frag") == false)
  {
    CLOG(FATAL, "App") << "Could not load the default shaders";
  }

  // Create the message log.
  Service<IMessageLog>::provide(NEW MessageLog());

  // Create the string dictionary, and try to load the default translation file.
  /// @todo Change this so language can be specified.
  Config::strings().loadFile(resourcesPath + "/strings.en");

  /// @note Standard map views provider creation has been moved to AppStateGameMode.

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
  m_guiDesktop.reset();
  m_appWindow.close();
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
      m_appTexture.reset(NEW sf::RenderTexture());
      m_appTexture->create(sfmlEvent.size.width, sfmlEvent.size.height);
      m_appWindow.setView(sf::View(
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
  return m_appWindow;
}

bool App::hasWindowFocus()
{
  return m_hasWindowFocus;
}

sf::Font & App::fontDefault()
{
  return *m_fontDefault;
}

sf::Font & App::fontDefaultBold()
{
  return *m_fontDefaultBold;
}

sf::Font & App::fontDefaultMono()
{
  return *m_fontDefaultMono;
}

sf::Font & App::fontDefaultUnicode()
{
  return *m_fontDefaultUnicode;
}

sf::Shader & App::shader()
{
  return *m_shader;
}

metagui::Desktop & App::guiDesktop()
{
  return *m_guiDesktop;
}

TileSheet & App::tileSheet()
{
  return *m_tileSheet;
}

int App::frameCounter() const
{
  return m_frameCounter;
}

App & App::instance()
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
    while (m_appWindow.pollEvent(event))
    {
      m_desktop.HandleEvent(event);

      handleSFMLEvent(event);
    }

    m_stateMachine->execute();

    // Update SFGUI with elapsed seconds since last call.
    m_desktop.Update(guiClock.restart().asSeconds());

    // Update frame counter if necessary.
    unsigned int elapsedFrameUsec = frameClock.getElapsedTime().asMicroseconds();

    // Limit to 60fps.
    if (elapsedFrameUsec >= 16667)
    {
      elapsedFrameUsec -= 16667;
      frameClock.restart();
      ++m_frameCounter;
    }

    // Rendering...
    m_appWindow.clear();
    m_appTexture->clear(Color::Red);

    m_stateMachine->render(*m_appTexture, m_frameCounter);

    m_appTexture->display();
    sf::Sprite sprite(m_appTexture->getTexture());
    m_appWindow.draw(sprite);

    m_sfgui.Display(m_appWindow);

    m_appWindow.display();
  }
}
