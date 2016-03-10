#include "stdafx.h"

#include "AppStateMainMenu.h"

#include "App.h"

AppStateMainMenu::AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window)
  :
  State{ state_machine },
  m_desktop{ "mainMenuDesktop", app_window.getSize() }
{
}

AppStateMainMenu::~AppStateMainMenu()
{}

void AppStateMainMenu::execute()
{}

bool AppStateMainMenu::render(sf::RenderTexture& texture, int frame)
{
  m_desktop.render(texture, frame);

  texture.draw(m_title);
  texture.draw(m_subtitle);

  texture.display();
  return true;
}

SFMLEventResult AppStateMainMenu::handle_sfml_event(sf::Event& event)
{
  /// @todo This is ugly, fix later
  SFMLEventResult result = static_cast<SFMLEventResult>(m_desktop.handle_sfml_event(event));

  if (result != SFMLEventResult::Handled)
  {
    switch (event.type)
    {
      case sf::Event::EventType::KeyPressed:
        switch (event.key.code)
        {
          case sf::Keyboard::Key::Space:
            // Switch to game state.
            this->change_to("AppStateGameMode");

            result = SFMLEventResult::Handled;
            break;

          default:
            break;
        }
        break;

      default:
        break;
    }
  }

  return result;
}

StringKey const& AppStateMainMenu::get_name()
{
  static StringKey name = "AppStateMainMenu";
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