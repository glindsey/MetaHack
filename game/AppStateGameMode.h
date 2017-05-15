#pragma once

#include "stdafx.h"

#include "events/UIEvents.h"
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
class SystemManager;


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

  virtual std::string const& getName() override;
  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

  GameState& gameState();
  SystemManager& systems();

  std::unordered_set<EventID> registeredEvents() const override;

protected:
  void render_map(sf::RenderTexture& texture, int frame);
  bool handle_key_press(UIEvents::EventKeyPressed const& key);
  bool handle_mouse_wheel(UIEvents::EventMouseWheelMoved const& wheel);
  sf::IntRect calcMessageLogDims();
  void resetInventorySelection();
  sf::IntRect calcStatusAreaDims();
  sf::IntRect calcInventoryDims();
  bool moveCursor(Direction direction);
  bool handleKeyPressTargetSelection(EntityId player, UIEvents::EventKeyPressed const& key);
  bool handleKeyPressCursorLook(EntityId player, UIEvents::EventKeyPressed const& key);

  void add_zoom(float zoom_amount);

  virtual bool onEvent_V(Event const& event) override;

private:
  /// Application window.
  sf::RenderWindow& m_appWindow;

  /// The debug key buffer.
  std::unique_ptr<KeyBuffer> m_debugBuffer;

  /// The current game state.
  std::unique_ptr<GameState> m_gameState;

  /// The manager of all game systems.
  std::unique_ptr<SystemManager> m_systemManager;

  /// Reference to the current MapView.
  MapView* m_mapView;
  //std::unique_ptr<MapView> m_mapView;

  /// The current InventorySelection.
  std::unique_ptr<InventorySelection> m_inventorySelection;

  /// True if the application window is in focus, false otherwise.
  bool m_windowInFocus;

  /// True if inventory window shows player inventory, false otherwise.
  bool m_inventoryAreaShowsPlayer;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  float m_mapZoomLevel;

  /// Current screen area that has keyboard focus.
  GameInputState m_currentInputState;

  /// Current location of the cursor on the map.
  IntVec2 m_cursorCoords;

  /// Action in progress (if any).
  /// Used for an action that needs a "target".
  std::unique_ptr<Actions::Action> m_actionInProgress;
};
