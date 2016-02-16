#ifndef THINGMANAGER_H
#define THINGMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <boost/pool/object_pool.hpp>

#include "MetadataCollection.h"
#include "ThingRef.h"

// Forward declarations
class GameState;
class MapTile;
class Thing;
class ThingMetadata;

/// ThingManager is a factory to create and manage all the Things in the game.
class ThingManager
{
  friend class ThingId;
  friend class ThingRef;

public:
  /// Constructor.
  explicit ThingManager(GameState& game_state);

  virtual ~ThingManager();

  /// Create a particular object given the type name.
  /// @param type The type name of the object to create.
  /// @return A ThingRef to the new object created.
  ThingRef create(std::string type);

  /// Create a floor object.
  /// @param map_tile Pointer to the map tile associated.
  /// @return A ThingRef to the new object created.
  ThingRef create_tile_contents(MapTile* map_tile);

  /// Clone a particular object.
  /// @param original_ref Reference to the object to clone.
  /// @return A ThingRef to the new cloned object.
  ThingRef clone(ThingRef original_ref);

  /// Destroy an object given a ThingRef to the object.
  /// If the given ThingRef does not correspond to an object, does nothing.
  /// @param ref ThingRef of the object to destroy.
  void destroy(ThingRef ref);

  /// Returns whether a Thing with a particular ThingRef exists.
  bool exists(ThingRef ref);

  /// Get the ThingRef of Mu (nothingness).
  /// @return The ThingRef of Mu.
  static ThingRef get_mu();

protected:

  /// Get a pointer to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing* get_ptr(ThingId data);

  /// Get a pointer to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing const* get_ptr(ThingId data) const;

private:
  /// Reference to owning game state.
  GameState& m_game_state;

  /// Map of ThingIds to Things.
  /// @todo Probably faster to use an unordered_map and use ThingId.id
  ///       as the hash function.
  std::unordered_map<ThingId, Thing*> m_thing_map;

  /// Object pool of Things that exist.
  boost::object_pool<Thing> m_thing_pool;
};

#endif // THINGMANAGER_H
