#ifndef APPSTATEMAINMENU_H
#define APPSTATEMAINMENU_H

#include <memory>

#include "State.h"
#include "AppStateMainMenuImpl.h"


class AppStateMainMenu : public State
{
  public:
    AppStateMainMenu(StateMachine& state_machine, sf::RenderWindow& app_window);
    virtual ~AppStateMainMenu();

    void execute() override;
    bool render(sf::RenderTarget& target, int frame) override;
    EventResult handle_event(sf::Event& event) override;
    std::string const& get_name() override;
    bool initialize() override;
    bool terminate() override;

  protected:
  private:
    sf::Text m_title;
    sf::Text m_subtitle;
};

#endif // APPSTATEMAINMENU_H
