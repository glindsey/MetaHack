#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stdafx.h"

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"

// Forward declarations
class MapFactory;
class EntityPool;

/// Class that encapsulates the entire state of the game data.
/// This class can be instantiated only once at a time. Attempting to create
/// a second instance when one is already present will cause an exception to
/// be thrown.
class GameState : public boost::noncopyable
{
public:
  GameState();

  /// Constructor that loads a game state from disk.
  /// Throws an exception if the game could not be loaded.
  /// @todo WRITE ME
  GameState(FileName filename);

  virtual ~GameState();

  /// Save the game state to disk.
  /// Throws an exception if the game could not be saved.
  /// @todo WRITE ME
  void saveState(FileName filename);

  MapFactory& getMaps();
  EntityPool& getEntities();

  ElapsedTime getGameClock() const;
  void setGameClock(ElapsedTime game_clock);
  void incrementGameClock(ElapsedTime added_time);

  /// Set the game player.
  /// If the caller attempts to set a EntityId of a Entity that does not exist,
  /// the program will abort with a FATAL log.
  /// @note Changing the player ID has not been testing as of this writing,
  ///       and unpredictable results may occur!
  /// @param ref EntityId of the Entity to set as the player.
  /// @return True if the set was successful, false otherwise.
  bool setPlayer(EntityId ref);

  /// Get the EntityId of the game player Entity.
  /// @return The player EntityId.
  EntityId getPlayer() const;

  /// Process a single tick in the game, if one needs to be processed.
  /// @return True if a tick elapsed, false if it did not.
  bool processGameClockTick();

  /// Get data for a specific Entity category.
  /// If it doesn't exist, attempt to load it.
  json& category(std::string name);

  /// Add data in second JSON object to first JSON object.
  /// @todo Belongs in a "JSON utilities" module or something, not here
  void addTo(json& first, json& second);

  /// Get reference to game state data.
  inline json& data()
  {
    return m_data;
  }

  /// Get const reference to game state data.
  inline json const& data() const
  {
    return m_data;
  }

  static GameState& instance();

protected:
  /// Attempt to load JSON data for an entity category.
  /// Also runs any associated Lua script.
  void GameState::loadCategory(std::string name);

private:
  /// Pointer to the Map Factory object.
  std::unique_ptr<MapFactory> m_map_factory;

  /// Pointer to the Entity Manager object.
  std::unique_ptr<EntityPool> m_entity_pool;

  /// Game state data, as stored in a JSON object.
  json m_data;

  /// Static pointer to the singleton instance of the GameState.
  static GameState* p_instance;
};

#define GAME  GameState::instance()

#endif // GAMESTATE_H