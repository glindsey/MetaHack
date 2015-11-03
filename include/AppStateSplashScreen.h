#ifndef APPSTATESPLASHSCREEN_H
#define APPSTATESPLASHSCREEN_H

#include <memory>

#include "State.h"

class AppStateSplashScreen : public State
{
  public:
    AppStateSplashScreen(StateMachine& state_machine, sf::RenderWindow& app_window);
    virtual ~AppStateSplashScreen();

    void execute() override;
    bool render(sf::RenderTarget& target, int frame) override;
    EventResult handle_event(sf::Event& event) override;
    std::string const& get_name() override;
    bool initialize() override;
    bool terminate() override;

  protected:
  private:
    sf::Texture m_splash_graphic;
    sf::Sprite m_splash_sprite;
    sf::Clock m_clock;
};

#endif // APPSTATESPLASHSCREEN_H
