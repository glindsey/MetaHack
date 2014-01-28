#include "AppStateGameMode.h"

#include <memory>
#include <set>

#include "Action.h"
#include "App.h"
#include "ConfigSettings.h"
#include "Entity.h"
#include "ErrorHandler.h"
#include "GetLetterKey.h"
#include "Human.h"
#include "IsType.h"
#include "KeyBuffer.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "Thing.h"
#include "ThingFactory.h"

// Includes required for test code.
#include "CoinGold.h"
#include "LightOrb.h"
#include "Rock.h"
#include "RockLichen.h"
#include "SackLarge.h"

struct AppStateGameMode::Impl
{
  Impl()
  {
    map_zoom_level = 1.0f;
    current_input_state = GameInputState::Map;
    selected_quantity = 1;
    action_in_progress.type = Action::Type::None;

    left_inventory_area.reset(new InventoryArea(calc_left_inven_dims(),
                              selected_things,
                              selected_quantity));
    right_inventory_area.reset(new InventoryArea(calc_right_inven_dims(),
                               selected_things,
                               selected_quantity));

    left_inventory_area->set_capital_letters(true);
    right_inventory_area->set_capital_letters(false);

    status_area.reset(new StatusArea(calc_status_area_dims()));
  }

  /// Map the player is currently on.
  MapId current_map_id;

  /// True if the application window is in focus, false otherwise.
  bool window_in_focus;

  /// Instance of the player status area.
  std::unique_ptr<StatusArea> status_area;

  /// Instance of the left ("surroundings") inventory area.
  std::unique_ptr<InventoryArea> left_inventory_area;

  /// Instance of the right ("player") inventory area.
  std::unique_ptr<InventoryArea> right_inventory_area;

  /// Map zoom level.  1.0 equals 100 percent zoom.
  float map_zoom_level;

  /// Current screen area that has keyboard focus.
  GameInputState current_input_state;

  /// Current location of the cursor on the map.
  sf::Vector2i cursor_coords;

  /// Vector of selected items to perform an action on.
  /// This is a vector instead of a set because the order that things are
  /// selected in is important for some actions (such as "put-into').
  std::vector<ThingId> selected_things;

  /// Item that is the "object" of an action (if any).
  ThingId object_thing;

  /// Selected quantity.
  unsigned int selected_quantity;

  /// Action in progress.
  /// Used for an action that needs a "target".
  Action action_in_progress;

  void reset_inventory_info()
  {
    left_inventory_area->set_viewed_container(TF.get_player());
    right_inventory_area->set_viewed_container(TF.get_player());
    left_inventory_area->set_inventory_type(InventoryType::Around);
    right_inventory_area->set_inventory_type(InventoryType::Inside);
    selected_things.clear();
    update_selected_quantity();
    update_left_inventory();
  }

  void update_selected_quantity()
  {
    if (selected_things.size() > 0)
    {
      ThingId thing_id = selected_things.back();
      Thing& thing = TF.get(thing_id);

      if (isType(&thing, Aggregate))
      {
        Aggregate& agg = dynamic_cast<Aggregate&>(thing);
        selected_quantity = agg.get_quantity();
      }
      else
      {
        selected_quantity = 1;
      }
    }
    else
    {
      selected_quantity = 0;
    }
  }

  sf::IntRect calc_status_area_dims()
  {
    sf::IntRect statusAreaDims;
    sf::IntRect leftInvAreaDims = left_inventory_area->get_dimensions();
    sf::IntRect rightInvAreaDims = right_inventory_area->get_dimensions();
    statusAreaDims.width = the_window.getSize().x -
                           (leftInvAreaDims.width +
                            rightInvAreaDims.width + 24);
    statusAreaDims.height = Settings.status_area_height;
    statusAreaDims.top = the_window.getSize().y -
                         (Settings.status_area_height + 5);
    statusAreaDims.left = leftInvAreaDims.width + 12;
    return statusAreaDims;
  }

  sf::IntRect calc_left_inven_dims()
  {
    sf::IntRect messageLogDims = the_message_log.get_dimensions();
    sf::IntRect inventoryAreaDims;
    inventoryAreaDims.width = Settings.inventory_area_width;
    inventoryAreaDims.height = the_window.getSize().y -
                               (messageLogDims.height + 18);
    inventoryAreaDims.left = 3;
    inventoryAreaDims.top = messageLogDims.top + messageLogDims.height + 10;

    return inventoryAreaDims;
  }

  sf::IntRect calc_right_inven_dims()
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

  void update_left_inventory()
  {
    if (current_input_state == GameInputState::CursorLook)
    {
      Map& game_map = MF.get(current_map_id);
      MapTile& tile = game_map.get_tile(cursor_coords);
      left_inventory_area->set_viewed_container(tile);
      left_inventory_area->set_inventory_type(InventoryType::Inside);
    }
    else
    {
      left_inventory_area->set_viewed_container(TF.get_player());
      left_inventory_area->set_inventory_type(InventoryType::Around);
    }
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
  : State(state_machine), impl(new Impl())
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

  Entity& player = TF.get_player();

  if (player.pending_action())
  {
    // QUESTION: Do we want to update all Things, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the root of the player's location.
    ThingId root_id = player.get_root_id();
    Container& root_container = TF.get_container(root_id);
    if (root_container.is_maptile())
    {
      // Process everything on the map.
      MapId root_map_id = root_container.get_map_id();
      Map& root_map = MF.get(root_map_id);

      std::vector<ThingId> thing_ids;
      root_map.gather_thing_ids(thing_ids);

      for (std::vector<ThingId>::iterator iter = thing_ids.begin();
           iter != thing_ids.end(); ++iter)
      {
        Thing& thing = TF.get(*iter);
        thing.do_process();
      }
    }
    else
    {
      // Hmm.  This shouldn't happen unless the player is in Limbo.
      MAJOR_ERROR("Player's root location isn't a MapTile?"
                  "  Uh, player, where did you go?");
    }

    // If player is directly on a map...
    Container& location = TF.get_container(player.get_location_id());
    if (location.is_maptile())
    {
      Map& player_map = MF.get(TF.get_tile(player.get_location_id()).get_map_id());

      // Update map lighting.
      player_map.update_lighting();

      // Update tile vertex array.
      player_map.update_tile_vertices(player);
    }
  }
}

bool AppStateGameMode::render(sf::RenderTarget& target, int frame)
{
  // Set focus for areas.
  the_message_log.set_focus(impl->current_input_state == GameInputState::MessageLog);
  impl->status_area->set_focus(impl->current_input_state == GameInputState::Map);

  try
  {
    Entity& player = TF.get_player();
    ThingId player_location_id = player.get_location_id();
    Container& location = TF.get_container(player_location_id);

    if (location.is_maptile())
    {
      MapTile& tile = TF.get_tile(player_location_id);
      Map& game_map = MF.get(tile.get_map_id());
      sf::Vector2i tile_coords = tile.get_coords();
      sf::Vector2f player_pixel_coords = MapTile::get_pixel_coords(tile_coords);
      sf::Vector2f cursor_pixel_coords = MapTile::get_pixel_coords(impl->cursor_coords);

      // Update thing vertex array.
      game_map.update_thing_vertices(player, frame);

      if (impl->current_input_state == GameInputState::CursorLook)
      {
        game_map.set_view(target, cursor_pixel_coords, impl->map_zoom_level);
        game_map.draw_to(target);

        MapTile& cursor_tile = game_map.get_tile(impl->cursor_coords);
        cursor_tile.draw_highlight(target,
                                   cursor_pixel_coords,
                                   Settings.cursor_border_color,
                                   Settings.cursor_bg_color,
                                   frame);
      }
      else
      {
        game_map.set_view(target, player_pixel_coords, impl->map_zoom_level);
        game_map.draw_to(target);
      }
    }
    else
    {
      /// @todo Handle rendering if we're inside something other than a map.
    }

    the_message_log.render(target, frame);
    impl->status_area->render(target, frame);
    impl->left_inventory_area->render(target, frame);
    impl->right_inventory_area->render(target, frame);
  }
  catch (std::exception const& e)
  {
    FATAL_ERROR("Exception while rendering: %s", e.what());
  }

  return true;
}

EventResult AppStateGameMode::handle_key_press(sf::Event::KeyEvent& key)
{
  EventResult result = EventResult::Ignored;
  Entity& player = TF.get_player();

  // *** Handle keys processed in any mode.
  if (!key.alt && !key.control)
  {
    if (key.code == sf::Keyboard::Key::Tab)
    {
      switch (impl->current_input_state)
      {
      case GameInputState::Map:
        impl->current_input_state = GameInputState::MessageLog;
        return EventResult::Handled;

      case GameInputState::MessageLog:
        impl->current_input_state = GameInputState::Map;
        return EventResult::Handled;

      default:
        break;
      }
    }
  }

  // *** Handle keys unique to a particular focus.
  switch (impl->current_input_state)
  {
  case GameInputState::TargetSelection:
  {
    int key_number = get_letter_key(key);
    Direction key_direction = get_direction_key(key);

    /// @todo Allow selecting a single object if the action permits it.
    if (impl->action_in_progress.target_can_be_thing)
    {
      // *** NON-MODIFIED KEYS ***********************************************
      if (!key.alt && !key.control && !key.shift)
      {
        if (key_number != -1)
        {
          impl->action_in_progress.target =
            impl->right_inventory_area->get_thingid(key_number);
          player.queue_action(impl->action_in_progress);
          impl->reset_inventory_info();
          impl->current_input_state = GameInputState::Map;
          result = EventResult::Handled;
        }
      }

      // *** SHIFTED KEYS *****************************************************
      else if (!key.alt && !key.control && key.shift)
      {
        if (key_number != -1)
        {
          impl->action_in_progress.target =
            impl->left_inventory_area->get_thingid(key_number);
          player.queue_action(impl->action_in_progress);
          impl->reset_inventory_info();
          impl->current_input_state = GameInputState::Map;
          result = EventResult::Handled;
        }
      }
    } // end if (impl->action_in_progress.target_can_be_thing)

    /// @todo Allow choosing directions if the action permits it.
    if (impl->action_in_progress.target_can_be_direction)
    {
      if (key_direction != Direction::None)
      {
        impl->action_in_progress.direction = key_direction;
        player.queue_action(impl->action_in_progress);
        impl->reset_inventory_info();
        impl->current_input_state = GameInputState::Map;
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
        impl->move_cursor(Direction::North);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageUp:
        impl->move_cursor(Direction::Northeast);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Right:
        impl->move_cursor(Direction::East);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageDown:
        impl->move_cursor(Direction::Southeast);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Down:
        impl->move_cursor(Direction::South);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::End:
        impl->move_cursor(Direction::Southwest);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Left:
        impl->move_cursor(Direction::West);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Home:
        impl->move_cursor(Direction::Northwest);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // "/" - go back to Map focus.
      case sf::Keyboard::Key::Slash:
        impl->current_input_state = GameInputState::Map;
        impl->reset_inventory_info();
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

    // *** NON-MODIFIED KEYS ***********************************************
    if (!key.alt && !key.control && !key.shift)
    {
      if ((key_number != -1) && (key_number != 0))
      {
        impl->right_inventory_area->toggle_selection(key_number);
        impl->update_selected_quantity();
        result = EventResult::Handled;
      }
      else if (key_direction != Direction::None)
      {
        if (key_direction == Direction::Self)
        {
          action.type = Action::Type::Wait;
          player.queue_action(action);
          result = EventResult::Handled;
        }
        else
        {
          action.type = Action::Type::Move;
          action.direction = key_direction;
          player.queue_action(action);
          result = EventResult::Handled;
        }
      }
      else switch (key.code)
      {
      case sf::Keyboard::Key::BackSpace:
        impl->reset_inventory_info();
        break;

        // "/" - go to cursor look mode.
      case sf::Keyboard::Key::Slash:
        impl->current_input_state = GameInputState::CursorLook;
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // "-" - subtract quantity
      case sf::Keyboard::Key::Dash:
      case sf::Keyboard::Key::Subtract:
        if (impl->selected_things.size() < 1)
        {
          the_message_log.add("You have to have something selected "
                              "before you can choose a quantity.");
        }
        else if (impl->selected_things.size() > 1)
        {
          the_message_log.add("You can only have one thing selected "
                              "when choosing a quantity.");
        }
        else
        {
          if (impl->selected_quantity > 1)
          {
            --(impl->selected_quantity);
          }
        }
        result = EventResult::Handled;
        break;

        // "+"/"=" - add quantity
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
      {
        unsigned int max_quantity;
        if (impl->selected_things.size() < 1)
        {
          the_message_log.add("You have to have something selected "
                              "before you can choose a quantity.");
          max_quantity = 1;
        }
        else if (impl->selected_things.size() > 1)
        {
          the_message_log.add("You can only have one thing selected "
                              "when choosing a quantity.");
          max_quantity = 1;
        }
        else
        {
          ThingId thing_id = impl->selected_things.back();
          Thing& thing = TF.get(thing_id);

          if (isType(&thing, Aggregate))
          {
            Aggregate& agg = dynamic_cast<Aggregate&>(thing);
            max_quantity = agg.get_quantity();
          }
          else
          {
            max_quantity = 1;
          }

          if (impl->selected_quantity < max_quantity)
          {
            ++(impl->selected_quantity);
          }
        }
      }

      result = EventResult::Handled;
      break;

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

    // *** SHIFTED KEYS *****************************************************
    if (!key.alt && !key.control && key.shift)
    {
      if (key_number != -1)
      {
        impl->left_inventory_area->toggle_selection(key_number);
        impl->update_selected_quantity();
        result = EventResult::Handled;
      }
      else if (key_direction != Direction::None)
      {
        action.type = Action::Type::Move;
        action.direction = key_direction;
        player.queue_action(action);
        result = EventResult::Handled;
      }
      else switch (key.code)
      {
      case sf::Keyboard::Key::LBracket:
        impl->reset_inventory_info();
        break;

      case sf::Keyboard::Key::RBracket:
        if (impl->selected_things.size() > 0)
        {
          ThingId thing_id = impl->selected_things.back();
          Thing& thing = TF.get(thing_id);

          if (thing.is_container())
          {
            Container& container = TF.get_container(thing_id);
            if (container.is_open())
            {
              if (!container.is_locked())
              {
                ThingId owner_id = thing.get_owner_id();
                Container& owner = TF.get_container(owner_id);
                if (owner.is_maptile())
                {
                  impl->left_inventory_area->set_viewed_container(container);
                  impl->left_inventory_area->set_inventory_type(InventoryType::Inside);
                }
                else
                {
                  impl->right_inventory_area->set_viewed_container(container);
                }

                impl->selected_things.clear();
              }
              else // if (container.is_locked())
              {
                the_message_log.add(container.get_name() +
                                    container.choose_verb(" are", " is") +
                                    " locked.");
              }
            }
            else // if (!container.is_open())
            {
              the_message_log.add(container.get_name() +
                                  container.choose_verb(" are", " is") +
                                  " closed.");
            }
          }
          else // if (!thing.is_container())
          {
            the_message_log.add(thing.get_name() +
                                thing.choose_verb(" are", " is") +
                                " not a container.");
          }
        }
        else
        {
          the_message_log.add("Nothing is currently selected.");
        }
        break;

      default:
        break;
      } // end switch (key.code)
    } // end if (!key.alt && !key.control && key.shift)

    // *** CONTROL KEYS *******************************************************
    // (Shift key is IGNORED here, keys are not case-sensitive.)
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
        action.thing_ids = impl->selected_things;
        player.queue_action(action);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // CTRL-P -- pick up items
      case sf::Keyboard::Key::P:    // Pick up
        action.type = Action::Type::Pickup;
        action.thing_ids = impl->selected_things;
        player.queue_action(action);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // CTRL-Q -- quaff (drink) items
      case sf::Keyboard::Key::Q:
        action.type = Action::Type::Quaff;
        action.thing_ids = impl->selected_things;
        player.queue_action(action);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // CTRL-S -- store item in another item
      case sf::Keyboard::Key::S:
        action.type = Action::Type::Store;
        action.thing_ids = impl->selected_things;
        if (action.thing_ids.size() > 0)
        {
          impl->action_in_progress = action;
          the_message_log.add("Choose a container to store into.");
          impl->current_input_state = GameInputState::TargetSelection;
          impl->action_in_progress.target_can_be_thing = true;
          impl->selected_things.clear();
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
        action.thing_ids = impl->selected_things;
        player.queue_action(action);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

        // CTRL-W -- wield item
      case sf::Keyboard::Key::W:
        action.type = Action::Type::Wield;
        action.thing_ids = impl->selected_things;
        player.queue_action(action);
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      default:
        break;
      }
    } // end if (!key.alt && key.control)

    // *** ALT KEYS ***********************************************************
    // (Shift key is IGNORED here, keys are not case-sensitive.)
    if (key.alt && !key.control)
    {
      switch (key.code)
      {
      default:
        break;
      }
    }

    // *** CONTROL + ALT KEYS *************************************************
    // (Shift key is IGNORED here, keys are not case-sensitive.)
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
  } // end switch (impl->current_input_state)

  return result;
}

EventResult AppStateGameMode::handle_mouse_wheel(sf::Event::MouseWheelEvent& wheel)
{
  add_zoom(wheel.delta * 0.05f);
  return EventResult::Handled;
}

void AppStateGameMode::add_zoom(float zoom_amount)
{
  float current_zoom_level = impl->map_zoom_level;

  current_zoom_level += zoom_amount;

  if (current_zoom_level < 0.1f)
  {
    current_zoom_level = 0.1f;
  }

  if (current_zoom_level > 3.0f)
  {
    current_zoom_level = 3.0f;
  }

  impl->map_zoom_level = current_zoom_level;
}

EventResult AppStateGameMode::handle_event(sf::Event& event)
{
  EventResult result = EventResult::Ignored;

  switch (event.type)
  {
  case sf::Event::EventType::Resized:
  {
    impl->left_inventory_area->set_dimensions(impl->calc_left_inven_dims());
    impl->right_inventory_area->set_dimensions(impl->calc_right_inven_dims());
    impl->status_area->set_dimensions(impl->calc_status_area_dims());
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
    switch (impl->current_input_state)
    {
    case GameInputState::Map:
      result = impl->status_area->handle_event(event);
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
  impl->current_map_id = MF.create(64, 64);
  Map& game_map = MF.get(impl->current_map_id);

  // Move player to start position on the map.
  ThingId player_id = TF.create(typeid(Human).name());
  TF.set_player_id(player_id);
  sf::Vector2i const& start_coords = game_map.get_start_coords();

  ThingId start_id = game_map.get_tile_id(start_coords.x, start_coords.y);
  bool player_moved = TF.get_player().move_into(start_id);

  if (player_moved == false)
  {
    FATAL_ERROR("Could not move player to start position!");
    return false;
  }

  // Set cursor to starting location.
  impl->cursor_coords = start_coords;

  // Set the left inventory location to the player's location,
  // and the right inventory location to the player's inventory.
  impl->reset_inventory_info();

  // TESTING CODE: Create a lighting orb held in player inventory.
  ThingId player_orb_id = TF.create<LightOrb>();
  TF.get(player_orb_id).move_into(player_id);

  // TESTING CODE: Create a rock immediately south of the player.
  ThingId rock_id = TF.create<Rock>();
  TF.get(rock_id).move_into(game_map.get_tile_id(start_coords.x,
                            start_coords.y + 1));

  // TESTING CODE: Create a sack immediately east of the player.
  ThingId sack_id = TF.create<SackLarge>();
  TF.get(sack_id).move_into(game_map.get_tile_id(start_coords.x + 1,
                            start_coords.y));

  // TESTING CODE: Create five gold coins west of the player.
  ThingId coins_id = TF.create<CoinGold>();
  Aggregate& coins_agg = dynamic_cast<Aggregate&>(TF.get(coins_id));
  coins_agg.set_quantity(5);
  TF.get(coins_id).move_into(game_map.get_tile_id(start_coords.x - 1,
                             start_coords.y));

  // TESTING CODE: Create ten gold coins northwest of the player.
  ThingId coins2_id = TF.create<CoinGold>();
  Aggregate& coins2_agg = dynamic_cast<Aggregate&>(TF.get(coins2_id));
  coins2_agg.set_quantity(10);
  TF.get(coins2_id).move_into(game_map.get_tile_id(start_coords.x - 1,
                              start_coords.y - 1));

  // TESTING CODE: Create ten gold coins northeast of the player.
  ThingId lichen_id = TF.create<RockLichen>();
  TF.get(lichen_id).move_into(game_map.get_tile_id(start_coords.x + 1,
                              start_coords.y - 1));

  // END TESTING CODE

  // Get the map ready.
  game_map.update_lighting();
  game_map.update_tile_vertices(TF.get_player());
  game_map.update_thing_vertices(TF.get_player(), 0);

  the_message_log.add("Welcome to the Etheric Catacombs!");

  return true;
}

bool AppStateGameMode::terminate()
{
  return true;
}
