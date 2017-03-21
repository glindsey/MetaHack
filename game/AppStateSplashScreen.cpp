#include "stdafx.h"

#include "game/AppStateSplashScreen.h"

#include "game/App.h"

AppStateSplashScreen::AppStateSplashScreen(StateMachine& state_machine, sf::RenderWindow& app_window)
  :
  AppState(state_machine,
           std::bind(&AppStateSplashScreen::render_splash, this, std::placeholders::_1, std::placeholders::_2))
{
}

AppStateSplashScreen::~AppStateSplashScreen()
{
}

void AppStateSplashScreen::execute()
{
  if (m_clock.getElapsedTime().asSeconds() >= 1 /*3*/)
  {
    this->change_to("AppStateMainMenu");
  }
}

SFMLEventResult AppStateSplashScreen::handle_sfml_event(sf::Event& event)
{
  return SFMLEventResult::Ignored;
}

std::string const& AppStateSplashScreen::getName()
{
  static std::string name = "AppStateSplashScreen";
  return name;
}

bool AppStateSplashScreen::initialize()
{
  if (m_splash_graphic.loadFromFile("splash.bmp"))
  {
    m_splash_sprite.setTexture(m_splash_graphic);
  }

  m_clock.restart();
  return true;
}

bool AppStateSplashScreen::terminate()
{
  return true;
}

void AppStateSplashScreen::render_splash(sf::RenderTexture& texture, int frame)
{
  texture.draw(m_splash_sprite);
  texture.display();
}

bool AppStateSplashScreen::onEvent_(Event const& event)
{
  /// @todo WRITE ME
  return true;
}
