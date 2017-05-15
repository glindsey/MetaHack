#ifndef APPSTATEMAINMENU_H
#define APPSTATEMAINMENU_H

#include "stdafx.h"

#include "game/AppState.h"

class AppStateMainMenu
  :
  public AppState
{
public:
  AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window);
  virtual ~AppStateMainMenu();

  virtual std::string const& getName() override;
  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

protected:
  void render_title(sf::RenderTexture& texture, int frame);

  virtual bool onEvent_V(Event const& event) override;

private:
  sf::Text m_title;
  sf::Text m_subtitle;
};

#endif // APPSTATEMAINMENU_H
