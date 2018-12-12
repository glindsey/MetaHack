#include "stdafx.h"

#include "game/AppStateMainMenu.h"

#include "game/App.h"

AppStateMainMenu::AppStateMainMenu(StateMachine& stateMachine,
                                   sf::RenderWindow& appWindow,
                                   sfg::SFGUI& sfgui,
                                   sfg::Desktop& desktop)
  :
  AppState(stateMachine, {},
           "AppStateMainMenu",
           std::bind(&AppStateMainMenu::render_title, this, std::placeholders::_1, std::placeholders::_2)),
  m_appWindow{ appWindow },
  m_sfgui{ sfgui },
  m_desktop{ desktop }
{
}

AppStateMainMenu::~AppStateMainMenu()
{
}

void AppStateMainMenu::execute()
{}

//SFMLEventResult AppStateMainMenu::handleSFMLEvent(sf::Event& event)
//{
//  SFMLEventResult result = the_desktop.handleSFMLEvent(event);
//
//  if (result != SFMLEventResult::Handled)
//  {
//    switch (event.type)
//    {
//      case sf::Event::EventType::KeyPressed:
//        switch (event.key.code)
//        {
//          case sf::Keyboard::Key::Space:
//            // Switch to game state.
//            this->change_to("AppStateGameMode");
//
//            result = SFMLEventResult::Handled;
//            break;
//
//          default:
//            break;
//        }
//        break;
//
//      default:
//        break;
//    }
//  }
//
//  return result;
//}

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

// === PROTECTED METHODS ======================================================

void AppStateMainMenu::render_title(sf::RenderTexture & texture, int frame)
{
  texture.draw(m_title);
  texture.draw(m_subtitle);
  texture.display();
}

bool AppStateMainMenu::onEvent(Event const& event)
{
  /// @todo WRITE ME
  return false;
}
