#ifndef THINGMANAGER_H
#define THINGMANAGER_H

#include "stdafx.h"

#include "MetadataCollection.h"
#include "ThingRef.h"

// Forward declarations
class GameState;
class MapTile;
class Thing;
class ThingMetadata;

/// ThingManager is a factory to create and manage all the Things in the game.
class ThingManager final
{
  friend class ThingId;
  friend class ThingRef;

public:
  /// Constructor.
  ThingManager();
  ~ThingManager();

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive) const
  {
    /// @todo GSL -- KEEP GOING HERE
    //archive(m_thing_map);
  }

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

  /// Get a reference to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing& get(ThingId data);

  /// Get a reference to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing const& get(ThingId data) const;

private:
  /// Map of ThingIds to Things.
  /// @todo Probably faster to use an unordered_map and use ThingId.id
  ///       as the hash function.
  std::unordered_map<ThingId, std::unique_ptr<Thing>> m_thing_map;
};

// === CONVENIENCE MACRO ======================================================
#define MU  ThingManager::get_mu()

#endif // THINGMANAGER_H