#include "AppStateSplashScreen.h"

#include "App.h"

struct AppStateSplashScreen::Impl
{
  sf::Texture splash_graphic;
  sf::Sprite splash_sprite;
  sf::Clock clock;
};

AppStateSplashScreen::AppStateSplashScreen(StateMachine* state_machine)
  : State(state_machine), pImpl(NEW Impl())
{
  //ctor
}

AppStateSplashScreen::~AppStateSplashScreen()
{
  //dtor
}

void AppStateSplashScreen::execute()
{
  if (pImpl->clock.getElapsedTime().asSeconds() >= 1 /*3*/)
  {
    this->change_to("AppStateMainMenu");
  }
}

bool AppStateSplashScreen::render(sf::RenderTarget& target, int frame)
{
  target.draw(pImpl->splash_sprite);
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
  if (!pImpl->splash_graphic.loadFromFile("cb.bmp"))
  {
    return false;
  }

  pImpl->splash_sprite.setTexture(pImpl->splash_graphic);
  pImpl->clock.restart();
  return true;
}

bool AppStateSplashScreen::terminate()
{
  return true;
}
