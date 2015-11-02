#include "AppStateGameMode.h"

#include <boost/log/trivial.hpp>
#include <memory>
#include <set>

#include "Action.h"
#include "App.h"
#include "ConfigSettings.h"
#include "GetLetterKey.h"
#include "KeyBuffer.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"

struct AppStateGameMode::Impl
{
  Impl()
  {
    map_zoom_level = 1.0f;
    current_input_state = GameInputState::Map;
    action_in_progress.type = Action::Type::None;

    inventory_area.reset(NEW InventoryArea(calc_inventory_dims()));

    status_area.reset(NEW StatusArea(calc_status_area_dims()));
  }

  /// Map the player is currently on.
  MapId current_map_id;

  /// True if the application window is in focus, false otherwise.
  bool window_in_focus;

  /// True if inventory window shows player inventory, false otherwise.
  bool inventory_area_shows_player;

  /// Instance of the player status area.
  std::unique_ptr<StatusArea> status_area;

  /// Instance of the inventory area.
  std::unique_ptr<InventoryArea> inventory_area;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  double map_zoom_level;

  /// Current screen area that has keyboard focus.
  GameInputState current_input_state;

  /// Current location of the cursor on the map.
  sf::Vector2i cursor_coords;

  /// Item that is the "object" of an action (if any).
  std::weak_ptr<Thing> object_thing;

  /// Action in progress.
  /// Used for an action that needs a "target".
  Action action_in_progress;

  void reset_inventory_area()
  {
    ThingRef player = TM.get_player();
    Map& game_map = MF.get(player->get_map_id());
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
    statusAreaDims.width = the_window.getSize().x -
                           (invAreaDims.width + 24);
    statusAreaDims.height = Settings.status_area_height;
    statusAreaDims.top = the_window.getSize().y -
                         (Settings.status_area_height + 5);
    statusAreaDims.left = 12;
    return statusAreaDims;
  }

  sf::IntRect calc_inventory_dims()
  {
    sf::IntRect messageLogDims = the_message_log.get_dimensions();
    sf::IntRect inventoryAreaDims;
    inventoryAreaDims.width = Settings.inventory_area_width;
    inventoryAreaDims.height = the_window.getSize().y -
                               (messageLogDims.height + 18);
    inventoryAreaDims.left = the_window.getSize().x -
                             (inventoryAreaDims.width + 3);
    inventoryAreaDims.top = messageLogDims.top + messageLogDims.height + 10;

    return inventoryAreaDims;
  }

  bool move_cursor(Direction direction)
  {
    Map& game_map = MF.get(current_map_id);
    bool result;

    result = game_map.calc_coords(cursor_coords, direction, cursor_coords);

    return result;
  }
};

AppStateGameMode::AppStateGameMode(StateMachine* state_machine)
  : State(state_machine), pImpl(NEW Impl())
{
}

AppStateGameMode::~AppStateGameMode()
{
  //dtor
}

void AppStateGameMode::execute()
{
  // First, check for debug commands ready to be run.
  KeyBuffer& debug_buffer = the_message_log.get_key_buffer();

  if (debug_buffer.get_enter())
  {
    /// @todo Implement debug command processing.
    the_message_log.add("** Sorry, debug command processing is not yet implemented.");
    debug_buffer.clear_buffer();
  }

  ThingRef player = TM.get_player();

  if (player->pending_action())
  {
    // QUESTION: Do we want to update all Things, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapId current_map_id = player->get_map_id();
    Map& current_map = MF.get(current_map_id);

    // Process everything on the map.
    current_map.process();
    pImpl->reset_inventory_area();

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      current_map.update_lighting();

      // Update tile vertex array.
      current_map.update_tile_vertices(player);
    }
  }
}

bool AppStateGameMode::render(sf::RenderTarget& target, int frame)
{
  // Set focus for areas.
  the_message_log.set_focus(pImpl->current_input_state == GameInputState::MessageLog);
  pImpl->status_area->set_focus(pImpl->current_input_state == GameInputState::Map);

  ThingRef player = TM.get_player();
  ThingRef location = player->get_location();

  if (location == TM.get_mu())
  {
    throw std::exception("Uh oh, the player's location appears to have been deleted!");
  }

  /// @todo We need a way to determine if the player is directly on a map,
  ///       and render either the map, or a container interior.
  ///       Should probably use an overridden "render_surroundings" method
  ///       for Things.

  if (!player->is_inside_another_thing())
  {
    MapTile* tile = player->get_maptile();
    if (tile != nullptr)
    {
      Map& game_map = MF.get(tile->get_map_id());
      sf::Vector2i tile_coords = tile->get_coords();
      sf::Vector2f player_pixel_coords = MapTile::get_pixel_coords(tile_coords);
      sf::Vector2f cursor_pixel_coords = MapTile::get_pixel_coords(pImpl->cursor_coords);

      // Update thing vertex array.
      game_map.update_thing_vertices(player, frame);

      if (pImpl->current_input_state == GameInputState::CursorLook)
      {
        game_map.set_view(target, cursor_pixel_coords, pImpl->map_zoom_level);
        game_map.draw_to(target);

        auto& cursor_tile = game_map.get_tile(pImpl->cursor_coords);
        cursor_tile.draw_highlight(target,
                                   cursor_pixel_coords,
                                   Settings.cursor_border_color,
                                   Settings.cursor_bg_color,
                                   frame);
      }
      else
      {
        game_map.set_view(target, player_pixel_coords, pImpl->map_zoom_level);
        game_map.draw_to(target);
      }
    }
  }

  the_message_log.render(target, frame);
  pImpl->status_area->render(target, frame);
  pImpl->inventory_area->render(target, frame);

  return true;
}

EventResult AppStateGameMode::handle_key_press(sf::Event::KeyEvent& key)
{
  EventResult result = EventResult::Ignored;
  ThingRef player = TM.get_player();

  // *** Handle keys processed in any mode.
  if (!key.alt && !key.control)
  {
    if (key.code == sf::Keyboard::Key::Tilde)
    {
      switch (pImpl->current_input_state)
      {
      case GameInputState::Map:
        pImpl->current_input_state = GameInputState::MessageLog;
        return EventResult::Handled;

      case GameInputState::MessageLog:
        pImpl->current_input_state = GameInputState::Map;
        return EventResult::Handled;

      default:
        break;
      }
    }
  }

  // *** Handle keys unique to a particular focus.
  switch (pImpl->current_input_state)
  {
  case GameInputState::TargetSelection:
  {
    int key_number = get_letter_key(key);
    Direction key_direction = get_direction_key(key);

    if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Tab)
    {
      pImpl->inventory_area_shows_player = !pImpl->inventory_area_shows_player;
      pImpl->reset_inventory_area();
    }

    if (pImpl->action_in_progress.target_can_be_thing)
    {
      if (!key.alt && !key.control && key_number != -1)
      {
        pImpl->action_in_progress.target =
          pImpl->inventory_area->get_thing(static_cast<InventorySlot>(key_number));
        player->queue_action(pImpl->action_in_progress);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        pImpl->current_input_state = GameInputState::Map;
        result = EventResult::Handled;
      }
    } // end if (pImpl->action_in_progress.target_can_be_thing)

    if (pImpl->action_in_progress.target_can_be_direction)
    {
      if (!key.alt && !key.control && key_direction != Direction::None)
      {
        pImpl->action_in_progress.direction = key_direction;
        player->queue_action(pImpl->action_in_progress);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        pImpl->current_input_state = GameInputState::Map;
        return EventResult::Handled;
      }
    }
  } // end case GameInputState::TargetSelection

  case GameInputState::CursorLook:
  {
    // *** NON-MODIFIED KEYS ***********************************************
    if (!key.alt && !key.control && !key.shift)
    {
      switch (key.code)
      {
      case sf::Keyboard::Key::Up:
        pImpl->move_cursor(Direction::North);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageUp:
        pImpl->move_cursor(Direction::Northeast);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Right:
        pImpl->move_cursor(Direction::East);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageDown:
        pImpl->move_cursor(Direction::Southeast);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Down:
        pImpl->move_cursor(Direction::South);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::End:
        pImpl->move_cursor(Direction::Southwest);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Left:
        pImpl->move_cursor(Direction::West);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Home:
        pImpl->move_cursor(Direction::Northwest);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // "/" - go back to Map focus.
      case sf::Keyboard::Key::Slash:
        pImpl->current_input_state = GameInputState::Map;
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      default:
        break;
      } // end switch (key.code)
    } // end if (no modifier keys)
    break;
  } // end case GameInputState::CursorLook

  case GameInputState::Map:
  {
    Action action;
    int key_number = get_letter_key(key);
    Direction key_direction = get_direction_key(key);

    // *** No ALT, no CTRL, shift is irrelevant ****************************
    if (!key.alt && !key.control)
    {
      if (key_number != -1)
      {
        pImpl->inventory_area->toggle_selection(static_cast<InventorySlot>(key_number));
        result = EventResult::Handled;
      }
      else if (key.code == sf::Keyboard::Key::Tab)
      {
        pImpl->inventory_area_shows_player = !pImpl->inventory_area_shows_player;
        pImpl->reset_inventory_area();
      }
    }

    // *** No ALT, no CTRL, no SHIFT ***************************************
    if (!key.alt && !key.control && !key.shift)
    {
      if (key_direction != Direction::None)
      {
        if (key_direction == Direction::Self)
        {
          action.type = Action::Type::Wait;
          player->queue_action(action);
          result = EventResult::Handled;
        }
        else
        {
          action.type = Action::Type::Move;
          action.direction = key_direction;
          player->queue_action(action);
          result = EventResult::Handled;
        }
      }
      else switch (key.code)
      {
      case sf::Keyboard::Key::BackSpace:
        pImpl->reset_inventory_area();
        break;

        // "/" - go to cursor look mode.
      case sf::Keyboard::Key::Slash:
        pImpl->current_input_state = GameInputState::CursorLook;
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // "-" - subtract quantity
      case sf::Keyboard::Key::Dash:
      case sf::Keyboard::Key::Subtract:
        {
          /// @todo Need a way to choose which inventory we're affecting.
          unsigned int slot_count = pImpl->inventory_area->get_selected_slot_count();
          if (slot_count < 1)
          {
            the_message_log.add("You have to have something selected "
                                "before you can choose a quantity.");
          }
          else if (slot_count > 1)
          {
            the_message_log.add("You can only have one thing selected "
                                "when choosing a quantity.");
          }
          else
          {
            pImpl->inventory_area->dec_selected_quantity();
          }
        }
        result = EventResult::Handled;
        break;

        // "+"/"=" - add quantity
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
        {
          unsigned int slot_count = pImpl->inventory_area->get_selected_slot_count();
          if (slot_count < 1)
          {
            the_message_log.add("You have to have something selected "
                                "before you can choose a quantity.");
          }
          else if (slot_count > 1)
          {
            the_message_log.add("You can only have one thing selected "
                                "when choosing a quantity.");
          }
          else
          {
            pImpl->inventory_area->inc_selected_quantity();
          }
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::LBracket:
        {
          ThingRef thing = pImpl->inventory_area->get_viewed();
          ThingRef location = thing->get_location();
          if (location != TM.get_mu())
          {
            pImpl->inventory_area->set_viewed(location);
          }
          else
          {
            the_message_log.add("You are at the top of the inventory tree.");
          }
        }
        break;

      case sf::Keyboard::Key::RBracket:
        {
          unsigned int slot_count = pImpl->inventory_area->get_selected_slot_count();

          if (slot_count > 0)
          {
            ThingRef thing = pImpl->inventory_area->get_selected_things().at(0);
            if (thing->get_intrinsic_value("inventory_size") != 0)
            {
              if (!thing->get_intrinsic_value("openable") || thing->get_property_flag("open"))
              {
                if (thing->get_intrinsic_value("lockable") && !thing->get_property_flag("locked"))
                {
                  pImpl->inventory_area->set_viewed(thing);
                }
                else // if (container.is_locked())
                {
                  the_message_log.add(thing->get_identifying_string() +
                                      thing->choose_verb(" are", " is") +
                                      " locked.");
                }
              }
              else // if (!container.is_open())
              {
                the_message_log.add(thing->get_identifying_string() +
                                    thing->choose_verb(" are", " is") +
                                    " closed.");
              }
            }
            else // if (!thing.is_container())
            {
              the_message_log.add(thing->get_identifying_string() +
                                  thing->choose_verb(" are", " is") +
                                  " not a container.");
            }
          }
          else
          {
            the_message_log.add("Nothing is currently selected.");
          }
          break;
        }

      case sf::Keyboard::Key::Comma:
        key.alt = false;
        key.control = true;
        key.shift = false;
        key.code = sf::Keyboard::Key::P;
        break;

      default:
        break;
      } // end switch (key.code)
    } // end if (!key.alt && !key.control && !key.shift)

    // *** No ALT, YES CTRL, SHIFT is irrelevant ******************************
    if (!key.alt && key.control)
    {
      switch (key.code)
      {
        // CTRL-MINUS -- Zoom out
      case sf::Keyboard::Key::Dash:
      case sf::Keyboard::Key::Subtract:
        add_zoom(-0.05);
        break;

        // CTRL-PLUS -- Zoom in
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
        add_zoom(0.05);
        break;

        // CTRL-D -- drop items
      case sf::Keyboard::Key::D:    // Drop
        action.type = Action::Type::Drop;
        action.things = pImpl->inventory_area->get_selected_things();
        player->queue_action(action);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // CTRL-P -- pick up items
      case sf::Keyboard::Key::P:    // Pick up
        action.type = Action::Type::Pickup;
        action.things = pImpl->inventory_area->get_selected_things();
        player->queue_action(action);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // CTRL-Q -- quaff (drink) items
      case sf::Keyboard::Key::Q:
        action.type = Action::Type::Quaff;
        action.things = pImpl->inventory_area->get_selected_things();
        player->queue_action(action);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // CTRL-S -- store item in another item
      case sf::Keyboard::Key::S:
        action.type = Action::Type::Store;
        action.things = pImpl->inventory_area->get_selected_things();
        if (action.things.size() > 0)
        {
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a container to store into.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->action_in_progress.target_can_be_thing = true;
          pImpl->inventory_area->clear_selected_slots();
        }
        else
        {
          the_message_log.add("You must first select the objects to store.");
        }
        result = EventResult::Handled;
        break;

        // CTRL-T -- take item out of container
      case sf::Keyboard::Key::T:
        action.type = Action::Type::TakeOut;
        action.things = pImpl->inventory_area->get_selected_things();
        player->queue_action(action);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

        // CTRL-W -- wield item
      case sf::Keyboard::Key::W:
        action.type = Action::Type::Wield;
        action.things = pImpl->inventory_area->get_selected_things();
        player->queue_action(action);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        result = EventResult::Handled;
        break;

      default:
        break;
      }
    } // end if (!key.alt && key.control)

    // *** YES ALT, no CTRL, SHIFT is irrelevant ******************************
    if (key.alt && !key.control)
    {
      switch (key.code)
      {
      default:
        break;
      }
    }

    // *** YES ALT, YES CTRL, SHIFT is irrelevant *****************************
    if (key.alt && key.control)
    {
      switch (key.code)
      {
      default:
        break;
      }
    }
    break;
  } // end case GameInputState::Map

  default:
    break;
  } // end switch (pImpl->current_input_state)

  return result;
}

EventResult AppStateGameMode::handle_mouse_wheel(sf::Event::MouseWheelEvent& wheel)
{
  add_zoom(wheel.delta * 0.05f);
  return EventResult::Handled;
}

void AppStateGameMode::add_zoom(double zoom_amount)
{
  double current_zoom_level = pImpl->map_zoom_level;

  current_zoom_level += zoom_amount;

  if (current_zoom_level < 0.1f)
  {
    current_zoom_level = 0.1f;
  }

  if (current_zoom_level > 3.0f)
  {
    current_zoom_level = 3.0f;
  }

  pImpl->map_zoom_level = current_zoom_level;
}

EventResult AppStateGameMode::handle_event(sf::Event& event)
{
  EventResult result = EventResult::Ignored;

  switch (event.type)
  {
  case sf::Event::EventType::Resized:
  {
    pImpl->inventory_area->set_dimensions(pImpl->calc_inventory_dims());
    pImpl->status_area->set_dimensions(pImpl->calc_status_area_dims());
    result = EventResult::Handled;
    break;
  }

  case sf::Event::EventType::KeyPressed:
    result = this->handle_key_press(event.key);
    break;

  case sf::Event::EventType::KeyReleased:
    break;

  case sf::Event::EventType::MouseWheelMoved:
    result = this->handle_mouse_wheel(event.mouseWheel);
    break;

  default:
    break;
  }

  if (result != EventResult::Handled)
  {
    switch (pImpl->current_input_state)
    {
    case GameInputState::Map:
      result = pImpl->status_area->handle_event(event);
      break;

    case GameInputState::MessageLog:
      result = the_message_log.handle_event(event);
      break;

    default:
      break;
    }
  }

  return result;
}

std::string const& AppStateGameMode::get_name()
{
  static std::string name = std::string("AppStateGameMode");
  return name;
}

bool AppStateGameMode::initialize()
{
  // Create the player.
  ThingRef player = TM.create("Human");
  player->set_proper_name("John Doe");
  TM.set_player(player);

  // Create the game map.
  pImpl->current_map_id = MF.create(64, 64);
  Map& game_map = MF.get(pImpl->current_map_id);

  // Move player to start position on the map.
  sf::Vector2i const& start_coords = game_map.get_start_coords();
  auto start = game_map.get_tile(start_coords).get_floor();
  bool player_moved = player->move_into(start);

  if (player_moved == false)
  {
    throw std::exception("Could not move player to start position!");
    return false;
  }

  // Set cursor to starting location.
  pImpl->cursor_coords = start_coords;

  // Set the viewed inventory location to the player's location.
  pImpl->inventory_area_shows_player = false;
  pImpl->reset_inventory_area();

  // TESTING CODE: Create a lighting orb held in player inventory.
  TRACE("Creating lighting orb...");
  ThingRef player_orb = TM.create("LightOrb");
  player_orb->move_into(player);

  // TESTING CODE: Create a sconce immediately north of the player.
  TRACE("Creating sconce...");
  ThingRef sconce = TM.create("Sconce");
  sconce->move_into(game_map.get_tile(start_coords.x, start_coords.y - 1).get_floor());

  // TESTING CODE: Create a rock immediately south of the player.
  TRACE("Creating rock...");
  ThingRef rock = TM.create("Rock");
  rock->move_into(game_map.get_tile(start_coords.x, start_coords.y + 1).get_floor());

  // TESTING CODE: Create a sack immediately east of the player.
  TRACE("Creating sack...");
  ThingRef sack = TM.create("SackCloth");
  sack->move_into(game_map.get_tile(start_coords.x + 1, start_coords.y).get_floor());

  // TESTING CODE: Create five gold coins west of the player.
  TRACE("Creating 5 coins...");
  ThingRef coins = TM.create("CoinGold");
  coins->set_quantity(5);
  coins->move_into(game_map.get_tile(start_coords.x - 1, start_coords.y).get_floor());

  // TESTING CODE: Create ten gold coins northwest of the player.
  TRACE("Creating 10 coins...");
  ThingRef coins2 = TM.create("CoinGold");
  coins2->set_quantity(10);
  coins2->move_into(game_map.get_tile(start_coords.x - 1, start_coords.y - 1).get_floor());

  // TESTING CODE: Create a rock lichen northeast of the player.
  TRACE("Creating rock lichen...");
  ThingRef lichen = TM.create("RockLichen");
  lichen->move_into(game_map.get_tile(start_coords.x + 1, start_coords.y - 1).get_floor());

  // END TESTING CODE

  // Get the map ready.
  game_map.update_lighting();
  game_map.update_tile_vertices(player);
  game_map.update_thing_vertices(player, 0);

  the_message_log.add("Welcome to the Etheric Catacombs!");

  return true;
}

bool AppStateGameMode::terminate()
{
  return true;
}
