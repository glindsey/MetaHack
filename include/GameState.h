#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <boost/noncopyable.hpp>
#include <memory>

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

  /// @todo Create a constructor that loads a game state from disk.

  virtual ~GameState();

  MapFactory& get_map_factory();
  ThingManager& get_thing_manager();

  static GameState& instance();

protected:

private:
  std::unique_ptr<MapFactory> m_map_factory;
  std::unique_ptr<ThingManager> m_thing_manager;

  static GameState* p_instance;
};

#define GAME  GameState::instance()

#endif // GAMESTATE_H
