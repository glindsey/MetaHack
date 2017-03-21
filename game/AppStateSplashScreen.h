#ifndef APPSTATESPLASHSCREEN_H
#define APPSTATESPLASHSCREEN_H

#include "stdafx.h"

#include "game/AppState.h"

class AppStateSplashScreen
  :
  public AppState
{
public:
  AppStateSplashScreen(StateMachine& state_machine, sf::RenderWindow& app_window);
  virtual ~AppStateSplashScreen();

  virtual SFMLEventResult handle_sfml_event(sf::Event& event) override;

  virtual std::string const& getName() override;
  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

protected:
  void render_splash(sf::RenderTexture& texture, int frame);

  virtual bool onEvent_(Event const& event) override;

private:
  sf::Texture m_splash_graphic;
  sf::Sprite m_splash_sprite;
  sf::Clock m_clock;
};

#endif // APPSTATESPLASHSCREEN_H
