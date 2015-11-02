#include "AppStateMainMenu.h"

#include "App.h"

struct AppStateMainMenu::Impl
{
  sf::Text title;
  sf::Text subtitle;
};

AppStateMainMenu::AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window)
  : State(state_machine), pImpl(NEW Impl())
{
}

AppStateMainMenu::~AppStateMainMenu()
{
}

void AppStateMainMenu::execute()
{
}

bool AppStateMainMenu::render(sf::RenderTarget& target, int frame)
{
  target.draw(pImpl->title);
  target.draw(pImpl->subtitle);
  return true;
}

EventResult AppStateMainMenu::handle_event(sf::Event& event)
{
  switch (event.type)
  {
  case sf::Event::EventType::KeyPressed:
    switch (event.key.code)
    {
    case sf::Keyboard::Key::Space:
      // Switch to game state.
      this->change_to("AppStateGameMode");

      return EventResult::Handled;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return EventResult::Ignored;
}

std::string const& AppStateMainMenu::get_name()
{
  static std::string name = std::string("AppStateMainMenu");
  return name;
}

bool AppStateMainMenu::initialize()
{
  pImpl->title.setString("Hey y'all main menu in the hizzouse");
  pImpl->title.setFont(the_default_font);
  pImpl->subtitle.setString("But actually there's no main menu yet so just press Spacebar.");
  pImpl->subtitle.setFont(the_default_font);
  pImpl->subtitle.setCharacterSize(16);
  pImpl->subtitle.setPosition(0, 100);

  return true;
}

bool AppStateMainMenu::terminate()
{
  return true;
}
