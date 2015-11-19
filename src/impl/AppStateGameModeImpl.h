#ifndef APPSTATEGAMEMODEIMPL_H
#define APPSTATEGAMEMODEIMPL_H

#include "SFML/Graphics.hpp"

#include "Action.h"
#include "ConfigSettings.h"
#include "GameState.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "New.h"

/// Current input state for the game.
enum class GameInputState
{
  Map,
  TargetSelection,
  CursorLook,
  MessageLog
};

class AppStateGameModeImpl
{
public:
  /// Constructor.
  AppStateGameModeImpl(sf::RenderWindow& app_window_)
    :
    app_window{ app_window_ },
    game_state{ NEW GameState() },
    window_in_focus{ true },
    inventory_area_shows_player{ false },
    inventory_area{ NEW InventoryArea(calc_inventory_dims()) },
    status_area{ NEW StatusArea(calc_status_area_dims()) },
    map_zoom_level{ 1.0f },
    current_input_state{ GameInputState::Map },
    cursor_coords{ 0, 0 },
    action_in_progress{ Action::Type::None }
  {}

  /// Application window.
  sf::RenderWindow& app_window;

  /// The current game state.
  std::unique_ptr<GameState> game_state;

  /// True if the application window is in focus, false otherwise.
  bool window_in_focus;

  /// True if inventory window shows player inventory, false otherwise.
  bool inventory_area_shows_player;

  /// Instance of the inventory area.
  std::unique_ptr<InventoryArea> inventory_area;

  /// Instance of the player status area.
  std::unique_ptr<StatusArea> status_area;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  float map_zoom_level;

  /// Current screen area that has keyboard focus.
  GameInputState current_input_state;

  /// Current location of the cursor on the map.
  sf::Vector2i cursor_coords;

  /// Action in progress.
  /// Used for an action that needs a "target".
  Action action_in_progress;

  /// Game clock.
  uint64_t game_clock;

  void reset_inventory_area()
  {
    ThingRef player = game_state->get_thing_manager().get_player();
    Map& game_map = GAME.get_map_factory().get(player->get_map_id());
    if (inventory_area_shows_player == true)
    {
      inventory_area->set_viewed(player);
    }
    else
    {
      if (current_input_state == GameInputState::CursorLook)
      {
        ThingRef floor_id = game_map.get_tile(cursor_coords).get_floor();
        inventory_area->set_viewed(floor_id);
      }
      else
      {
        inventory_area->set_viewed(player->get_location());
      }
    }
  }

  sf::IntRect calc_status_area_dims()
  {
    sf::IntRect statusAreaDims;
    sf::IntRect invAreaDims = inventory_area->get_dimensions();
    statusAreaDims.width = app_window.getSize().x -
      (invAreaDims.width + 24);
    statusAreaDims.height = Settings.get<int>("status_area_height");
    statusAreaDims.top = app_window.getSize().y - (Settings.get<int>("status_area_height") + 5);
    statusAreaDims.left = 12;
    return statusAreaDims;
  }

  sf::IntRect calc_inventory_dims()
  {
    sf::IntRect messageLogDims = the_message_log.get_dimensions();
    sf::IntRect inventoryAreaDims;
    inventoryAreaDims.width = Settings.get<int>("inventory_area_width");
    inventoryAreaDims.height = app_window.getSize().y - 10;
    inventoryAreaDims.left = app_window.getSize().x - (inventoryAreaDims.width + 3);
    inventoryAreaDims.top = 5;

    return inventoryAreaDims;
  }

  bool move_cursor(Direction direction)
  {
    ThingRef player = game_state->get_thing_manager().get_player();
    Map& game_map = GAME.get_map_factory().get(player->get_map_id());
    bool result;

    result = game_map.calc_coords(cursor_coords, direction, cursor_coords);

    return result;
  }
};

#endif // APPSTATEGAMEMODEIMPL_H
