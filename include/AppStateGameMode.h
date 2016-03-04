#ifndef APPSTATEGAMEMODE_H
#define APPSTATEGAMEMODE_H

#include "stdafx.h"

#include "EventHandler.h"
#include "GameState.h"
#include "State.h"

/// Object that represents the game mode state.
/// Anything about the game that needs to be saved should go into the
/// GameState class (e.g. data model members). Data view members which
/// do not get saved should go here.
class AppStateGameMode : public State
{
public:
  AppStateGameMode(StateMachine& state_machine, sf::RenderWindow& app_window);
  virtual ~AppStateGameMode();

  void execute() override;
  bool render(sf::RenderTarget& target, int frame) override;
  EventResult handle_event(sf::Event& event) override;
  std::string const& get_name() override;
  bool initialize() override;
  bool terminate() override;

  GameState& get_game_state();

protected:
  EventResult handle_key_press(sf::Event::KeyEvent& key);
  EventResult handle_mouse_wheel(sf::Event::MouseWheelEvent& wheel);

  void add_zoom(float zoom_amount);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // APPSTATEGAMEMODE_H
