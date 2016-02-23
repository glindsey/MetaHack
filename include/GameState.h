#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <boost/noncopyable.hpp>
#include <memory>

#include "ThingRef.h"

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
  GameState(std::string filename);

  virtual ~GameState();

  /// Save the game state to disk.
  /// Throws an exception if the game could not be saved.
  /// @todo WRITE ME
  void save_state(std::string filename);

  MapFactory& get_map_factory();
  ThingManager& get_thing_manager();

  /// Set the game player.
  /// If the caller attempts to set a ThingRef of a Thing that does not exist,
  /// the program will abort with a FATAL_ERROR call.
  /// @note Changing the player ID has not been testing as of this writing,
  ///       and unpredictable results may occur!
  /// @param ref ThingRef of the Thing to set as the player.
  /// @return True if the set was successful, false otherwise.
  bool set_player(ThingRef ref);

  /// Get the ThingRef of the game player Thing.
  /// @return The player ThingRef.
  ThingRef get_player() const;

  static GameState& instance();

protected:

private:
  std::unique_ptr<MapFactory> m_map_factory;
  std::unique_ptr<ThingManager> m_thing_manager;
  ThingRef m_player;

  static GameState* p_instance;
};

#define GAME  GameState::instance()

#endif // GAMESTATE_H
