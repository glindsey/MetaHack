#include "stdafx.h"

#include "AppStateMainMenu.h"

#include "App.h"

AppStateMainMenu::AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window)
  :
  State{ state_machine }
{}

AppStateMainMenu::~AppStateMainMenu()
{}

void AppStateMainMenu::execute()
{}

bool AppStateMainMenu::render(sf::RenderTarget& target, int frame)
{
  target.draw(m_title);
  target.draw(m_subtitle);
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
  m_title.setString("Hey y'all main menu in the hizzouse");
  m_title.setFont(the_default_font);
  m_subtitle.setString("But actually there's no main menu yet so just press Spacebar.");
  m_subtitle.setFont(the_default_font);
  m_subtitle.setCharacterSize(16);
  m_subtitle.setPosition(0, 100);

  return true;
}

bool AppStateMainMenu::terminate()
{
  return true;
}