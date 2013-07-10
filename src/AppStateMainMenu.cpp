#include "AppStateMainMenu.h"

#include "App.h"

struct AppStateMainMenu::Impl
{
  sf::Text title;
  sf::Text subtitle;
};

AppStateMainMenu::AppStateMainMenu(StateMachine* state_machine)
  : State(state_machine), impl(new Impl())
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
  target.draw(impl->title);
  target.draw(impl->subtitle);
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
  impl->title.setString("Hey y'all main menu in the hizzouse");
  impl->title.setFont(the_default_font);
  impl->subtitle.setString("But actually there's no main menu yet so just press Spacebar.");
  impl->subtitle.setFont(the_default_font);
  impl->subtitle.setCharacterSize(16);
  impl->subtitle.setPosition(0, 100);

  return true;
}

bool AppStateMainMenu::terminate()
{
  return true;
}
