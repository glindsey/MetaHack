#include "stdafx.h"

#include "game/GameState.h"

#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "Service.h"
#include "services/IGraphicViews.h"


// Namespace aliases
namespace fs = boost::filesystem;

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  Assert("GameState", p_instance == nullptr, "tried to create more than one GameState instance at a time");

  p_instance = this;

  m_entity_pool.reset(NEW EntityPool(*this));
  m_map_factory.reset(NEW MapFactory(*this));

  m_data["types"] = json::object({});
  m_data["entities"] = json::object({});
  m_data["global"] = json::object({});
}

GameState::GameState(FileName filename)
{
#if 0
  Assert("GameState", p_instance == nullptr, "tried to create more than one GameState instance at a time");

  p_instance = this;

  std::ifstream fs{ filename.c_str() };
  cereal::XMLInputArchive iarchive{ fs };

  iarchive(m_map_factory, m_entity_pool, m_player);
#endif
}

GameState::~GameState()
{
  p_instance = nullptr;
}

void GameState::saveState(FileName filename)
{
#if 0
  std::ofstream fs{ filename.c_str() };
  cereal::XMLOutputArchive oarchive{ fs };

  oarchive(m_map_factory, m_entity_pool, m_player);
#endif
}

MapFactory& GameState::getMaps()
{
  Assert("GameState", m_map_factory, "MapFactory does not exist");

  return *m_map_factory;
}

EntityPool& GameState::getEntities()
{
  Assert("GameState", m_entity_pool, "EntityPool does not exist");

  return *m_entity_pool;
}

ElapsedTime GameState::getGameClock() const
{
  return m_data["global"].value("clock", 0ULL);
}

void GameState::setGameClock(ElapsedTime game_clock)
{
  m_data["global"]["clock"] = game_clock;
}

void GameState::incrementGameClock(ElapsedTime added_time)
{
  /// @todo Check for the unlikely, but not impossible, chance of rollover.
  setGameClock(getGameClock() + added_time);
}

bool GameState::setPlayer(EntityId ref)
{
  Assert("GameState", ref != getEntities().get_mu(), "tried to make nothingness the player");

  m_data["global"]["player"] = ref;
  return true;
}

EntityId GameState::getPlayer() const
{
  return m_data["global"].value("player", EntityId());
}

bool GameState::processGameClockTick()
{
  EntityId player = getPlayer();

  auto player_action_pending = player->anyActionIsPending();
  auto player_action_in_progress = player->actionIsInProgress();
  if (player_action_pending || player_action_in_progress)
  {
    // QUESTION: Do we want to update all Entities, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapId current_map_id = player->getMapId();
    Map& current_map = GAME.getMaps().get(current_map_id);

    // Process everything on the map, and increment game clock.
    current_map.processEntities();
    incrementGameClock(ElapsedTime(1));

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      current_map.updateLighting();
    }
    return true;
  }
  return false;
}

json & GameState::category(std::string name)
{
  if (m_data["categories"].count(name) == 0)
  {
    loadCategory(name);
  }

  return m_data["categories"][name];
}

void GameState::addTo(json & first, json & second)
{
  json flat_first = first.flatten();

  json flat_second = second.flatten();

  for (auto iter = flat_second.cbegin(); iter != flat_second.cend(); ++iter)
  {
    if (flat_first.count(iter.key()) == 0)
    {
      flat_first[iter.key()] = iter.value();
    }
  }

  first = flat_first.unflatten();
}

void GameState::loadCategory(std::string name)
{
  json& category_data = m_data["categories"][name];

  // Look for the various files containing this metadata.
  FileName resource_string = "resources/entity/" + name;
  FileName jsonfile_string = resource_string + ".json";
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);
  fs::path jsonfile_path = fs::path(jsonfile_string);

  /// Try to load this Entity's JSON metadata.
  if (fs::exists(jsonfile_path))
  {
    CLOG(INFO, "GameState") << "Loading JSON data for " << name << " category";

    try
    {
      std::ifstream ifs(jsonfile_string);
      category_data << ifs;
    }
    catch (std::exception& e)
    {
      CLOG(FATAL, "GameState") << "Error reading " <<
        jsonfile_string << ": " << e.what();
    }
  }
  else
  {
    CLOG(WARNING, "GameState") << "Can't find " << jsonfile_string;
  }

  /// Check the parent key.
  if (category_data.count("parent") != 0)
  {
    std::string parent_name = category_data["parent"];
    if (parent_name == name)
    {
      CLOG(FATAL, "GameState") << name << " is defined as its own parent. What do you think this is, a Heinlein story?";
    }
    json& parent_data = category(parent_name);

    addTo(category_data, parent_data);
  }

  // DEBUG: Check for "Human".
  if (name == "Human")
  {
    CLOG(TRACE, "GameState") << "================================";
    CLOG(TRACE, "GameState") << "DEBUG: Human JSON contents are:";
    CLOG(TRACE, "GameState") << category_data.dump(2);
    CLOG(TRACE, "GameState") << "================================";
  }

  /// Try to load and run this Entity's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "GameState") << "Loading Lua script for " << name;
    the_lua_instance.require(resource_string, true);
  }
  else
  {
    CLOG(WARNING, "GameState") << "Can't find " << luafile_string;
  }

  Service<IGraphicViews>::get().loadViewResourcesFor(name, category_data);
}

GameState& GameState::instance()
{
  Assert("GameState", p_instance != nullptr, "tried to get non-existent GameState instance");

  return *(p_instance);
}