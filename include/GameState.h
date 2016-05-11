#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stdafx.h"

#include "ThingId.h"

// Forward declarations
class MapFactory;
class ThingManager;

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

  MapFactory& get_map_factory();
  ThingManager& get_thing_manager();

  /// Set the game player.
  /// If the caller attempts to set a ThingId of a Thing that does not exist,
  /// the program will abort with a FATAL_ERROR call.
  /// @note Changing the player ID has not been testing as of this writing,
  ///       and unpredictable results may occur!
  /// @param ref ThingId of the Thing to set as the player.
  /// @return True if the set was successful, false otherwise.
  bool set_player(ThingId ref);

  /// Get the ThingId of the game player Thing.
  /// @return The player ThingId.
  ThingId get_player() const;

  /// Process a single tick in the game, if one needs to be processed.
  /// @return True if a tick elapsed, false if it did not.
  bool process_tick();

  static GameState& instance();

protected:

private:
  /// Pointer to the Map Factory object.
  std::unique_ptr<MapFactory> m_map_factory;

  /// Pointer to the Thing Manager object.
  std::unique_ptr<ThingManager> m_thing_manager;

  /// Reference to the Thing that is serving as the player.
  ThingId m_player;

  /// Current game clock, in milliseconds.
  uint64_t m_game_clock;

  /// Static pointer to the singleton instance of the GameState.
  static GameState* p_instance;
};

#define GAME  GameState::instance()

#endif // GAMESTATE_H
