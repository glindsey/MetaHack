#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stdafx.h"

#include "Object.h"

// Forward declarations
class ComponentManager;
class EntityId;
class EntityPool;
class MapFactory;

/// Class that encapsulates the entire state of the game data.
/// This class can be instantiated only once at a time. Attempting to create
/// a second instance when one is already present will cause an exception to
/// be thrown.
class GameState : public Object
{
public:
  /// Constructor that constructs the game state from the provided JSON.
  /// Pass an empty object to construct the state from scratch.
  GameState(json const& j);

  virtual ~GameState();

  GameState(GameState const&) = delete;
  GameState(GameState&&) = delete;
  GameState operator==(GameState const&) = delete;
  GameState operator==(GameState&&) = delete;

  void initialize(json const& j);

  friend void from_json(json const& j, GameState& obj);
  friend void to_json(json& j, GameState const& obj);

  MapFactory& maps();
  EntityPool& entities();
  ComponentManager& components();

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

  static GameState& instance();

protected:

private:
  /// Pointer to the Map Factory object.
  std::unique_ptr<MapFactory> m_mapFactory;

  /// Pointer to the Entity Manager object.
  std::unique_ptr<EntityPool> m_entityPool;

  /// Pointer to the Components Manager object.
  std::unique_ptr<ComponentManager> m_componentsManager;

  /// Global game data, as stored in a JSON object.
  json m_global;

  /// Static pointer to the singleton instance of the GameState.
  static GameState* s_instance;
};

#define GAME  GameState::instance()
#define MAPS  GAME.maps()
#define ENTITIES GAME.entities()
#define COMPONENTS GAME.components()

#endif // GAMESTATE_H
