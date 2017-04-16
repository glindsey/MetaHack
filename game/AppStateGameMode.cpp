#include "stdafx.h"

#include "game/AppStateGameMode.h"

#include "actions/Action.h"
#include "AssertHelper.h"
#include "design_patterns/Event.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/AppState.h"
#include "game/GameState.h"
#include "game_windows/MessageLogView.h"
#include "game_windows/StatusArea.h"
#include "inventory/InventoryArea.h"
#include "inventory/InventorySelection.h"
#include "keybuffer/KeyBuffer.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "map/MapStandard2DView.h"
#include "maptile/MapTile.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/IGraphicViews.h"
#include "services/IStringDictionary.h"
#include "services/MessageLog.h"
#include "state_machine/StateMachine.h"
#include "utilities/GetLetterKey.h"
#include "utilities/StringTransforms.h"

/// Actions that can be performed.
#include "actions/ActionAttack.h"
#include "actions/ActionWear.h"
#include "actions/ActionClose.h"
#include "actions/ActionDrop.h"
#include "actions/ActionEat.h"
#include "actions/ActionFill.h"
#include "actions/ActionGet.h"
#include "actions/ActionHurl.h"
#include "actions/ActionInscribe.h"
#include "actions/ActionLock.h"
#include "actions/ActionMix.h"
#include "actions/ActionMove.h"
#include "actions/ActionOpen.h"
#include "actions/ActionPutInto.h"
#include "actions/ActionQuaff.h"
#include "actions/ActionRead.h"
#include "actions/ActionShoot.h"
#include "actions/ActionTakeOut.h"
#include "actions/ActionTurn.h"
#include "actions/ActionUnlock.h"
#include "actions/ActionUse.h"
#include "actions/ActionWait.h"
#include "actions/ActionWield.h"

#include "GUIObject.h"
#include "GUIWindow.h"

AppStateGameMode::AppStateGameMode(StateMachine& state_machine, sf::RenderWindow& m_app_window)
  :
  AppState(state_machine,
           std::bind(&AppStateGameMode::render_map, this, std::placeholders::_1, std::placeholders::_2)),
  m_app_window{ m_app_window },
  m_debug_buffer{ NEW KeyBuffer() },
  m_game_state{ NEW GameState() },
  m_inventory_selection{ NEW InventorySelection() },
  m_window_in_focus{ true },
  m_inventory_area_shows_player{ false },
  m_map_zoom_level{ 1.0f },
  m_current_input_state{ GameInputState::Map },
  m_cursor_coords{ 0, 0 }
{
  getStateMachine().addObserver(*this, App::EventAppWindowResized::id());
  getStateMachine().addObserver(*this, App::EventKeyPressed::id());

  the_desktop.addChild(NEW MessageLogView("MessageLogView", Service<IMessageLog>::get(), *(m_debug_buffer.get()), calcMessageLogDims()))->setFlag("titlebar", true);
  the_desktop.addChild(NEW InventoryArea("InventoryArea", *(m_inventory_selection.get()), calcInventoryDims()))->setFlag("titlebar", true);
  the_desktop.addChild(NEW StatusArea("StatusArea", calcStatusAreaDims()))->setGlobalFocus(true);
}

AppStateGameMode::~AppStateGameMode()
{
  the_desktop.removeChild("StatusArea");
  the_desktop.removeChild("InventoryArea");
  the_desktop.removeChild("MessageLogView");

  getStateMachine().removeObserver(*this, EventID::All);
}

void AppStateGameMode::execute()
{
  auto& game = getGameState();
  auto& game_data = game.data();

  // First, check for debug commands ready to be run.
  if (m_debug_buffer->get_enter())
  {
    /// Call the Lua interpreter with the command.
    std::string luaCommand = m_debug_buffer->get_buffer();
    Service<IMessageLog>::get().add("> " + luaCommand);
    if (luaL_dostring(the_lua_state, luaCommand.c_str()))
    {
      std::string result = lua_tostring(the_lua_state, -1);
      Service<IMessageLog>::get().add(result);
    }

    m_debug_buffer->clear_buffer();
  }

  bool ticked = game.processGameClockTick();

  // If the game clock ticked (player action started or is in progress)...
  if (ticked)
  {
    EntityId player = game.getPlayer();

    // Update view's cached tile data.
    m_map_view->update_tiles(player);

    // If the action completed, reset the inventory selection.
    if (!player->voluntaryActionIsPending() && !player->actionIsInProgress())
    {
      resetInventorySelection();
    }
  }
}

SFMLEventResult AppStateGameMode::handle_sfml_event(sf::Event& event)
{
  SFMLEventResult result = SFMLEventResult::Ignored;

  // Let the GUI handle events.
  if (result != SFMLEventResult::Handled)
  {
    result = the_desktop.handle_sfml_event(event);
  }

  return result;
}

std::string const& AppStateGameMode::getName()
{
  static std::string name = "AppStateGameMode";
  return name;
}

bool AppStateGameMode::initialize()
{
  auto& config = Service<IConfigSettings>::get();
  auto& game = getGameState();
  auto& game_data = game.data();

  // Create the player.
  EntityId player = getGameState().entities().create("Human");
  player->setProperName(config.get("player-name"));
  game.setPlayer(player);

  // Create the game map.
  /// @todo This shouldn't be hardcoded here
#ifdef NDEBUG
  MapId current_map_id = game.maps().create(64, 64);
#else
  MapId current_map_id = game.maps().create(20, 20);
#endif

  Map& game_map = game.maps().get(current_map_id);

  // Move player to start position on the map.
  auto& start_coords = game_map.getStartCoords();

  auto start_floor = game_map.getTile(start_coords).getTileContents();
  Assert("Game", start_floor, "starting tile floor doesn't exist");

  bool player_moved = player->move_into(start_floor);
  Assert("Game", player_moved, "player could not be moved into starting tile");

  // Set cursor to starting location.
  m_cursor_coords = start_coords;

  // Set the viewed inventory location to the player's location.
  m_inventory_area_shows_player = false;
  resetInventorySelection();

  // Set the map view.
  m_map_view = the_desktop.addChild(Service<IGraphicViews>::get().createMapView("MainMapView", game_map, the_desktop.getSize()));

  // Get the map ready.
  game_map.updateLighting();

  // Get the map view ready.
  m_map_view->update_tiles(player);
  m_map_view->update_things(player, 0);

  putMsg(tr("WELCOME_MSG"));

  return true;
}

bool AppStateGameMode::terminate()
{
  the_desktop.removeChild("MainMapView");

  return true;
}

GameState& AppStateGameMode::getGameState()
{
  return *m_game_state;
}

std::unordered_set<EventID> AppStateGameMode::registeredEvents() const
{
  auto events = AppState::registeredEvents();
  events.insert({
    App::EventAppWindowResized::id(),
    App::EventKeyPressed::id()
  });
  return events;
}


// === PROTECTED METHODS ======================================================
void AppStateGameMode::render_map(sf::RenderTexture& texture, int frame)
{
  auto& config = Service<IConfigSettings>::get();
  auto& game = getGameState();
  auto& game_data = game.data();

  texture.clear();

  EntityId player = game.getPlayer();
  EntityId location = player->getLocation();

  if (location == game.entities().get_mu())
  {
    throw std::runtime_error("Uh oh, the player's location appears to have been deleted!");
  }

  /// @todo We need a way to determine if the player is directly on a map,
  ///       and render either the map, or a container interior.
  ///       Should probably use an overridden "render_surroundings" method
  ///       for Entities.

  if (!player->isInsideAnotherEntity())
  {
    MapTile* tile = player->getMapTile();
    if (tile != nullptr)
    {
      Map& game_map = game.maps().get(tile->getMapId());
      IntVec2 tile_coords = tile->getCoords();
      RealVec2 player_pixel_coords = MapTile::getPixelCoords(tile_coords);
      RealVec2 cursor_pixel_coords = MapTile::getPixelCoords(m_cursor_coords);

      // Update entity vertex array.
      m_map_view->update_things(player, frame);

      if (m_current_input_state == GameInputState::CursorLook)
      {
        m_map_view->set_view(texture, cursor_pixel_coords, m_map_zoom_level);
        m_map_view->render_map(texture, frame);

        Color border_color = config.get("cursor-border-color");
        Color bg_color = config.get("cursor-bg-color");
        m_map_view->draw_highlight(texture,
                                   cursor_pixel_coords,
                                   border_color,
                                   bg_color,
                                   frame);
      }
      else
      {
        m_map_view->set_view(texture, player_pixel_coords, m_map_zoom_level);
        m_map_view->render_map(texture, frame);
      }
    }
  }
  texture.display();
}

bool AppStateGameMode::handle_key_press(App::EventKeyPressed const& key)
{
  auto& game = getGameState();
  auto& game_data = game.data();
  EntityId player = game.getPlayer();

  // *** Handle keys processed in any mode.
  if (!key.alt && !key.control)
  {
    if (key.code == sf::Keyboard::Key::Tilde)
    {
      switch (m_current_input_state)
      {
        case GameInputState::Map:
          m_current_input_state = GameInputState::MessageLog;
          the_desktop.getChild("MessageLogView").setGlobalFocus(true);
          return false;

        case GameInputState::MessageLog:
          m_current_input_state = GameInputState::Map;
          the_desktop.getChild("StatusArea").setGlobalFocus(true);
          return false;

        default:
          break;
      }
    }
  }

  // *** Handle keys unique to a particular focus.
  switch (m_current_input_state)
  {
    case GameInputState::TargetSelection:
      return handleKeyPressTargetSelection(player, key);

    case GameInputState::CursorLook:
      return handleKeyPressCursorLook(player, key);

    case GameInputState::Map:
    {
      std::unique_ptr<Actions::Action> p_action;

      std::vector<EntityId>& entities = m_inventory_selection->get_selected_things();
      int key_number = get_letter_key(key);
      Direction key_direction = get_direction_key(key);

      // *** No ALT, no CTRL, shift is irrelevant ****************************
      if (!key.alt && !key.control)
      {
        if (key_number != -1)
        {
          m_inventory_selection->toggle_selection(static_cast<InventorySlot>(key_number));
          return false;
        }
        else if (key.code == sf::Keyboard::Key::Tab)
        {
          m_inventory_area_shows_player = !m_inventory_area_shows_player;
          resetInventorySelection();
          return false;
        }
        else if (key.code == sf::Keyboard::Key::Escape)
        {
          putMsg(tr("QUIT_MSG"));
          return false;
        }
      }

      // *** No ALT, no CTRL, no SHIFT ***************************************
      if (!key.alt && !key.control && !key.shift)
      {
        if (key_direction != Direction::None)
        {
          if (key_direction == Direction::Self)
          {
            p_action.reset(new Actions::ActionWait(player));
            player->queueAction(std::move(p_action));
            return false;
          }
          else
          {
            p_action.reset(new Actions::ActionTurn(player));
            p_action->setTarget(key_direction);
            player->queueAction(std::move(p_action));

            p_action.reset(new Actions::ActionMove(player));
            p_action->setTarget(key_direction);
            player->queueAction(std::move(p_action));
            return false;
          }
        }
        else switch (key.code)
        {
          case sf::Keyboard::Key::BackSpace:
            resetInventorySelection();
            return false;

            // "/" - go to cursor look mode.
          case sf::Keyboard::Key::Slash:
            m_current_input_state = GameInputState::CursorLook;
            m_inventory_area_shows_player = false;
            resetInventorySelection();
            return false;

            // "-" - subtract quantity
          case sf::Keyboard::Key::Dash:
          case sf::Keyboard::Key::Subtract:
          {
            /// @todo Need a way to choose which inventory we're affecting.
            auto slot_count = m_inventory_selection->get_selected_slot_count();
            if (slot_count < 1)
            {
              putMsg(tr("QUANTITY_NEED_SOMETHING_SELECTED"));
            }
            else if (slot_count > 1)
            {
              putMsg(tr("QUANTITY_NEED_ONE_THING_SELECTED"));
            }
            else
            {
              m_inventory_selection->dec_selected_quantity();
            }
          }
          return false;

          // "+"/"=" - add quantity
          case sf::Keyboard::Key::Equal:
          case sf::Keyboard::Key::Add:
          {
            auto slot_count = m_inventory_selection->get_selected_slot_count();
            if (slot_count < 1)
            {
              putMsg(tr("QUANTITY_NEED_SOMETHING_SELECTED"));
            }
            else if (slot_count > 1)
            {
              putMsg(tr("QUANTITY_NEED_ONE_THING_SELECTED"));
            }
            else
            {
              m_inventory_selection->inc_selected_quantity();
            }
          }
          return false;

          case sf::Keyboard::Key::LBracket:
          {
            EntityId entity = m_inventory_selection->get_viewed();
            EntityId location = entity->getLocation();
            if (location != getGameState().entities().get_mu())
            {
              m_inventory_selection->set_viewed(location);
            }
            else
            {
              putMsg(tr("AT_TOP_OF_INVENTORY_TREE"));
            }
            return false;
          }

          case sf::Keyboard::Key::RBracket:
          {
            auto slot_count = m_inventory_selection->get_selected_slot_count();

            if (slot_count > 0)
            {
              EntityId entity = m_inventory_selection->get_selected_things().at(0);
              if (static_cast<int>(entity->getIntrinsic("inventory-size", 0)) != 0)
              {
                if (!entity->canHaveActionDoneBy(EntityId::Mu(), Actions::ActionOpen::prototype) ||
                    entity->getModifiedProperty("open", true))
                {
                  if (!entity->canHaveActionDoneBy(EntityId::Mu(), Actions::ActionLock::prototype) ||
                      !entity->getModifiedProperty("locked", false))
                  {
                    m_inventory_selection->set_viewed(entity);
                  }
                  else // if (container.is_locked())
                  {
                    std::string message = StringTransforms::makeString(player, entity, "THE_FOO_IS_LOCKED");
                    putMsg(message);
                  }
                }
                else // if (!container.is_open())
                {
                  /// @todo Need a way to make this cleaner.
                  std::string message = StringTransforms::makeString(player, entity, "THE_FOO_IS_CLOSED");
                  putMsg(message);
                }
              }
              else // if (!entity.is_container())
              {
                /// @todo This probably doesn't belong here.
                std::string message = StringTransforms::makeString(player, entity, "THE_FOO_IS_NOT_A_CONTAINER");
                putMsg(message);
              }
            }
            else
            {
              putMsg(tr("NOTHING_IS_SELECTED"));
            }
            return false;
          }

          case sf::Keyboard::Key::Comma:
            /// @todo This is a copy of CTRL-G; split out into separate method.
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_PICKUP_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionGet(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

          default:
            break;
        } // end switch (key.code)
      } // end if (!key.alt && !key.control && !key.shift)

        // *** No ALT, YES CTRL, SHIFT is irrelevant ******************************
      if (!key.alt && key.control)
      {
        if (key_direction != Direction::None)
        {
          if (key_direction == Direction::Self)
          {
            p_action.reset(new Actions::ActionWait(player));
            player->queueAction(std::move(p_action));
            return false;
          }
          else
          {
            // CTRL-arrow -- Turn without moving
            p_action.reset(new Actions::ActionTurn(player));
            p_action->setTarget(key_direction);
            player->queueAction(std::move(p_action));
            return false;
          }
        }
        else switch (key.code)
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
            m_map_zoom_level = 1.0f;
            break;

            // CTRL-A -- attire/adorn
          case sf::Keyboard::Key::A:    // Attire
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_WEAR_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionWear(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-C -- close items
          case sf::Keyboard::Key::C:    // Close
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_action_in_progress.reset(new Actions::ActionClose(player));
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_DIRECTION"), { tr("VERB_CLOSE_2") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            else
            {
              // Item(s) specified, so proceed with items.
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionClose(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-D -- drop items
          case sf::Keyboard::Key::D:    // Drop
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_DROP_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionDrop(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-E -- eat items
          case sf::Keyboard::Key::E:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_EAT_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionEat(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-F -- fill item(s)
          case sf::Keyboard::Key::F:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_FIRST"), { tr("VERB_FILL_2") }));
            }
            else if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_FILL_2") }));
            }
            else
            {
              m_action_in_progress.reset(new Actions::ActionFill(player));
              m_action_in_progress->setObject(entities.front());
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_OR_DIRECTION"), { tr("VERB_FILL_GER") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            return false;

            // CTRL-G -- get (pick up) items
          case sf::Keyboard::Key::G:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_PICKUP_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionGet(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-H -- hurl items
          case sf::Keyboard::Key::H:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_FIRST"), { tr("VERB_THROW_2") }));
            }
            else if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_THROW_2") }));
            }
            else
            {
              m_action_in_progress.reset(new Actions::ActionHurl(player));
              m_action_in_progress->setObject(entities.front());
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_DIRECTION"), { tr("VERB_THROW_2") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            return false;

            // CTRL-I -- inscribe with an item
          case sf::Keyboard::Key::I:
            if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_WRITE_2") }));
            }
            else
            {
              m_action_in_progress.reset(new Actions::ActionInscribe(player));
              if (entities.size() != 0)
              {
                m_action_in_progress->setObject(entities.front());
              }

              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_OR_DIRECTION"), { tr("VERB_WRITE_GER") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            return false;

            // CTRL-M -- mix items
          case sf::Keyboard::Key::M:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_TWO_ITEMS_FIRST"), { tr("VERB_MIX_2") }));
            }
            else if (entities.size() != 2)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_EXACTLY_TWO_AT_A_TIME"), { tr("VERB_MIX_2") }));
            }
            else
            {
              p_action.reset(new Actions::ActionMix(player));
              p_action->setObjects(entities);
              player->queueAction(std::move(p_action));
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

          case sf::Keyboard::Key::O:    // Open
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_action_in_progress.reset(new Actions::ActionOpen(player));
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_DIRECTION"), { tr("VERB_OPEN_2") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            else
            {
              // Item(s) specified, so proceed with items.
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionOpen(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-P -- put item in another item
          case sf::Keyboard::Key::P:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_STORE_2") }));
            }
            else
            {
              m_action_in_progress.reset(new Actions::ActionPutInto(player));
              m_action_in_progress->setObjects(entities);
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_CONTAINER"), { tr("VERB_STORE_DESC") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            return false;

            // CTRL-Q -- quaff (drink) from items
          case sf::Keyboard::Key::Q:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_DRINK_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionQuaff(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-R -- read an item
            /// @todo Maybe allow reading from direction?
          case sf::Keyboard::Key::R:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_READ_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionRead(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-S -- shoot items
          case sf::Keyboard::Key::S:
            /// @todo Skip the item check here, as we want to shoot our wielded weapon
            /// if no item is selected.
            if (entities.size() < 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_FIRST"), { tr("VERB_SHOOT_2") }));
            }
            if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_SHOOT_2") }));
            }
            else
            {
              /// @todo If no items are selected, fire your wielded item.
              ///       Otherwise, wield the selected item and fire it.
              m_action_in_progress.reset(new Actions::ActionShoot(player));
              m_action_in_progress->setObject(entities.front());
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_DIRECTION"), { tr("VERB_SHOOT_2") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            return false;

            // CTRL-T -- take item out of container
          case sf::Keyboard::Key::T:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_REMOVE_2") }));
            }
            else
            {
              p_action.reset(new Actions::ActionTakeOut(player));
              p_action->setObjects(entities);
              player->queueAction(std::move(p_action));
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-U -- use an item
          case sf::Keyboard::Key::U:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEMS_FIRST"), { tr("VERB_USE_2") }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionUse(player));
                p_action->setObject(entity);
                player->queueAction(std::move(p_action));
              }
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-W -- wield item
          case sf::Keyboard::Key::W:
            if (entities.size() == 0)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ITEM_FIRST"), { tr("VERB_WIELD_2") }));
            }
            else if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_WIELD_2") }));
            }
            else
            {
              p_action.reset(new Actions::ActionWield(player));
              p_action->setObject(entities.front());
              player->queueAction(std::move(p_action));
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-X -- Xplicit attack
          case sf::Keyboard::Key::X:
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_action_in_progress.reset(new Actions::ActionAttack(player));
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_DIRECTION"), { tr("VERB_ATTACK_2") }));
              m_current_input_state = GameInputState::TargetSelection;
              m_inventory_selection->clear_selected_slots();
            }
            else if (entities.size() > 1)
            {
              putMsg(StringTransforms::makeString(player, EntityId::Mu(), tr("CHOOSE_ONLY_ONE_AT_A_TIME"), { tr("VERB_ATTACK_2") }));
            }
            else
            {
              // Item(s) specified, so proceed with items.
              p_action.reset(new Actions::ActionAttack(player));
              p_action->setObject(entities.front());
              player->queueAction(std::move(p_action));
              m_inventory_area_shows_player = false;
              resetInventorySelection();
            }
            return false;

          default:
            break;
        } // end switch
      } // end if (!key.alt && key.control)

        // *** YES ALT, no CTRL, SHIFT is irrelevant ******************************
        // Right now we don't have any ALT-* combinations.
#if 0
      if (key.alt && !key.control)
      {
        switch (key.code)
        {
          default:
            break;
        }
      }
#endif

      // *** YES ALT, YES CTRL, SHIFT is irrelevant *****************************
      if (key.alt && key.control)
      {
        if (key_direction != Direction::None)
        {
          if (key_direction == Direction::Self)
          {
            p_action.reset(new Actions::ActionWait(player));
            player->queueAction(std::move(p_action));
            return false;
          }
          else
          {
            // CTRL-ALT-arrow -- Move without turning
            p_action.reset(new Actions::ActionMove(player));
            p_action->setTarget(key_direction);
            player->queueAction(std::move(p_action));
            return false;
          }
        }
        else switch (key.code)
        {
          // There are no other CTRL-ALT key combinations right now.
          default:
            return false;
        }
      }

      break;
    } // end case GameInputState::Map

    default:
      break;
  } // end switch (m_current_input_state)

  return true;
}

bool AppStateGameMode::handle_mouse_wheel(App::EventMouseWheelMoved const& wheel)
{
  add_zoom(wheel.delta * 0.05f);
  return false;
}

void AppStateGameMode::add_zoom(float zoom_amount)
{
  float current_zoom_level = m_map_zoom_level;

  current_zoom_level += zoom_amount;

  if (current_zoom_level < 0.1f)
  {
    current_zoom_level = 0.1f;
  }

  if (current_zoom_level > 3.0f)
  {
    current_zoom_level = 3.0f;
  }

  m_map_zoom_level = current_zoom_level;
}

EventResult AppStateGameMode::onEvent_NVI(Event const& event)
{
  if (event.getId() == App::EventAppWindowResized::id())
  {
    auto info = static_cast<App::EventAppWindowResized const&>(event);
    the_desktop.setSize({ info.new_size.x, info.new_size.y });
    the_desktop.getChild("MessageLogView").setRelativeDimensions(calcMessageLogDims());
    the_desktop.getChild("InventoryArea").setRelativeDimensions(calcInventoryDims());
    the_desktop.getChild("StatusArea").setRelativeDimensions(calcStatusAreaDims());
    return{ EventHandled::Yes, ContinueBroadcasting::Yes };
  }
  else if (event.getId() == App::EventKeyPressed::id())
  {
    auto info = static_cast<App::EventKeyPressed const&>(event);
    bool keep_broadcasting = handle_key_press(info);
    return{ 
      EventHandled::Yes, 
      (keep_broadcasting ? ContinueBroadcasting::Yes : ContinueBroadcasting::No)
    };
  }
  else if (event.getId() == App::EventMouseWheelMoved::id())
  {
    auto info = static_cast<App::EventMouseWheelMoved const&>(event);
    bool keep_broadcasting = handle_mouse_wheel(info);
    return{
      EventHandled::Yes,
      (keep_broadcasting ? ContinueBroadcasting::Yes : ContinueBroadcasting::No)
    };
  }

  /// @todo WRITE ME
  return{ EventHandled::No, ContinueBroadcasting::Yes };
}

sf::IntRect AppStateGameMode::calcMessageLogDims()
{
  sf::IntRect messageLogDims;
  auto& config = Service<IConfigSettings>::get();

  int inventory_area_width = config.get("inventory-area-width");
  int messagelog_area_height = config.get("messagelog-area-height");
  messageLogDims.width = m_app_window.getSize().x - (inventory_area_width + 24);
  messageLogDims.height = messagelog_area_height - 10;
  //messageLogDims.height = static_cast<int>(m_app_window.getSize().y * 0.25f) - 10;
  messageLogDims.left = 12;
  messageLogDims.top = 5;
  return messageLogDims;
}

void AppStateGameMode::resetInventorySelection()
{
  auto& game = getGameState();
  auto& game_data = game.data();
  EntityId player = game.getPlayer();
  Map& game_map = game.maps().get(player->getMapId());

  if (m_inventory_area_shows_player == true)
  {
    m_inventory_selection->set_viewed(player);
  }
  else
  {
    if (m_current_input_state == GameInputState::CursorLook)
    {
      EntityId floor_id = game_map.getTile(m_cursor_coords).getTileContents();
      m_inventory_selection->set_viewed(floor_id);
    }
    else
    {
      m_inventory_selection->set_viewed(player->getLocation());
    }
  }
}

sf::IntRect AppStateGameMode::calcStatusAreaDims()
{
  sf::IntRect statusAreaDims;
  sf::IntRect invAreaDims = the_desktop.getChild("InventoryArea").getRelativeDimensions();
  auto& config = Service<IConfigSettings>::get();

  statusAreaDims.width = m_app_window.getSize().x -
    (invAreaDims.width + 24);
  statusAreaDims.height = config.get("status-area-height");
  statusAreaDims.top = m_app_window.getSize().y - (config.get("status-area-height") + 5);
  statusAreaDims.left = 12;
  return statusAreaDims;
}

sf::IntRect AppStateGameMode::calcInventoryDims()
{
  sf::IntRect messageLogDims = the_desktop.getChild("MessageLogView").getRelativeDimensions();
  sf::IntRect inventoryAreaDims;
  auto& config = Service<IConfigSettings>::get();

  inventoryAreaDims.width = config.get("inventory-area-width");
  inventoryAreaDims.height = m_app_window.getSize().y - 10;
  inventoryAreaDims.left = m_app_window.getSize().x - (inventoryAreaDims.width + 3);
  inventoryAreaDims.top = 5;

  return inventoryAreaDims;
}

bool AppStateGameMode::moveCursor(Direction direction)
{
  auto& game = getGameState();
  auto& game_data = game.data();
  EntityId player = game.getPlayer();
  Map& game_map = game.maps().get(player->getMapId());
  bool result;

  result = game_map.calcCoords(m_cursor_coords, direction, m_cursor_coords);

  return result;
}

bool AppStateGameMode::handleKeyPressTargetSelection(EntityId player, App::EventKeyPressed const& key)
{
  int key_number = get_letter_key(key);
  Direction key_direction = get_direction_key(key);

  if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Tab)
  {
    m_inventory_area_shows_player = !m_inventory_area_shows_player;
    resetInventorySelection();
    return false;
  }

  if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Escape)
  {
    putMsg(tr("ABORTED"));
    m_inventory_area_shows_player = false;
    resetInventorySelection();
    m_current_input_state = GameInputState::Map;
    return false;
  }

  if (m_action_in_progress && m_action_in_progress->hasTrait(Actions::Trait::CanBeSubjectVerbObjectPrepositionTarget))
  {
    if (!key.alt && !key.control && key_number != -1)
    {
      m_action_in_progress->setTarget(m_inventory_selection->getEntity(static_cast<InventorySlot>(key_number)));
      player->queueAction(std::move(m_action_in_progress));
      m_inventory_area_shows_player = false;
      resetInventorySelection();
      m_current_input_state = GameInputState::Map;
      return false;
    }
  } // end if (action_in_progress.target_can_be_thing)

  if (m_action_in_progress && (m_action_in_progress->hasTrait(Actions::Trait::CanBeSubjectVerbDirection) ||
                               m_action_in_progress->hasTrait(Actions::Trait::CanBeSubjectVerbObjectPrepositionDirection)))
  {
    if (!key.alt && !key.control && key_direction != Direction::None)
    {
      m_action_in_progress->setTarget(key_direction);
      player->queueAction(std::move(m_action_in_progress));
      m_inventory_area_shows_player = false;
      resetInventorySelection();
      m_current_input_state = GameInputState::Map;
      return false;
    }
  }

  return true;
}

bool AppStateGameMode::handleKeyPressCursorLook(EntityId player, App::EventKeyPressed const& key)
{
  // *** NON-MODIFIED KEYS ***********************************************
  if (!key.alt && !key.control && !key.shift)
  {
    switch (key.code)
    {
      case sf::Keyboard::Key::Up:
        moveCursor(Direction::North);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::PageUp:
        moveCursor(Direction::Northeast);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Right:
        moveCursor(Direction::East);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::PageDown:
        moveCursor(Direction::Southeast);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Down:
        moveCursor(Direction::South);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::End:
        moveCursor(Direction::Southwest);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Left:
        moveCursor(Direction::West);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Home:
        moveCursor(Direction::Northwest);
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

        // "/" - go back to Map focus.
      case sf::Keyboard::Key::Slash:
        m_current_input_state = GameInputState::Map;
        m_inventory_area_shows_player = false;
        resetInventorySelection();
        return false;

      default:
        break;
    } // end switch (key.code)
  } // end if (no modifier keys)

  return true;
}