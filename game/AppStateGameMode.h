#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>

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
class InventoryArea;
class InventorySelection;
class MapView;
class StatusArea;

namespace Systems
{
  class Manager;
}


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
  AppStateGameMode(StateMachine& state_machine,
                   sf::RenderWindow& appWindow,
                   sfg::SFGUI& sfgui,
                   sfg::Desktop& desktop);
  virtual ~AppStateGameMode();

  virtual bool initialize() override;
  virtual void execute() override;
  virtual bool terminate() override;

  GameState& gameState();
  Systems::Manager& systems();

protected:
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  bool handle_key_press(UIEvents::EventKeyPressed const& key);
  bool handle_mouse_wheel(UIEvents::EventMouseWheelMoved const& wheel);
  //sf::IntRect calcMessageLogDims();
  void resetInventorySelection();
  //sf::IntRect calcStatusAreaDims();
  //sf::IntRect calcInventoryDims();
  bool moveCursor(Direction direction);
  bool handleKeyPressTargetSelection(EntityId player, UIEvents::EventKeyPressed const& key);
  bool handleKeyPressCursorLook(EntityId player, UIEvents::EventKeyPressed const& key);

  void add_zoom(float zoom_amount);

  virtual bool onEvent(Event const& event) override;

private:
  /// Application window.
  sf::RenderWindow& m_appWindow;

  /// The SFGUI instance.
  sfg::SFGUI& m_sfgui;

  /// The SFGUI desktop.
  sfg::Desktop& m_desktop;

  /// The MessageLogView instance.
  std::unique_ptr<MessageLogView> m_messageLogView;

  /// The InventoryArea instance.
  std::unique_ptr<InventoryArea> m_inventoryArea;

  /// The StatusArea instance.
  std::unique_ptr<StatusArea> m_statusArea;

  /// The tilesheet.
  std::unique_ptr<TileSheet> m_tileSheet;

  /// The current game state.
  std::unique_ptr<GameState> m_gameState;

  /// The manager of all game systems.
  std::unique_ptr<Systems::Manager> m_systemManager;

  /// The current MapView.
  std::unique_ptr<MapView> m_mapView;

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
