#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stdafx.h"

#include "EntityId.h"

// Forward declarations
class MapFactory;
class MetadataCollection;
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
  void save_state(FileName filename);

  MapFactory& get_maps();
  EntityPool& get_things();
  MetadataCollection& get_metadata_collection(std::string category);

  ElapsedTime const& get_game_clock() const;
  void set_game_clock(ElapsedTime game_clock);
  void increment_game_clock(ElapsedTime added_time);

  /// Set the game player.
  /// If the caller attempts to set a EntityId of a Entity that does not exist,
  /// the program will abort with a FATAL_ERROR call.
  /// @note Changing the player ID has not been testing as of this writing,
  ///       and unpredictable results may occur!
  /// @param ref EntityId of the Entity to set as the player.
  /// @return True if the set was successful, false otherwise.
  bool set_player(EntityId ref);

  /// Get the EntityId of the game player Entity.
  /// @return The player EntityId.
  EntityId get_player() const;

  /// Process a single tick in the game, if one needs to be processed.
  /// @return True if a tick elapsed, false if it did not.
  bool process_tick();

  static GameState& instance();

protected:

private:
  /// Pointer to the Map Factory object.
  std::unique_ptr<MapFactory> m_map_factory;

  /// Pointer to the Entity Manager object.
  std::unique_ptr<EntityPool> m_thing_manager;

  /// A collection of collections -- a metacollection!
  boost::ptr_unordered_map<std::string, MetadataCollection> m_metacollection;

  /// Reference to the Entity that is serving as the player.
  EntityId m_player;

  /// Current game clock, in ticks. Ticks represent tens of milliseconds.
  ElapsedTime m_game_clock;

  /// Static pointer to the singleton instance of the GameState.
  static GameState* p_instance;
};

#define GAME  GameState::instance()

#endif // GAMESTATE_H
