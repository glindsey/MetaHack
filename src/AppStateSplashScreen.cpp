#include "AppStateSplashScreen.h"

#include "App.h"

AppStateSplashScreen::AppStateSplashScreen(StateMachine& state_machine, sf::RenderWindow& app_window)
  : 
  State(state_machine)
{}

AppStateSplashScreen::~AppStateSplashScreen()
{}

void AppStateSplashScreen::execute()
{
  if (m_clock.getElapsedTime().asSeconds() >= 1 /*3*/)
  {
    this->change_to("AppStateMainMenu");
  }
}

bool AppStateSplashScreen::render(sf::RenderTarget& target, int frame)
{
  target.draw(m_splash_sprite);
  return true;
}

EventResult AppStateSplashScreen::handle_event(sf::Event& event)
{
  return EventResult::Ignored;
}

std::string const& AppStateSplashScreen::get_name()
{
  static std::string name = std::string("AppStateSplashScreen");
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
