#ifndef APPSTATEGAMEMODE_H
#define APPSTATEGAMEMODE_H

#include "stdafx.h"

#include "Action.h"
#include "Direction.h"
#include "GameState.h"
#include "InventoryArea.h"
#include "SFMLEventHandler.h"
#include "State.h"
#include "StatusArea.h"

#include "gui/GUIDesktopPane.h"

/// Object that represents the game mode state.
/// Anything about the game that needs to be saved should go into the
/// GameState class (e.g. data model members). Data view members which
/// do not get saved should go here.
class AppStateGameMode : public State
{
  /// Current input state for the game.
  enum class GameInputState
  {
    Map,
    TargetSelection,
    CursorLook,
    MessageLog
  };

public:
  AppStateGameMode(StateMachine& state_machine, sf::RenderWindow& app_window);
  virtual ~AppStateGameMode();

  void execute() override;
  bool render(sf::RenderTexture& texture, int frame) override;
  SFMLEventResult handle_sfml_event(sf::Event& event) override;
  StringKey const& get_name() override;
  bool initialize() override;
  bool terminate() override;

  GameState& get_game_state();

protected:
  void render_map(sf::RenderTexture& texture, int frame);
  SFMLEventResult handle_key_press(sf::Event::KeyEvent& key);
  SFMLEventResult handle_mouse_wheel(sf::Event::MouseWheelEvent& wheel);
  sf::IntRect calc_message_log_dims();
  void reset_inventory_area();
  sf::IntRect calc_status_area_dims();
  sf::IntRect calc_inventory_dims();
  bool move_cursor(Direction direction);
  SFMLEventResult handle_key_press_target_selection(ThingRef player, sf::Event::KeyEvent& key);
  SFMLEventResult handle_key_press_cursor_look(ThingRef player, sf::Event::KeyEvent& key);

  void add_zoom(float zoom_amount);

private:
  /// Application window.
  sf::RenderWindow& m_app_window;

  /// The desktop GUI pane.
  metagui::DesktopPane m_desktop;

  /// The current game state.
  std::unique_ptr<GameState> m_game_state;

  /// True if the application window is in focus, false otherwise.
  bool m_window_in_focus;

  /// True if inventory window shows player inventory, false otherwise.
  bool m_inventory_area_shows_player;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  float m_map_zoom_level;

  /// Current screen area that has keyboard focus.
  GameInputState m_current_input_state;

  /// Current location of the cursor on the map.
  sf::Vector2i m_cursor_coords;

  /// Action in progress (if any).
  /// Used for an action that needs a "target".
  std::unique_ptr<Action> m_action_in_progress;
};

#endif // APPSTATEGAMEMODE_H
