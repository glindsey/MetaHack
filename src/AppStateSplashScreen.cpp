#include "stdafx.h"

#include "AppStateSplashScreen.h"

#include "App.h"

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

StringKey const& AppStateSplashScreen::get_name()
{
  static StringKey name = "AppStateSplashScreen";
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