#ifndef APPSTATESPLASHSCREEN_H
#define APPSTATESPLASHSCREEN_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>

#include "game/AppState.h"

class AppStateSplashScreen
  :
  public AppState
{
public:
  AppStateSplashScreen(StateMachine& state_machine,
                       sf::RenderWindow& appWindow,
                       sfg::SFGUI& sfgui,
                       sfg::Desktop& desktop);

  virtual ~AppStateSplashScreen();

  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

protected:
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual bool onEvent(Event const& event) override;

private:
  /// Application window.
  sf::RenderWindow& m_appWindow;

  /// The SFGUI instance.
  sfg::SFGUI& m_sfgui;

  /// The SFGUI desktop.
  sfg::Desktop& m_desktop;

  sf::Texture m_splash_graphic;
  sf::Sprite m_splash_sprite;
  sf::Clock m_clock;
};

#endif // APPSTATESPLASHSCREEN_H
