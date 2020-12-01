#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>

#include "Object.h"
#include "types/common.h"

// Forward declarations
namespace Config
{
  class Paths;
  class Settings;
}
class StateMachine;
class TileSheet;

/// View-Controller for the application.
class AppVC : public Object
{
public:
  struct EventAppQuitRequested : public ConcreteEvent<EventAppQuitRequested>
  {
    EventAppQuitRequested()
    {}

    void printToStream(std::ostream& os) const
    {
      Event::printToStream(os);
    }
  };

  struct EventAppWindowResized : public ConcreteEvent<EventAppWindowResized>
  {
    EventAppWindowResized(UintVec2 newSize_)
      :
      newSize(newSize_)
    {}

    UintVec2 const newSize;

    void printToStream(std::ostream& os) const
    {
      Event::printToStream(os);
      os << " | new size: " << newSize;
    }
  };

  struct EventAppWindowClosed : public ConcreteEvent<EventAppWindowClosed>
  {
    EventAppWindowClosed()
    {}

    void printToStream(std::ostream& os) const
    {
      Event::printToStream(os);
    }
  };

  struct EventAppWindowFocusChanged : public ConcreteEvent<EventAppWindowFocusChanged>
  {
    EventAppWindowFocusChanged(bool focused_)
      :
      focused{ focused_ }
    {}

    bool const focused;

    void printToStream(std::ostream& os) const
    {
      Event::printToStream(os);
      os << " | focused: " << (focused ? "true" : "false");
    }
  };

  explicit AppVC(Config::Paths& paths,
                 Config::Settings& settings,
                 sf::RenderWindow& window);
  AppVC(AppVC const&) = delete;
  AppVC(AppVC&&) = delete;
  AppVC& operator=(AppVC const&) = delete;
  AppVC& operator=(AppVC&&) = delete;
  virtual ~AppVC();

  /// Get the default font.
  sf::Font& fontDefault();

  /// Get the default bold font.
  sf::Font& fontDefaultBold();

  ///Get the default monospace font.
  sf::Font& fontDefaultMono();

  /// Get the default Unicode font.
  sf::Font& fontDefaultUnicode();

  /// Get the shader.
  sf::Shader& shader();

  /// Get the TileSheet instance.
  TileSheet& tileSheet();

  /// Get the current frame counter.
  int frameCounter() const;

  /// Draw to the screen (if it is time to do so).
  void drawEverything(sfg::Desktop& desktop,
                      sfg::SFGUI& sfgui,
                      StateMachine& stateMachine);

  /// @todo All below methods should eventually be removed once VC logic is
  ///       moved here.

  /// Increment the current frame counter.
  void incrementFrameCounter();

  /// The app window.
  sf::RenderWindow& window();

  /// The app render texture.
  sf::RenderTexture& texture();

  /// Set the app render texture.
  void setTexture(sf::RenderTexture* texture);

protected:

private:
  /// Reference to the app window to draw to.
  sf::RenderWindow& m_appWindow;

  /// Pointer to off-screen buffer for drawing composition.
  std::unique_ptr<sf::RenderTexture> m_appTexture;

  /// The default font instance.
  std::unique_ptr<sf::Font> m_fontDefault;

  /// The default bold font instance.
  std::unique_ptr<sf::Font> m_fontDefaultBold;

  /// The default monospace font instance.
  std::unique_ptr<sf::Font> m_fontDefaultMono;

  /// The default Unicode font instance.
  std::unique_ptr<sf::Font> m_fontDefaultUnicode;

  /// The shader instance.
  std::unique_ptr<sf::Shader> m_shader;

  /// The tilesheet.
  std::unique_ptr<TileSheet> m_tileSheet;

  /// Clock used for adjusting render speed.
  sf::Clock m_renderClock;

  /// Clock used for the GUI.
  sf::Clock m_guiClock;

  /// The rendering frame counter.
  int m_frameCounter;
};
