#ifndef APPSTATEMAINMENU_H
#define APPSTATEMAINMENU_H

#include "stdafx.h"

#include "State.h"

#include "GUIDesktop.h"

class AppStateMainMenu
  :
  public State
{
public:
  AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window);
  virtual ~AppStateMainMenu();

  void execute() override;
  bool render(sf::RenderTexture& texture, int frame) override;
  SFMLEventResult handle_sfml_event(sf::Event& event) override;
  StringKey const& get_name() override;
  bool initialize() override;
  bool terminate() override;

protected:
  void render_title(sf::RenderTexture& texture, int frame);

private:
  sf::Text m_title;
  sf::Text m_subtitle;
};

#endif // APPSTATEMAINMENU_H
