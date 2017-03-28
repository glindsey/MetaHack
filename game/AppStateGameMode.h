#ifndef APPSTATEGAMEMODE_H
#define APPSTATEGAMEMODE_H

#include "stdafx.h"

#include "game/App.h"
#include "game/AppState.h"

// Forward declarations
namespace Actions
{
  class Action;
}
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

  std::unordered_set<EventID> registeredEvents() const override;

protected:
  void render_map(sf::RenderTexture& texture, int frame);
  bool handle_key_press(App::EventKeyPressed const& key);
  bool handle_mouse_wheel(App::EventMouseWheelMoved const& wheel);
  sf::IntRect calcMessageLogDims();
  void resetInventorySelection();
  sf::IntRect calcStatusAreaDims();
  sf::IntRect calcInventoryDims();
  bool moveCursor(Direction direction);
  bool handleKeyPressTargetSelection(EntityId player, App::EventKeyPressed const& key);
  bool handleKeyPressCursorLook(EntityId player, App::EventKeyPressed const& key);

  void add_zoom(float zoom_amount);

  virtual EventResult onEvent_NVI(Event const& event) override;

private:
  /// Application window.
  sf::RenderWindow& m_app_window;

  /// The debug key buffer.
  std::unique_ptr<KeyBuffer> m_debug_buffer;

  /// The current game state.
  std::unique_ptr<GameState> m_game_state;

  /// Reference to the current MapView.
  MapView* m_map_view;
  //std::unique_ptr<MapView> m_map_view;

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
  IntVec2 m_cursor_coords;

  /// Action in progress (if any).
  /// Used for an action that needs a "target".
  std::unique_ptr<Actions::Action> m_action_in_progress;
};

#endif // APPSTATEGAMEMODE_H
