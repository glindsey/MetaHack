#ifndef APP_H
#define APP_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>

#include "Object.h"
#include "types/common.h"

// Forward declarations
class ConfigSettings;
class MessageLogView;
class StateMachine;
class Strings;
class TileSheet;

/// Class that defines the overall application.
/// @todo Possibly split all the rendering-specific stuff out into a separate
///       object. Not entirely sure what the way to go is here.
class App : public Object
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

  explicit App(sf::RenderWindow& appWindow,
               sfg::SFGUI& sfgui,
               sfg::Desktop& desktop);
  App(App const&) = delete;
  App(App&&) = delete;
  App& operator=(App const&) = delete;
  App& operator=(App&&) = delete;
  virtual ~App();

  void run();

  void handleSFMLEvent(sf::Event& sfmlEvent);

  sf::RenderWindow& renderWindow();

  bool hasWindowFocus();

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

  /// Get the current App instance.
  /// If no App instance currently exists, throws an exception.
  static App& instance();

protected:

private:
  sf::RenderWindow& m_appWindow;
  sfg::SFGUI& m_sfgui;
  sfg::Desktop& m_desktop;

  /// Pointer to off-screen buffer for drawing composition.
  std::unique_ptr<sf::RenderTexture> m_appTexture;

  /// The state machine.
  std::unique_ptr<StateMachine> m_stateMachine;
  bool m_isRunning;
  bool m_hasWindowFocus;

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

  /// The rendering frame counter.
  int m_frameCounter;

  /// A static pointer to the existing App instance.
  static App* s_instance;

};

// Here are a few macros to save on typing.
#define the_default_font          (App::instance().fontDefault())
#define the_default_bold_font     (App::instance().fontDefaultBold())
#define the_default_mono_font     (App::instance().fontDefaultMono())
#define the_default_unicode_font  (App::instance().fontDefaultUnicode())
#define the_shader                (App::instance().shader())
#define the_tilesheet             (App::instance().tileSheet())

#endif // APP_H
