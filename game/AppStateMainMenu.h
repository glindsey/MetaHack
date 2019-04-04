#ifndef APPSTATEMAINMENU_H
#define APPSTATEMAINMENU_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>

#include "game/AppState.h"

class AppStateMainMenu
  :
  public AppState
{
public:
  AppStateMainMenu(StateMachine& state_machine,
                   sf::RenderWindow& appWindow,
                   sfg::SFGUI& sfgui,
                   sfg::Desktop& desktop);

  virtual ~AppStateMainMenu();

  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

protected:
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual bool onEvent(Event const& event) override;

private:
  /// Application window.
  sf::RenderWindow& m_appWindow;

  /// The SFGUI instance.
  sfg::SFGUI& m_sfgui;

  /// The SFGUI desktop.
  sfg::Desktop& m_desktop;

  sf::Text m_title;
  sf::Text m_subtitle;
};

#endif // APPSTATEMAINMENU_H
