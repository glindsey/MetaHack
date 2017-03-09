#ifndef APPSTATEGAMEMODE_H
#define APPSTATEGAMEMODE_H

#include "stdafx.h"

#include "AppState.h"

// Forward declarations
class Action;
class Direction;
class EntityId;
class GameState;
class InventorySelection;
class KeyBuffer;
class MapView;


/// Object that represents the game mode state.
/// Anything about the game that needs to be saved should go into the
/// GameState class (e.g. data model members). Data view members which
/// do not get saved should go here.
class AppStateGameMode
  :
  public AppState
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

  virtual SFMLEventResult handle_sfml_event(sf::Event& event) override;

  virtual std::string const& getName() override;
  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

  GameState& get_game_state();

protected:
  void render_map(sf::RenderTexture& texture, int frame);
  SFMLEventResult handle_key_press(sf::Event::KeyEvent& key);
  SFMLEventResult handle_mouse_wheel(sf::Event::MouseWheelEvent& wheel);
  sf::IntRect calc_message_log_dims();
  void reset_inventory_selection();
  sf::IntRect calc_status_area_dims();
  sf::IntRect calc_inventory_dims();
  bool move_cursor(Direction direction);
  SFMLEventResult handle_key_press_target_selection(EntityId player, sf::Event::KeyEvent& key);
  SFMLEventResult handle_key_press_cursor_look(EntityId player, sf::Event::KeyEvent& key);

  void add_zoom(float zoom_amount);

private:
  /// Application window.
  sf::RenderWindow& m_app_window;

  /// The debug key buffer.
  std::unique_ptr<KeyBuffer> m_debug_buffer;

  /// The current game state.
  std::unique_ptr<GameState> m_game_state;

  /// The current MapView.
  std::unique_ptr<MapView> m_map_view;

  /// The current InventorySelection.
  std::unique_ptr<InventorySelection> m_inventory_selection;

  /// True if the application window is in focus, false otherwise.
  bool m_window_in_focus;

  /// True if inventory window shows player inventory, false otherwise.
  bool m_inventory_area_shows_player;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  float m_map_zoom_level;

  /// Current screen area that has keyboard focus.
  GameInputState m_current_input_state;

  /// Current location of the cursor on the map.
  Vec2i m_cursor_coords;

  /// Action in progress (if any).
  /// Used for an action that needs a "target".
  std::unique_ptr<Action> m_action_in_progress;
};

#endif // APPSTATEGAMEMODE_H
