#include "stdafx.h"

#include "game/AppVC.h"

#include "config/Paths.h"
#include "config/Settings.h"
#include "events/UIEvents.h"
#include "state_machine/StateMachine.h"
#include "tilesheet/TileSheet.h"

AppVC::AppVC(Config::Paths& paths,
             Config::Settings& settings,
             sf::RenderWindow& window)
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
    UIEvents::EventMouseWheelMoved::id
  }),
  m_appTexture{ NEW sf::RenderTexture() },
  m_appWindow(window)
{
  auto& resourcesPath = paths.resources();

  // Create the app texture for off-screen composition.
  m_appTexture->create(m_appWindow.getSize().x, m_appWindow.getSize().y);

  // Create the default fonts.
  m_fontDefault.reset(NEW sf::Font());
  std::string defaultFont = settings.get("font-name-default");
  FileName fontName = resourcesPath + "/font/" + defaultFont + ".ttf";
  if (m_fontDefault->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default font";
  }

  m_fontDefaultBold.reset(NEW sf::Font());
  std::string defaultBoldFont = settings.get("font-name-bold");
  fontName = resourcesPath + "/font/" + defaultBoldFont + ".ttf";
  if (m_fontDefaultBold->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default bold font";
  }

  m_fontDefaultMono.reset(NEW sf::Font());
  std::string defaultMonoFont = settings.get("font-name-mono");
  fontName = resourcesPath + "/font/" + defaultMonoFont + ".ttf";
  if (m_fontDefaultMono->loadFromFile(fontName) == false)
  {
    CLOG(FATAL, "App") << "Could not load the default monospace font";
  }

  m_fontDefaultUnicode.reset(NEW sf::Font());
  std::string defaultUnicodeFont = settings.get("font-name-unicode");
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

  // Create the tilesheet.
  auto tileSize = settings.get("graphics-tile-size");
  auto textureSize = settings.get("tilesheet-texture-size");
  m_tileSheet.reset(NEW TileSheet(tileSize, textureSize));

  // Start the rendering and GUI clocks running.
  m_renderClock.restart();
  m_guiClock.restart();
}

AppVC::~AppVC() {}

sf::Font& AppVC::fontDefault()
{
  return *m_fontDefault;
}

sf::Font& AppVC::fontDefaultBold()
{
  return *m_fontDefaultBold;
}

sf::Font& AppVC::fontDefaultMono()
{
  return *m_fontDefaultMono;
}

sf::Font& AppVC::fontDefaultUnicode()
{
  return *m_fontDefaultUnicode;
}

sf::Shader& AppVC::shader()
{
  return *m_shader;
}

TileSheet& AppVC::tileSheet()
{
  return *m_tileSheet;
}

int AppVC::frameCounter() const
{
  return m_frameCounter;
}

void AppVC::incrementFrameCounter()
{
  ++m_frameCounter;
}

void AppVC::drawEverything(sfg::Desktop& desktop,
                           sfg::SFGUI& sfgui,
                           StateMachine& stateMachine)
{
  // Update SFGUI with elapsed seconds since last call.
  desktop.Update(m_guiClock.restart().asSeconds());

  // Update frame counter if necessary.
  unsigned int elapsedFrameUsec =
    m_renderClock.getElapsedTime().asMicroseconds();

  // Limit to 60fps.
  if (elapsedFrameUsec >= 16667)
  {
    elapsedFrameUsec -= 16667;
    m_renderClock.restart();
    ++m_frameCounter;

    // Rendering...
    m_appWindow.clear();
    m_appTexture->clear(Color::Red);

    stateMachine.render(*m_appTexture, m_frameCounter);

    m_appTexture->display();
    sf::Sprite sprite(m_appTexture->getTexture());
    m_appWindow.draw(sprite);

    sfgui.Display(m_appWindow);

    m_appWindow.display();
  }
}

sf::RenderWindow& AppVC::window()
{
  return m_appWindow;
}

sf::RenderTexture& AppVC::texture()
{
  return *m_appTexture;
}

void AppVC::setTexture(sf::RenderTexture* texture)
{
  m_appTexture.reset(texture);
}
