#ifndef APP_H
#define APP_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>

#include "Object.h"
#include "types/common.h"

// Forward declarations
class AppVC;
class ConfigSettings;
class MessageLogView;
class StateMachine;
class Strings;
class TileSheet;

/// Class that defines the overall application.
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

  /// Get the view controller.
  /// @todo Remove this -- eventually there should be no need for other code to
  ///       directly access the app VC. For now, though, it's necessary so
  ///       certain VC members can be accessed.
  AppVC& vc();

  /// Get the current frame counter.
  int frameCounter() const;

  /// Get the current App instance.
  /// If no App instance currently exists, throws an exception.
  static App& instance();

  static sf::Font& the_default_font();
  static sf::Font& the_default_bold_font();
  static sf::Font& the_default_mono_font();
  static sf::Font& the_default_unicode_font();
  static sf::Shader& the_shader();
  static TileSheet& the_tilesheet();

private:
  /// The App view-controller.
  std::unique_ptr<AppVC> m_vc;

  sfg::SFGUI& m_sfgui;
  sfg::Desktop& m_desktop;

  /// The state machine.
  std::unique_ptr<StateMachine> m_stateMachine;
  bool m_isRunning;
  bool m_hasWindowFocus;

  /// The rendering frame counter.
  int m_frameCounter;

  /// A static pointer to the existing App instance.
  static App* s_instance;

};

#endif // APP_H
