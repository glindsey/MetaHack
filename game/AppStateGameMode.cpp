#include "stdafx.h"

#include "game/AppStateGameMode.h"

#include "actions/Action.h"
#include "AssertHelper.h"
#include "components/ComponentManager.h"
#include "design_patterns/Event.h"
#include "entity/EntityFactory.h"
#include "game/App.h"
#include "game/AppState.h"
#include "game/GameState.h"
#include "game_windows/MessageLogView.h"
#include "game_windows/StatusArea.h"
#include "inventory/InventoryArea.h"
#include "inventory/InventorySelection.h"
#include "keybuffer/KeyBuffer.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "map/MapStandard2DView.h"
#include "maptile/MapTile.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/Standard2DGraphicViews.h"
#include "services/IStrings.h"
#include "services/MessageLog.h"
#include "state_machine/StateMachine.h"
#include "systems/Manager.h"
#include "systems/SystemDirector.h"
#include "systems/SystemLighting.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemSenseSight.h"
#include "systems/SystemGeometry.h"
#include "utilities/GetLetterKey.h"
#include "utilities/Shortcuts.h"
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

AppStateGameMode::AppStateGameMode(StateMachine& state_machine, 
                                   sf::RenderWindow& m_appWindow)
  :
  AppState(state_machine,
           { App::EventAppWindowResized::id,
             UIEvents::EventKeyPressed::id },
           "AppStateGameMode",
           std::bind(&AppStateGameMode::render_map, this, std::placeholders::_1, std::placeholders::_2)),
  m_appWindow{ m_appWindow },
  m_debugBuffer{ NEW KeyBuffer() },
  m_gameState{ NEW GameState({}) },
  m_systemManager{ NEW Systems::Manager(*m_gameState) },
  m_inventorySelection{ NEW InventorySelection() },
  m_windowInFocus{ true },
  m_inventoryAreaShowsPlayer{ false },
  m_mapZoomLevel{ 1.0f },
  m_currentInputState{ GameInputState::Map },
  m_cursorCoords{ 0, 0 }
{
  App::instance().addObserver(*this, EventID::All);

  the_desktop.addChild(NEW MessageLogView(the_desktop, 
                                          "MessageLogView", 
                                          S<IMessageLog>(), 
                                          *m_debugBuffer, 
                                          calcMessageLogDims()))->setFlag("titlebar", true);
  the_desktop.addChild(NEW InventoryArea(the_desktop, 
                                         "InventoryArea", 
                                         *m_inventorySelection, 
                                         calcInventoryDims(), 
                                         *m_gameState))->setFlag("titlebar", true);
  the_desktop.addChild(NEW StatusArea(the_desktop, 
                                      "StatusArea", 
                                      calcStatusAreaDims(), 
                                      *m_gameState))->setGlobalFocus(true);

  // Create the standard map views provider.
  /// @todo Make this configurable.
  Service<IGraphicViews>::provide(NEW Standard2DGraphicViews());
}

AppStateGameMode::~AppStateGameMode()
{
  the_desktop.removeChild("StatusArea");
  the_desktop.removeChild("InventoryArea");
  the_desktop.removeChild("MessageLogView");

  App::instance().removeObserver(*this, EventID::All);
}

void AppStateGameMode::execute()
{
  auto& game = gameState();
  auto& components = game.components();

  // First, check for debug commands ready to be run.
  if (m_debugBuffer->get_enter())
  {
    /// Call the Lua interpreter with the command.
    std::string luaCommand = m_debugBuffer->get_buffer();
    S<IMessageLog>().add("> " + luaCommand);

    /// DEBUG: If the command is "dump", write out gamestate JSON to a file.
    /// @todo Remove this, or make it a Lua function instead.
    std::string command = boost::to_lower_copy(luaCommand);
    if (command == "dump")
    {
      S<IMessageLog>().add("Dumping game state to dump.json...");
      json gameStateJSON = game;
      std::ofstream of("dump.json");
      of << gameStateJSON.dump(2);
      S<IMessageLog>().add("...Dump complete.");
    }
    else
    {
      if (luaL_dostring(m_gameState->lua().state(), luaCommand.c_str()))
      {
        std::string result = lua_tostring(m_gameState->lua().state(), -1);
        S<IMessageLog>().add(result);
      }
    }

    m_debugBuffer->clear_buffer();
  }

  EntityId player = components.globals.player();

  // If there's a player action waiting or in progress...
  if (components.activity.existsFor(player) && 
      components.activity[player].actionPendingOrInProgress())
  {
    // Update map used for systems that care about it.
    /// @todo This should no longer be required thanks to events, try removing it
    auto map = components.position.existsFor(player) ? components.position[player].map() : "";
    m_systemManager->director().setMap(map);
    m_systemManager->lighting().setMap(map);
    m_systemManager->senseSight().setMap(map);

    // Run systems.
    m_systemManager->runOneCycle();

    // Update view's cached tile data.
    m_mapView->updateTiles(player, m_systemManager->lighting());

    // If the action completed, reset the inventory selection.
    if (!components.activity.existsFor(player) || 
        !components.activity[player].actionPendingOrInProgress())
    {
      resetInventorySelection();
    }
  }
}

bool AppStateGameMode::initialize()
{
  auto& config = S<IConfigSettings>();
  auto& game = gameState();

  // Create the player.
  EntityId player = gameState().entities().create({ "Human" });
  COMPONENTS.properName[player] = config.get("player-name").get<std::string>();
  COMPONENTS.globals.setPlayer(player);

  // Create the game map.
  /// @todo This shouldn't be hardcoded here
#ifdef NDEBUG
  MapID currentMapId = game.maps().create(64, 64);
#else
  MapID currentMapId = game.maps().create(20, 20);
#endif

  Map& game_map = game.maps().get(currentMapId);

  // Initialize systems that need initializing.
  m_systemManager->lighting().setMap(currentMapId);
  m_systemManager->senseSight().setMap(currentMapId);

  // Move player to start position on the map.
  auto& start_coords = game_map.getStartCoords();

  auto start_floor = game_map.getTile(start_coords).getSpaceEntity();
  Assert("Game", start_floor, "starting tile floor doesn't exist");

  bool player_moved = m_systemManager->geometry().moveEntityInto(player, start_floor);
  Assert("Game", player_moved, "player could not be moved into starting tile");

  // Set cursor to starting location.
  m_cursorCoords = start_coords;

  // Set the viewed inventory location to the player's location.
  m_inventoryAreaShowsPlayer = false;
  resetInventorySelection();

  // Set the map view.
  m_mapView = the_desktop.addChild(S<IGraphicViews>().createMapView(the_desktop, 
                                                                    "MainMapView", 
                                                                    game_map, 
                                                                    the_desktop.getSize()));

  // Get the map view ready.
  m_mapView->updateTiles(player, m_systemManager->lighting());
  m_mapView->updateEntities(player, m_systemManager->lighting(), 0);

  // Run all systems once.
  m_systemManager->runOneCycle();

  putMsg(tr("WELCOME_MSG"));

  return true;
}

bool AppStateGameMode::terminate()
{
  auto mapView = the_desktop.removeChild("MainMapView");
  mapView->removeObserver(the_desktop, EventID::All);

  return true;
}

GameState& AppStateGameMode::gameState()
{
  return *m_gameState;
}

Systems::Manager& AppStateGameMode::systems()
{
  return *m_systemManager;
}

// === PROTECTED METHODS ======================================================
void AppStateGameMode::render_map(sf::RenderTexture& texture, int frame)
{
  auto& config = S<IConfigSettings>();
  auto& game = gameState();

  texture.clear();

  EntityId player = game.components().globals.player();
  EntityId location = game.components().position[player].parent();

  if (location == EntityId::Void)
  {
    throw std::runtime_error("Uh oh, the player's location appears to have been deleted!");
  }

  /// @todo We need a way to determine if the player is directly on a map,
  ///       and render either the map, or a container interior.
  ///       Should probably use an overridden "render_surroundings" method
  ///       for Entities.

  if (COMPONENTS.position.existsFor(player) && !COMPONENTS.position[player].isInsideAnotherEntity())
  {
    auto& position = COMPONENTS.position[player];
    RealVec2 player_pixel_coords = MapTile::getPixelCoords(position.coords());
    RealVec2 cursor_pixel_coords = MapTile::getPixelCoords(m_cursorCoords);

    // Update entity vertex array.
    m_mapView->updateEntities(player, m_systemManager->lighting(), frame);

    if (m_currentInputState == GameInputState::CursorLook)
    {
      m_mapView->set_view(texture, cursor_pixel_coords, m_mapZoomLevel);
      m_mapView->render_map(texture, frame);

      Color border_color = config.get("cursor-border-color");
      Color bg_color = config.get("cursor-bg-color");
      m_mapView->draw_highlight(texture,
                                cursor_pixel_coords,
                                border_color,
                                bg_color,
                                frame);
    }
    else
    {
      m_mapView->set_view(texture, player_pixel_coords, m_mapZoomLevel);
      m_mapView->render_map(texture, frame);
    }
  }

  texture.display();
}

bool AppStateGameMode::handle_key_press(UIEvents::EventKeyPressed const& key)
{
  auto& game = gameState();
  auto& components = game.components();
  auto& director = systems().director();
  auto& narrator = systems().narrator();

  EntityId player = game.components().globals.player();

  // *** Handle keys processed in any mode.
  if (!key.alt && !key.control)
  {
    if (key.code == sf::Keyboard::Key::Tilde)
    {
      switch (m_currentInputState)
      {
        case GameInputState::Map:
          m_currentInputState = GameInputState::MessageLog;
          the_desktop.getChild("MessageLogView").setGlobalFocus(true);
          return false;

        case GameInputState::MessageLog:
          m_currentInputState = GameInputState::Map;
          the_desktop.getChild("StatusArea").setGlobalFocus(true);
          return false;

        default:
          break;
      }
    }
  }

  // *** Handle keys unique to a particular focus.
  switch (m_currentInputState)
  {
    case GameInputState::TargetSelection:
      return handleKeyPressTargetSelection(player, key);

    case GameInputState::CursorLook:
      return handleKeyPressCursorLook(player, key);

    case GameInputState::Map:
    {
      std::unique_ptr<Actions::Action> p_action;

      std::vector<EntityId>& entities = m_inventorySelection->getSelectedThings();
      int key_number = get_letter_key(key);
      Direction key_direction = get_direction_key(key);

      // *** No ALT, no CTRL, shift is irrelevant ****************************
      if (!key.alt && !key.control)
      {
        if (key_number != -1)
        {
          m_inventorySelection->toggleSelection(static_cast<InventorySlot>(key_number));
          return false;
        }
        else if (key.code == sf::Keyboard::Key::Tab)
        {
          m_inventoryAreaShowsPlayer = !m_inventoryAreaShowsPlayer;
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
            director.queueEntityAction(player, std::move(p_action));
            return false;
          }
          else
          {
            p_action.reset(new Actions::ActionTurn(player));
            p_action->setTarget(key_direction);
            director.queueEntityAction(player, std::move(p_action));

            p_action.reset(new Actions::ActionMove(player));
            p_action->setTarget(key_direction);
            director.queueEntityAction(player, std::move(p_action));
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
            m_currentInputState = GameInputState::CursorLook;
            m_inventoryAreaShowsPlayer = false;
            resetInventorySelection();
            return false;

            // "-" - subtract quantity
          case sf::Keyboard::Key::Dash:
          case sf::Keyboard::Key::Subtract:
          {
            /// @todo Need a way to choose which inventory we're affecting.
            auto slot_count = m_inventorySelection->getSelectedSlotCount();
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
              m_inventorySelection->decSelectedQuantity();
            }
          }
          return false;

          // "+"/"=" - add quantity
          case sf::Keyboard::Key::Equal:
          case sf::Keyboard::Key::Add:
          {
            auto slot_count = m_inventorySelection->getSelectedSlotCount();
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
              m_inventorySelection->incSelectedQuantity();
            }
          }
          return false;

          case sf::Keyboard::Key::LBracket:
          {
            EntityId entity = m_inventorySelection->getViewed();
            EntityId location = components.position[entity].parent();
            if (location != EntityId::Void)
            {
              m_inventorySelection->setViewed(location);
            }
            else
            {
              putMsg(tr("AT_TOP_OF_INVENTORY_TREE"));
            }
            return false;
          }

          case sf::Keyboard::Key::RBracket:
          {
            auto slot_count = m_inventorySelection->getSelectedSlotCount();

            if (slot_count > 0)
            {
              EntityId entity = m_inventorySelection->getSelectedThings().at(0);
              if (components.inventory.existsFor(entity))
              {
                if (!components.openable.existsFor(entity) ||
                    components.openable[entity].isOpen())
                {
                  if (!components.lockable.existsFor(entity) ||
                      !components.lockable[entity].isLocked())
                  {
                    m_inventorySelection->setViewed(entity);
                  }
                  else // if (container.is_locked())
                  {
                    putMsg(narrator.makeTr("THE_FOO_IS_LOCKED", { { "object", entity } }));
                  }
                }
                else // if (!container.is_open())
                {
                  putMsg(narrator.makeTr("THE_FOO_IS_CLOSED", { { "object", entity } }));
                }
              }
              else // if (!entity.is_container())
              {
                putMsg(narrator.makeTr("THE_FOO_IS_NOT_A_CONTAINER", { { "object", entity } }));
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
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_PICKUP_2") } }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionGet(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
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
            director.queueEntityAction(player, std::move(p_action));
            return false;
          }
          else
          {
            // CTRL-arrow -- Turn without moving
            p_action.reset(new Actions::ActionTurn(player));
            p_action->setTarget(key_direction);
            director.queueEntityAction(player, std::move(p_action));
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
            m_mapZoomLevel = 1.0f;
            break;

            // CTRL-A -- attire/adorn
          case sf::Keyboard::Key::A:    // Attire
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { {"verb", tr("VERB_WEAR_2") } }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionWear(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-C -- close items
          case sf::Keyboard::Key::C:    // Close
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_actionInProgress.reset(new Actions::ActionClose(player));
              putMsg(narrator.makeTr("CHOOSE_DIRECTION", { {"verb", tr("VERB_CLOSE_2")} }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            else
            {
              // Item(s) specified, so proceed with items.
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionClose(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-D -- drop items
          case sf::Keyboard::Key::D:    // Drop
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { {"verb", "VERB_DROP_2"} }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionDrop(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-E -- eat items
          case sf::Keyboard::Key::E:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { {"verb", "VERB_EAT_2"} }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionEat(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-F -- fill item(s)
          case sf::Keyboard::Key::F:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEM_FIRST", { {"verb", tr("VERB_FILL_2")} }));
            }
            else if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { {"verb", tr("VERB_FILL_2")} }));
            }
            else
            {
              m_actionInProgress.reset(new Actions::ActionFill(player));
              m_actionInProgress->setObject(entities.front());
              putMsg(narrator.makeTr("CHOOSE_ITEM_OR_DIRECTION", { {"verb", tr("VERB_FILL_GER")} }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            return false;

            // CTRL-G -- get (pick up) items
          case sf::Keyboard::Key::G:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { {"verb", tr("VERB_PICKUP_2")} }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionGet(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-H -- hurl items
          case sf::Keyboard::Key::H:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEM_FIRST", { { "verb", tr("VERB_THROW_2") } }));
            }
            else if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { { "verb", tr("VERB_THROW_2") } }));
            }
            else
            {
              m_actionInProgress.reset(new Actions::ActionHurl(player));
              m_actionInProgress->setObject(entities.front());
              putMsg(narrator.makeTr("CHOOSE_DIRECTION", { { "verb", tr("VERB_THROW_2") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            return false;

            // CTRL-I -- inscribe with an item
          case sf::Keyboard::Key::I:
            if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { { "verb", tr("VERB_WRITE_2") } }));
            }
            else
            {
              m_actionInProgress.reset(new Actions::ActionInscribe(player));
              if (entities.size() != 0)
              {
                m_actionInProgress->setObject(entities.front());
              }

              putMsg(narrator.makeTr("CHOOSE_ITEM_OR_DIRECTION", { { "verb", tr("VERB_WRITE_GER") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            return false;

            // CTRL-M -- mix items
          //case sf::Keyboard::Key::M:
          //  if (entities.size() == 0)
          //  {
          //    putMsg(StringTransforms::makeString(player, EntityId::Void, tr("CHOOSE_TWO_ITEMS_FIRST"), { tr("VERB_MIX_2") }));
          //  }
          //  else if (entities.size() != 2)
          //  {
          //    putMsg(StringTransforms::makeString(player, EntityId::Void, tr("CHOOSE_EXACTLY_TWO_AT_A_TIME"), { tr("VERB_MIX_2") }));
          //  }
          //  else
          //  {
          //    p_action.reset(new Actions::ActionMix(player));
          //    p_action->setObjects(entities);
          //    director.queueEntityAction(player, p_action));
          //    m_inventoryAreaShowsPlayer = false;
          //    resetInventorySelection();
          //  }
          //  return false;

          case sf::Keyboard::Key::O:    // Open
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_actionInProgress.reset(new Actions::ActionOpen(player));
              putMsg(narrator.makeTr("CHOOSE_DIRECTION", { { "verb", tr("VERB_OPEN_2") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            else
            {
              // Item(s) specified, so proceed with items.
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionOpen(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-P -- put item in another item
          case sf::Keyboard::Key::P:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_STORE_2") } }));
            }
            else
            {
              m_actionInProgress.reset(new Actions::ActionPutInto(player));
              m_actionInProgress->setObjects(entities);
              putMsg(narrator.makeTr("CHOOSE_CONTAINER", { { "verb", tr("VERB_STORE_DESC") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            return false;

            // CTRL-Q -- quaff (drink) from items
          case sf::Keyboard::Key::Q:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_DRINK_2") }  }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionQuaff(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-R -- read an item
            /// @todo Maybe allow reading from direction?
          case sf::Keyboard::Key::R:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_READ_2") } }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionRead(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-S -- shoot items
          case sf::Keyboard::Key::S:
            /// @todo Skip the item check here, as we want to shoot our wielded weapon
            /// if no item is selected.
            if (entities.size() < 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEM_FIRST", { { "verb", tr("VERB_SHOOT_2") } }));
            }
            if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { { "verb", tr("VERB_SHOOT_2") } }));
            }
            else
            {
              /// @todo If no items are selected, fire your wielded item.
              ///       Otherwise, wield the selected item and fire it.
              m_actionInProgress.reset(new Actions::ActionShoot(player));
              m_actionInProgress->setObject(entities.front());
              putMsg(narrator.makeTr("CHOOSE_DIRECTION", { { "verb", tr("VERB_SHOOT_2") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            return false;

            // CTRL-T -- take item out of container
          case sf::Keyboard::Key::T:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_REMOVE_2") } }));
            }
            else
            {
              p_action.reset(new Actions::ActionTakeOut(player));
              p_action->setObjects(entities);
              director.queueEntityAction(player, std::move(p_action));
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-U -- use an item
          case sf::Keyboard::Key::U:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEMS_FIRST", { { "verb", tr("VERB_USE_2") } }));
            }
            else
            {
              for (auto entity : entities)
              {
                p_action.reset(new Actions::ActionUse(player));
                p_action->setObject(entity);
                director.queueEntityAction(player, std::move(p_action));
              }
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-W -- wield item
          case sf::Keyboard::Key::W:
            if (entities.size() == 0)
            {
              putMsg(narrator.makeTr("CHOOSE_ITEM_FIRST", { { "verb", tr("VERB_WIELD_2") } }));
            }
            else if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { { "verb", tr("VERB_WIELD_2") } }));
            }
            else
            {
              p_action.reset(new Actions::ActionWield(player));
              p_action->setObject(entities.front());
              director.queueEntityAction(player, std::move(p_action));
              m_inventoryAreaShowsPlayer = false;
              resetInventorySelection();
            }
            return false;

            // CTRL-X -- Xplicit attack
          case sf::Keyboard::Key::X:
            if (entities.size() == 0)
            {
              // No item specified, so ask for a direction.
              m_actionInProgress.reset(new Actions::ActionAttack(player));
              putMsg(narrator.makeTr("CHOOSE_DIRECTION", { { "verb", tr("VERB_ATTACK_2") } }));
              m_currentInputState = GameInputState::TargetSelection;
              m_inventorySelection->clearSelectedSlots();
            }
            else if (entities.size() > 1)
            {
              putMsg(narrator.makeTr("CHOOSE_ONLY_ONE_AT_A_TIME", { { "verb", tr("VERB_ATTACK_2") } }));
            }
            else
            {
              // Item(s) specified, so proceed with items.
              p_action.reset(new Actions::ActionAttack(player));
              p_action->setObject(entities.front());
              director.queueEntityAction(player, std::move(p_action));
              m_inventoryAreaShowsPlayer = false;
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
            director.queueEntityAction(player, std::move(p_action));
            return false;
          }
          else
          {
            // CTRL-ALT-arrow -- Move without turning
            p_action.reset(new Actions::ActionMove(player));
            p_action->setTarget(key_direction);
            director.queueEntityAction(player, std::move(p_action));
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
  } // end switch (m_currentInputState)

  return true;
}

bool AppStateGameMode::handle_mouse_wheel(UIEvents::EventMouseWheelMoved const& wheel)
{
  add_zoom(wheel.delta * 0.05f);
  return false;
}

void AppStateGameMode::add_zoom(float zoom_amount)
{
  float current_zoom_level = m_mapZoomLevel;

  current_zoom_level += zoom_amount;

  if (current_zoom_level < 0.1f)
  {
    current_zoom_level = 0.1f;
  }

  if (current_zoom_level > 3.0f)
  {
    current_zoom_level = 3.0f;
  }

  m_mapZoomLevel = current_zoom_level;
}

bool AppStateGameMode::onEvent(Event const& event)
{
  auto id = event.getId();

  if (id == App::EventAppWindowResized::id)
  {
    auto info = static_cast<App::EventAppWindowResized const&>(event);
    the_desktop.setSize({ info.newSize.x, info.newSize.y });
    the_desktop.getChild("MessageLogView").setRelativeDimensions(calcMessageLogDims());
    the_desktop.getChild("InventoryArea").setRelativeDimensions(calcInventoryDims());
    the_desktop.getChild("StatusArea").setRelativeDimensions(calcStatusAreaDims());
    return false;
  }
  else if (id == UIEvents::EventKeyPressed::id)
  {
    auto info = static_cast<UIEvents::EventKeyPressed const&>(event);
    bool keep_broadcasting = handle_key_press(info);
    return !keep_broadcasting;
  }
  else if (id == UIEvents::EventMouseWheelMoved::id)
  {
    auto info = static_cast<UIEvents::EventMouseWheelMoved const&>(event);
    bool keep_broadcasting = handle_mouse_wheel(info);
    return !keep_broadcasting;
  }

  /// @todo WRITE ME
  return false;
}

sf::IntRect AppStateGameMode::calcMessageLogDims()
{
  sf::IntRect messageLogDims;
  auto& config = S<IConfigSettings>();

  int inventory_area_width = config.get("inventory-area-width");
  int messagelog_area_height = config.get("messagelog-area-height");
  messageLogDims.width = m_appWindow.getSize().x - (inventory_area_width + 24);
  messageLogDims.height = messagelog_area_height - 10;
  //messageLogDims.height = static_cast<int>(m_appWindow.getSize().y * 0.25f) - 10;
  messageLogDims.left = 12;
  messageLogDims.top = 5;
  return messageLogDims;
}

void AppStateGameMode::resetInventorySelection()
{
  auto& game = gameState();
  auto& components = game.components();
  EntityId player = components.globals.player();

  if (m_inventoryAreaShowsPlayer == true)
  {
    m_inventorySelection->setViewed(player);
  }
  else
  {
    if (m_currentInputState == GameInputState::CursorLook)
    {
      if (components.position.existsFor(player))
      {
        MapID gameMap = components.position[player].map();
        EntityId floorId = MAPS.get(gameMap).getTile(m_cursorCoords).getSpaceEntity();
        m_inventorySelection->setViewed(floorId);
      }
    }
    else
    {
      m_inventorySelection->setViewed(components.position[player].parent());
    }
  }
}

sf::IntRect AppStateGameMode::calcStatusAreaDims()
{
  sf::IntRect statusAreaDims;
  sf::IntRect invAreaDims = the_desktop.getChild("InventoryArea").getRelativeDimensions();
  auto& config = S<IConfigSettings>();

  statusAreaDims.width = m_appWindow.getSize().x -
    (invAreaDims.width + 24);
  statusAreaDims.height = config.get("status-area-height");
  statusAreaDims.top = m_appWindow.getSize().y - (config.get("status-area-height") + 5);
  statusAreaDims.left = 12;
  return statusAreaDims;
}

sf::IntRect AppStateGameMode::calcInventoryDims()
{
  sf::IntRect messageLogDims = the_desktop.getChild("MessageLogView").getRelativeDimensions();
  sf::IntRect inventoryAreaDims;
  auto& config = S<IConfigSettings>();

  inventoryAreaDims.width = config.get("inventory-area-width");
  inventoryAreaDims.height = m_appWindow.getSize().y - 10;
  inventoryAreaDims.left = m_appWindow.getSize().x - (inventoryAreaDims.width + 3);
  inventoryAreaDims.top = 5;

  return inventoryAreaDims;
}

bool AppStateGameMode::moveCursor(Direction direction)
{
  auto& game = gameState();
  auto& components = game.components();
  EntityId player = components.globals.player();
  
  bool result = false;

  if (components.position.existsFor(player))
  {
    MapID map = components.position[player].map();
    result = MAPS.get(map).calcCoords(m_cursorCoords, direction, m_cursorCoords);
  }

  return result;
}

bool AppStateGameMode::handleKeyPressTargetSelection(EntityId player, UIEvents::EventKeyPressed const& key)
{
  auto& director = systems().director();
  int key_number = get_letter_key(key);
  Direction key_direction = get_direction_key(key);

  if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Tab)
  {
    m_inventoryAreaShowsPlayer = !m_inventoryAreaShowsPlayer;
    resetInventorySelection();
    return false;
  }

  if (!key.alt && !key.control && key.code == sf::Keyboard::Key::Escape)
  {
    putMsg(tr("ABORTED"));
    m_inventoryAreaShowsPlayer = false;
    resetInventorySelection();
    m_currentInputState = GameInputState::Map;
    return false;
  }

  if (m_actionInProgress && m_actionInProgress->hasTrait(Actions::Trait::CanBeSubjectVerbObjectPrepositionTarget))
  {
    if (!key.alt && !key.control && key_number != -1)
    {
      m_actionInProgress->setTarget(m_inventorySelection->getEntity(static_cast<InventorySlot>(key_number)));
      director.queueEntityAction(player, std::move(m_actionInProgress));
      m_inventoryAreaShowsPlayer = false;
      resetInventorySelection();
      m_currentInputState = GameInputState::Map;
      return false;
    }
  } // end if (action_in_progress.target_can_be_thing)

  if (m_actionInProgress && (m_actionInProgress->hasTrait(Actions::Trait::CanBeSubjectVerbDirection) ||
                               m_actionInProgress->hasTrait(Actions::Trait::CanBeSubjectVerbObjectPrepositionDirection)))
  {
    if (!key.alt && !key.control && key_direction != Direction::None)
    {
      m_actionInProgress->setTarget(key_direction);
      director.queueEntityAction(player, std::move(m_actionInProgress));
      m_inventoryAreaShowsPlayer = false;
      resetInventorySelection();
      m_currentInputState = GameInputState::Map;
      return false;
    }
  }

  return true;
}

bool AppStateGameMode::handleKeyPressCursorLook(EntityId player, UIEvents::EventKeyPressed const& key)
{
  // *** NON-MODIFIED KEYS ***********************************************
  if (!key.alt && !key.control && !key.shift)
  {
    switch (key.code)
    {
      case sf::Keyboard::Key::Up:
        moveCursor(Direction::North);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::PageUp:
        moveCursor(Direction::Northeast);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Right:
        moveCursor(Direction::East);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::PageDown:
        moveCursor(Direction::Southeast);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Down:
        moveCursor(Direction::South);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::End:
        moveCursor(Direction::Southwest);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Left:
        moveCursor(Direction::West);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      case sf::Keyboard::Key::Home:
        moveCursor(Direction::Northwest);
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

        // "/" - go back to Map focus.
      case sf::Keyboard::Key::Slash:
        m_currentInputState = GameInputState::Map;
        m_inventoryAreaShowsPlayer = false;
        resetInventorySelection();
        return false;

      default:
        break;
    } // end switch (key.code)
  } // end if (no modifier keys)

  return true;
}