#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "stdafx.h"

#include "Event.h"
#include "Object.h"
#include "entity/EntityId.h"

// Forward declarations
class ComponentManager;
class EntityPool;
class Lua;
class MapFactory;

/// Class that encapsulates the entire state of the game data.
/// This class can be instantiated only once at a time. Attempting to create
/// a second instance when one is already present will cause an exception to
/// be thrown.
class GameState : public Object
{
public:

  struct EventClockChanged : public ConcreteEvent<EventClockChanged>
  {
    EventClockChanged(ElapsedTicks ticks_) :
      ticks{ ticks_ }
    {}

    ElapsedTicks const ticks;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | current time: " << ticks;
    }
  };

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
  MapFactory const& maps() const;
  EntityPool& entities();
  EntityPool const& entities() const;
  ComponentManager& components();
  ComponentManager const& components() const;

  /// Process a single tick in the game, if one needs to be processed.
  /// @return True if a tick elapsed, false if it did not.
  bool processGameClockTick();

  /// Get the Lua state object.
  Lua& lua();
  Lua const& lua() const;

  static GameState& instance();

protected:

private:
  /// Pointer to the Lua state object.
  std::unique_ptr<Lua> m_lua;

  /// Pointer to the Map Factory object.
  std::unique_ptr<MapFactory> m_mapFactory;

  /// Pointer to the Entity Manager object.
  std::unique_ptr<EntityPool> m_entityPool;

  /// Pointer to the Components Manager object.
  std::unique_ptr<ComponentManager> m_components;

  /// Static pointer to the singleton instance of the GameState.
  static GameState* s_instance;
};

#define GAME              GameState::instance()
#define MAPS              (GAME.maps())
#define ENTITIES          (GAME.entities())
#define COMPONENTS        (GAME.components())

#endif // GAMESTATE_H
