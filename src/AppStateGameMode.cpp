#include "AppStateGameMode.h"

#include <memory>
#include <set>

#include "Action.h"
#include "App.h"
#include "ConfigSettings.h"
#include "Entity.h"
#include "ErrorHandler.h"
#include "Human.h"
#include "IsType.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MessageLog.h"
#include "Thing.h"
#include "ThingFactory.h"

// Includes required for test code.
#include "LightOrb.h"
#include "Rock.h"
#include "SackLarge.h"

struct AppStateGameMode::Impl
{
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
  AreaFocus current_area_focus;

  /// If true, the arrow keys move the player.  If false, they only move the
  /// cursor.  This lets you examine things on the map, as well as perform
  /// actions on them.
  bool nav_mode_player;

  /// Current location of the cursor on the map.
  sf::Vector2i cursor_coords;

  /// Vector of selected items to perform an action on.
  /// This is a vector instead of a set because the order that things are
  /// selected in is important for some actions (such as "put-into').
  std::vector<ThingId> selected_things;

  void reset_inventory_info()
  {
    ThingId player_id = TF.get_player_id();
    left_inventory_area->set_viewed_id(player_id);
    right_inventory_area->set_viewed_id(player_id);
    left_inventory_area->set_inventory_type(InventoryType::Around);
    right_inventory_area->set_inventory_type(InventoryType::Inside);
    selected_things.clear();
    update_left_inventory();
  }

  sf::IntRect calc_status_area_dims()
  {
    sf::IntRect statusAreaDims;
    sf::IntRect leftInvAreaDims = left_inventory_area->get_dimensions();
    sf::IntRect rightInvAreaDims = right_inventory_area->get_dimensions();
    statusAreaDims.width = the_window.getSize().x -
                           (leftInvAreaDims.width +
                            rightInvAreaDims.width + 18);
    statusAreaDims.height = Settings.status_area_height;
    statusAreaDims.top = the_window.getSize().y -
                         (Settings.status_area_height + 3);
    statusAreaDims.left = leftInvAreaDims.width + 9;
    return statusAreaDims;
  }

  sf::IntRect calc_left_inven_dims()
  {
    sf::IntRect messageLogDims = the_message_log.get_dimensions();
    sf::IntRect inventoryAreaDims;
    inventoryAreaDims.width = Settings.inventory_area_width;
    inventoryAreaDims.height = the_window.getSize().y -
                                (messageLogDims.height + 12);
    inventoryAreaDims.left = 3;
    inventoryAreaDims.top = messageLogDims.top + messageLogDims.height + 6;

    return inventoryAreaDims;
  }

  sf::IntRect calc_right_inven_dims()
  {
    sf::IntRect messageLogDims = the_message_log.get_dimensions();
    sf::IntRect inventoryAreaDims;
    inventoryAreaDims.width = Settings.inventory_area_width;
    inventoryAreaDims.height = the_window.getSize().y -
                                (messageLogDims.height + 12);
    inventoryAreaDims.left = the_window.getSize().x -
                                (inventoryAreaDims.width + 3);
    inventoryAreaDims.top = messageLogDims.top + messageLogDims.height + 6;

    return inventoryAreaDims;
  }

  void update_left_inventory()
  {
    if (nav_mode_player)
    {
      ThingId player_id = TF.get_player_id();
      left_inventory_area->set_viewed_id(player_id);
      left_inventory_area->set_inventory_type(InventoryType::Around);
    }
    else
    {
      Map& game_map = MF.get(current_map_id);
      ThingId tileId = game_map.get_tile_id(cursor_coords.x, cursor_coords.y);
      left_inventory_area->set_viewed_id(tileId);
      left_inventory_area->set_inventory_type(InventoryType::Inside);
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
  impl->current_area_focus = AreaFocus::Map;
  impl->nav_mode_player = true;
  impl->map_zoom_level = 1.0f;

  impl->left_inventory_area.reset(new InventoryArea(impl->calc_left_inven_dims(),
                                                    impl->selected_things));
  impl->right_inventory_area.reset(new InventoryArea(impl->calc_right_inven_dims(),
                                                     impl->selected_things));

  impl->left_inventory_area->set_capital_letters(true);
  impl->right_inventory_area->set_capital_letters(false);

  impl->status_area.reset(new StatusArea(impl->calc_status_area_dims()));
}

AppStateGameMode::~AppStateGameMode()
{
  //dtor
}

void AppStateGameMode::execute()
{
  Entity& player = TF.get_player();

  if (player.pending_action())
  {
    // QUESTION: Do we want to update all Things, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the root of the player's location.
    ThingId root_id = player.get_root_id();
    Thing& root_thing = TF.get(root_id);
    if (isType(&root_thing, MapTile))
    {
      // Process everything on the map.
      MapId root_map_id = root_thing.get_map_id();
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
      MAJOR_ERROR("Player's root location isn't a MapTile?  Uh, player, where did you go?");
    }

    // If player is directly on a map...
    if (isType(&TF.get(player.get_location_id()), MapTile))
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
  the_message_log.set_focus(impl->current_area_focus == AreaFocus::MessageLog);
  impl->status_area->set_focus(impl->current_area_focus == AreaFocus::Map);
  impl->left_inventory_area->set_focus(impl->current_area_focus == AreaFocus::LeftInv);
  impl->right_inventory_area->set_focus(impl->current_area_focus == AreaFocus::RightInv);

  try
  {
    Entity& player = TF.get_player();
    ThingId player_location_id = player.get_location_id();
    Thing& location = TF.get(player_location_id);

    if (isType(&location, MapTile))
    {
      MapTile& tile = TF.get_tile(player_location_id);
      Map& game_map = MF.get(tile.get_map_id());
      sf::Vector2i tile_coords = tile.get_coords();
      sf::Vector2f player_pixel_coords = MapTile::get_pixel_coords(tile_coords);
      sf::Vector2f cursor_pixel_coords = MapTile::get_pixel_coords(impl->cursor_coords);

      // Update thing vertex array.
      game_map.update_thing_vertices(player, frame);

      if (impl->nav_mode_player)
      {
        game_map.set_view(target, player_pixel_coords, impl->map_zoom_level);
        game_map.draw_to(target);
      }
      else
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
    }
    else
    {
      // TODO: handle rendering if we're inside, um, something else
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

  // First handle stuff that isn't predicated on window focus.
  if (!key.alt && !key.control)
  {
    // Letter selections...
    if (!key.shift)
    {
      switch (key.code)
      {
      case sf::Keyboard::Key::LBracket:
        {
          impl->reset_inventory_info();
        }
        break;

      case sf::Keyboard::Key::RBracket:
        if (impl->selected_things.size() > 0)
        {
          ThingId thing_id = impl->selected_things.back();
          Thing& thing = TF.get(thing_id);

          if (thing.get_inventory_size() != 0)
          {
            ThingId owner_id = thing.get_owner_id();
            Thing& owner = TF.get(owner_id);
            if (isType(&owner, MapTile))
            {
              impl->left_inventory_area->set_viewed_id(thing_id);
              impl->left_inventory_area->set_inventory_type(InventoryType::Inside);
            }
            else
            {
              impl->right_inventory_area->set_viewed_id(thing_id);
            }

            impl->selected_things.clear();
          }
        }
        else
        {
          the_message_log.add("The last-selected thing is not a container.");
        }
        break;

      case sf::Keyboard::Key::A:
        impl->right_inventory_area->toggle_selection(1);
        break;
      case sf::Keyboard::Key::B:
        impl->right_inventory_area->toggle_selection(2);
        break;
      case sf::Keyboard::Key::C:
        impl->right_inventory_area->toggle_selection(3);
        break;
      case sf::Keyboard::Key::D:
        impl->right_inventory_area->toggle_selection(4);
        break;
      case sf::Keyboard::Key::E:
        impl->right_inventory_area->toggle_selection(5);
        break;
      case sf::Keyboard::Key::F:
        impl->right_inventory_area->toggle_selection(6);
        break;
      case sf::Keyboard::Key::G:
        impl->right_inventory_area->toggle_selection(7);
        break;
      case sf::Keyboard::Key::H:
        impl->right_inventory_area->toggle_selection(8);
        break;
      case sf::Keyboard::Key::I:
        impl->right_inventory_area->toggle_selection(9);
        break;
      case sf::Keyboard::Key::J:
        impl->right_inventory_area->toggle_selection(10);
        break;
      case sf::Keyboard::Key::K:
        impl->right_inventory_area->toggle_selection(11);
        break;
      case sf::Keyboard::Key::L:
        impl->right_inventory_area->toggle_selection(12);
        break;
      case sf::Keyboard::Key::M:
        impl->right_inventory_area->toggle_selection(13);
        break;
      case sf::Keyboard::Key::N:
        impl->right_inventory_area->toggle_selection(14);
        break;
      case sf::Keyboard::Key::O:
        impl->right_inventory_area->toggle_selection(15);
        break;
      case sf::Keyboard::Key::P:
        impl->right_inventory_area->toggle_selection(16);
        break;
      case sf::Keyboard::Key::Q:
        impl->right_inventory_area->toggle_selection(17);
        break;
      case sf::Keyboard::Key::R:
        impl->right_inventory_area->toggle_selection(18);
        break;
      case sf::Keyboard::Key::S:
        impl->right_inventory_area->toggle_selection(19);
        break;
      case sf::Keyboard::Key::T:
        impl->right_inventory_area->toggle_selection(20);
        break;
      case sf::Keyboard::Key::U:
        impl->right_inventory_area->toggle_selection(21);
        break;
      case sf::Keyboard::Key::V:
        impl->right_inventory_area->toggle_selection(22);
        break;
      case sf::Keyboard::Key::W:
        impl->right_inventory_area->toggle_selection(23);
        break;
      case sf::Keyboard::Key::X:
        impl->right_inventory_area->toggle_selection(24);
        break;
      case sf::Keyboard::Key::Y:
        impl->right_inventory_area->toggle_selection(25);
        break;
      case sf::Keyboard::Key::Z:
        impl->right_inventory_area->toggle_selection(26);
        break;
      default:
        break;
      }
    }
    else
    {
      switch (key.code)
      {
      case sf::Keyboard::Key::Num2:
        impl->left_inventory_area->toggle_selection(0);
        break;
      case sf::Keyboard::Key::A:
        impl->left_inventory_area->toggle_selection(1);
        break;
      case sf::Keyboard::Key::B:
        impl->left_inventory_area->toggle_selection(2);
        break;
      case sf::Keyboard::Key::C:
        impl->left_inventory_area->toggle_selection(3);
        break;
      case sf::Keyboard::Key::D:
        impl->left_inventory_area->toggle_selection(4);
        break;
      case sf::Keyboard::Key::E:
        impl->left_inventory_area->toggle_selection(5);
        break;
      case sf::Keyboard::Key::F:
        impl->left_inventory_area->toggle_selection(6);
        break;
      case sf::Keyboard::Key::G:
        impl->left_inventory_area->toggle_selection(7);
        break;
      case sf::Keyboard::Key::H:
        impl->left_inventory_area->toggle_selection(8);
        break;
      case sf::Keyboard::Key::I:
        impl->left_inventory_area->toggle_selection(9);
        break;
      case sf::Keyboard::Key::J:
        impl->left_inventory_area->toggle_selection(10);
        break;
      case sf::Keyboard::Key::K:
        impl->left_inventory_area->toggle_selection(11);
        break;
      case sf::Keyboard::Key::L:
        impl->left_inventory_area->toggle_selection(12);
        break;
      case sf::Keyboard::Key::M:
        impl->left_inventory_area->toggle_selection(13);
        break;
      case sf::Keyboard::Key::N:
        impl->left_inventory_area->toggle_selection(14);
        break;
      case sf::Keyboard::Key::O:
        impl->left_inventory_area->toggle_selection(15);
        break;
      case sf::Keyboard::Key::P:
        impl->left_inventory_area->toggle_selection(16);
        break;
      case sf::Keyboard::Key::Q:
        impl->left_inventory_area->toggle_selection(17);
        break;
      case sf::Keyboard::Key::R:
        impl->left_inventory_area->toggle_selection(18);
        break;
      case sf::Keyboard::Key::S:
        impl->left_inventory_area->toggle_selection(19);
        break;
      case sf::Keyboard::Key::T:
        impl->left_inventory_area->toggle_selection(20);
        break;
      case sf::Keyboard::Key::U:
        impl->left_inventory_area->toggle_selection(21);
        break;
      case sf::Keyboard::Key::V:
        impl->left_inventory_area->toggle_selection(22);
        break;
      case sf::Keyboard::Key::W:
        impl->left_inventory_area->toggle_selection(23);
        break;
      case sf::Keyboard::Key::X:
        impl->left_inventory_area->toggle_selection(24);
        break;
      case sf::Keyboard::Key::Y:
        impl->left_inventory_area->toggle_selection(25);
        break;
      case sf::Keyboard::Key::Z:
        impl->left_inventory_area->toggle_selection(26);
        break;
      default:
        break;
      }
    }

    if (key.code == sf::Keyboard::Key::BackSpace)
    {
      impl->reset_inventory_info();
    }

    if (key.code == sf::Keyboard::Key::Tab)
    {
      if (!key.shift)
      {
        switch (impl->current_area_focus)
        {
        case AreaFocus::Map:
          impl->current_area_focus = AreaFocus::LeftInv;
          return EventResult::Handled;

        case AreaFocus::LeftInv:
          impl->current_area_focus = AreaFocus::MessageLog;
          return EventResult::Handled;

        case AreaFocus::MessageLog:
          impl->current_area_focus = AreaFocus::RightInv;
          return EventResult::Handled;

        case AreaFocus::RightInv:
          impl->current_area_focus = AreaFocus::Map;
          return EventResult::Handled;

        default:
          break;
        }
      }
      else
      {
        switch (impl->current_area_focus)
        {
        case AreaFocus::Map:
          impl->current_area_focus = AreaFocus::RightInv;
          return EventResult::Handled;

        case AreaFocus::LeftInv:
          impl->current_area_focus = AreaFocus::Map;
          return EventResult::Handled;

        case AreaFocus::MessageLog:
          impl->current_area_focus = AreaFocus::LeftInv;
          return EventResult::Handled;

        case AreaFocus::RightInv:
          impl->current_area_focus = AreaFocus::MessageLog;
          return EventResult::Handled;

        default:
          break;
        }
      }
    }
  }

  if (impl->current_area_focus == AreaFocus::Map)
  {
    Action action;

    // *** NON-MODIFIED KEYS **************************************************
    if (!key.alt && !key.control && !key.shift)
    {
      switch (key.code)
      {

      case sf::Keyboard::Key::Up:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::North;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::North);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageUp:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::Northeast;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::Northeast);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Right:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::East;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::East);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::PageDown:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::Southeast;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::Southeast);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Down:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::South;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::South);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::End:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::Southwest;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::Southwest);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Left:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::West;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::West);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      case sf::Keyboard::Key::Home:
        if (impl->nav_mode_player)
        {
          // TODO: check if dir is blocked, and if so, select instead of moving.
          action.type = Action::Type::Move;
          action.move_info.direction = Direction::Northwest;
          player.queue_action(action);
        }
        else
        {
          impl->move_cursor(Direction::Northwest);
          impl->reset_inventory_info();
        }
        result = EventResult::Handled;
        break;

      // "/" - toggle between player/cursor movement
      case sf::Keyboard::Key::Slash:
        impl->nav_mode_player = !impl->nav_mode_player;
        impl->reset_inventory_info();
        result = EventResult::Handled;
        break;

      // "." - wait a turn
      case sf::Keyboard::Key::Period:
      case sf::Keyboard::Key::Delete:
        action.type = Action::Type::Wait;
        player.queue_action(action);

        result = EventResult::Handled;
        break;

      // "," - NH-style shortcut for "pick up"
      case sf::Keyboard::Key::Comma:
        key.alt = false;
        key.control = true;
        key.shift = false;
        key.code = sf::Keyboard::Key::P;
        break;

      default:
        break;
      }
    }

    // *** SHIFTED KEYS *******************************************************
    if (!key.alt && !key.control && key.shift)
    {
      switch (key.code)
      {
      // "<" - go up a level
      case sf::Keyboard::Key::Comma:
        action.type = Action::Type::Move;
        action.move_info.direction = Direction::Up;
        player.queue_action(action);
        result = EventResult::Handled;
        break;

      // ">" - go down a level
      case sf::Keyboard::Key::Period:
        action.type = Action::Type::Move;
        action.move_info.direction = Direction::Down;
        player.queue_action(action);
        result = EventResult::Handled;
        break;

      default:
        break;
      }
    }

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
        player.queue_action(action);
        impl->reset_inventory_info();
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

      default:
        break;
      }
    }

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

  } // end if (impl->current_area_focus == AreaFocus::Map)

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
  // TODO: Obviously we need to make this more state-dependent, as we'll
  //       handle keys differently if we're issuing commands, at a prompt, in
  //       a menu, et cetera.

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
    switch (impl->current_area_focus)
    {
    case AreaFocus::Map:
      result = impl->status_area->handle_event(event);
      break;

    case AreaFocus::MessageLog:
      result = the_message_log.handle_event(event);
      break;

    case AreaFocus::LeftInv:
      result = impl->left_inventory_area->handle_event(event);
      break;

    case AreaFocus::RightInv:
      result = impl->right_inventory_area->handle_event(event);
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
  TF.get(rock_id).move_into(game_map.get_tile_id(start_coords.x, start_coords.y + 1));

  // TESTING CODE: Create a sack immediately east of the player.
  ThingId sack_id = TF.create<SackLarge>();
  TF.get(sack_id).move_into(game_map.get_tile_id(start_coords.x + 1, start_coords.y));

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
