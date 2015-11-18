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

AppStateGameMode::AppStateGameMode(StateMachine& state_machine, sf::RenderWindow& app_window)
  : 
  State{ state_machine },
  pImpl(app_window)
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
    /// Call the Lua interpreter with the command.
    std::string contents = debug_buffer.get_buffer();
    the_message_log.add("> " + contents);
    if(luaL_dostring(the_lua_state, contents.c_str()))
    {
      the_message_log.add(lua_tostring(the_lua_state, -1));
    }

    debug_buffer.clear_buffer();
  }

  ThingRef player = TM.get_player();

  if (player->action_is_pending())
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
                                   Settings.get<sf::Color>("cursor_border_color"),
                                   Settings.get<sf::Color>("cursor_bg_color"),
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

    if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Escape)
    {
      the_message_log.add("Aborted.");
      pImpl->inventory_area_shows_player = false;
      pImpl->reset_inventory_area();
      pImpl->current_input_state = GameInputState::Map;
      result = EventResult::Handled;
    }

    if (pImpl->action_in_progress.target_can_be_thing())
    {
      if (!key.alt && !key.control && key_number != -1)
      {
        pImpl->action_in_progress.set_target(
          pImpl->inventory_area->get_thing(static_cast<InventorySlot>(key_number)));
        player->queue_action(pImpl->action_in_progress);
        pImpl->inventory_area_shows_player = false;
        pImpl->reset_inventory_area();
        pImpl->current_input_state = GameInputState::Map;
        result = EventResult::Handled;
      }
    } // end if (pImpl->action_in_progress.target_can_be_thing)

    if (pImpl->action_in_progress.target_can_be_direction())
    {
      if (!key.alt && !key.control && key_direction != Direction::None)
      {
        pImpl->action_in_progress.set_target(key_direction);
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
        result = EventResult::Handled;
      }
      else if (key.code == sf::Keyboard::Key::Escape)
      {
        the_message_log.add("Press CTRL-ALT-Q to quit the game.");
        result = EventResult::Handled;
      }
    }

    // *** No ALT, no CTRL, no SHIFT ***************************************
    if (!key.alt && !key.control && !key.shift)
    {
      if (key_direction != Direction::None)
      {
        if (key_direction == Direction::Self)
        {
          action.set_type(Action::Type::Wait);
          player->queue_action(action);
          result = EventResult::Handled;
        }
        else
        {
          action.set_type(Action::Type::Move);
          action.set_target(key_direction);
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
            if (thing->get_intrinsic<int>("inventory_size") != 0)
            {
              if (!thing->get_intrinsic<bool>("openable") || thing->get_property<bool>("open"))
              {
                if (!thing->get_intrinsic<bool>("lockable") || !thing->get_property<bool>("locked"))
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
        /// @todo Find a better way to do this. This is hacky.
        key.alt = false;
        key.control = true;
        key.shift = false;
        key.code = sf::Keyboard::Key::G;
        break;

      default:
        break;
      } // end switch (key.code)
    } // end if (!key.alt && !key.control && !key.shift)

    // *** No ALT, YES CTRL, SHIFT is irrelevant ******************************
    if (!key.alt && key.control)
    {
      action.set_things(pImpl->inventory_area->get_selected_things());

      switch (key.code)
      {
        // CTRL-MINUS -- Zoom out
      case sf::Keyboard::Key::Dash:
      case sf::Keyboard::Key::Subtract:
        add_zoom(-0.05f);
        break;

        // CTRL-PLUS -- Zoom in
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
        add_zoom(0.05f);
        break;

      case sf::Keyboard::Key::Num0:
        pImpl->map_zoom_level = 1.0f;
        break;

        // CTRL-A -- attire/adorn
      case sf::Keyboard::Key::A:    // Attack
        action.set_type(Action::Type::Attire);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please choose the item(s) to wear first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-C -- close items
      case sf::Keyboard::Key::C:    // Close
        action.set_type(Action::Type::Close);
        if (action.get_things().size() == 0)
        {
          // No item specified, so ask for a direction.
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a direction to close.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        else
        {
          // Item(s) specified, so proceed with items.
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-D -- drop items
      case sf::Keyboard::Key::D:    // Drop
        action.set_type(Action::Type::Drop);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please choose the item(s) to drop first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-E -- eat items
      case sf::Keyboard::Key::E:
        action.set_type(Action::Type::Eat);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please choose the item(s) to eat first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-F -- fill item(s)
      case sf::Keyboard::Key::F:
        action.set_type(Action::Type::Fill);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) to fill first.");
        }
        else
        {
          pImpl->action_in_progress = action;
          the_message_log.add("Choose an item or direction to fill from.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        result = EventResult::Handled;
        break;

        // CTRL-G -- get (pick up) items
      case sf::Keyboard::Key::G:
        action.set_type(Action::Type::Get);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) to pick up first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-H -- hurl items
      case sf::Keyboard::Key::H:
        action.set_type(Action::Type::Hurl);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item you want to hurl first.");
        }
        else if (action.get_things().size() > 1)
        {
          the_message_log.add("You can only hurl one item at once.");
        }
        else
        {
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a direction to hurl the item.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        result = EventResult::Handled;
        break;

        // CTRL-I -- inscribe on an item
      case sf::Keyboard::Key::I:
        action.set_type(Action::Type::Inscribe);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item you want to write on first.");
        }
        else if (action.get_things().size() > 1)
        {
          the_message_log.add("You can only write on one item at a time.");
        }
        else
        {
          pImpl->action_in_progress = action;
          the_message_log.add("Choose an item to write with.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        result = EventResult::Handled;
        break;


        // CTRL-M -- mix items
      case sf::Keyboard::Key::M:
        action.set_type(Action::Type::Mix);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the two items you want to mix together first.");
        }
        else if (action.get_things().size() != 2)
        {
          the_message_log.add("You must select exactly two items to mix together.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::O:    // Open
        action.set_type(Action::Type::Open);
        if (action.get_things().size() == 0)
        {
          // No item specified, so ask for a direction.
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a direction to open.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        else
        {
          // Item(s) specified, so proceed with items.
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-P -- put item in another item
      case sf::Keyboard::Key::P:
        action.set_type(Action::Type::PutInto);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) you want to store first.");
        }
        else
        {
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a container to put the item(s) into.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        result = EventResult::Handled;
        break;

        // CTRL-Q -- quaff (drink) from items
      case sf::Keyboard::Key::Q:
        action.set_type(Action::Type::Quaff);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) you want to quaff from first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-R -- read an item
      case sf::Keyboard::Key::R:
        action.set_type(Action::Type::Read);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) you want to read first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;

        // CTRL-S -- shoot items
      case sf::Keyboard::Key::S:
        action.set_type(Action::Type::Shoot);
        // Skip the item check here, as we want to shoot our wielded weapon
        // if no item is selected.
        if (action.get_things().size() > 1)
        {
          the_message_log.add("You can only shoot one item at once.");
        }
        else
        {
          /// @todo If no items are selected, fire your wielded item.
          ///       Otherwise, wield the selected item and fire it.
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a direction to shoot.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        result = EventResult::Handled;
        break;

        // CTRL-T -- take item out of container
      case sf::Keyboard::Key::T:
        action.set_type(Action::Type::TakeOut);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please select the item(s) to take out first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-U -- use an item
      case sf::Keyboard::Key::U:
        action.set_type(Action::Type::Use);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please choose the item to use first.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-W -- wield item
      case sf::Keyboard::Key::W:
        action.set_type(Action::Type::Wield);
        if (action.get_things().size() == 0)
        {
          the_message_log.add("Please choose the item to wield first.");
        }
        else if (action.get_things().size() > 1)
        {
          the_message_log.add("You may only wield one item at a time.");
        }
        else
        {
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }
        result = EventResult::Handled;
        break;

        // CTRL-X -- Xplicit attack
      case sf::Keyboard::Key::X:
        action.set_type(Action::Type::Attack);
        if (action.get_things().size() == 0)
        {
          // No item specified, so ask for a direction.
          pImpl->action_in_progress = action;
          the_message_log.add("Choose a direction to attack.");
          pImpl->current_input_state = GameInputState::TargetSelection;
          pImpl->inventory_area->clear_selected_slots();
        }
        else if (action.get_things().size() > 1)
        {
          the_message_log.add("You can only attack one item at once.");
        }
        else
        {
          // Item(s) specified, so proceed with items.
          player->queue_action(action);
          pImpl->inventory_area_shows_player = false;
          pImpl->reset_inventory_area();
        }

      default:
        break;
      } // end switch 
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

void AppStateGameMode::add_zoom(float zoom_amount)
{
  float current_zoom_level = pImpl->map_zoom_level;

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
  // (This is done by the map.lua script now.)
  //auto start = game_map.get_tile(start_coords).get_floor();
  //bool player_moved = player->move_into(start);

  // Set cursor to starting location.
  // (Should this be done by the map.lua script too?)
  sf::Vector2i const& start_coords = game_map.get_start_coords();
  pImpl->cursor_coords = start_coords;

  // Set the viewed inventory location to the player's location.
  pImpl->inventory_area_shows_player = false;
  pImpl->reset_inventory_area();

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
