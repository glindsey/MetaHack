#include "AppStateSplashScreen.h"

struct AppStateSplashScreen::Impl
{
  sf::Texture splash_graphic;
  sf::Sprite splash_sprite;
  sf::Clock clock;
};

AppStateSplashScreen::AppStateSplashScreen(StateMachine* state_machine)
  : State(state_machine), impl(new Impl())
{
  //ctor
}

AppStateSplashScreen::~AppStateSplashScreen()
{
  //dtor
}

void AppStateSplashScreen::execute()
{
  if (impl->clock.getElapsedTime().asSeconds() >= 1 /*3*/)
  {
    this->change_to("AppStateMainMenu");
  }
}

bool AppStateSplashScreen::render(sf::RenderTarget& target, int frame)
{
  target.draw(impl->splash_sprite);
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
  if (!impl->splash_graphic.loadFromFile("cb.bmp"))
  {
    return false;
  }

  impl->splash_sprite.setTexture(impl->splash_graphic);
  impl->clock.restart();
  return true;
}

bool AppStateSplashScreen::terminate()
{
  return true;
}
