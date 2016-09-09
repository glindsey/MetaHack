#ifndef THINGMANAGER_H
#define THINGMANAGER_H

#include "stdafx.h"

#include "MetadataCollection.h"
#include "ThingId.h"

// Forward declarations
class GameState;
class MapTile;
class Thing;
class ThingMetadata;

/// ThingManager is a factory to create and manage all the Things in the game.
class ThingManager
{
  friend class ThingId;
  friend class ThingView;

public:
  /// Constructor.
  ThingManager(GameState& game);
  ~ThingManager();

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive) const
  {
    /// @todo GSL -- KEEP GOING HERE
    //archive(m_thing_map);
  }

  bool first_is_subtype_of_second(std::string first, std::string second);

  /// Create a particular object given the type name.
  /// @param type The type name of the object to create.
  /// @return ThingId of the new object created.
  ThingId create(std::string type);

  /// Create a floor object.
  /// @param map_tile Pointer to the map tile associated.
  /// @return ThingId of the new object created.
  ThingId create_tile_contents(MapTile* map_tile);

  /// Clone a particular object.
  /// @param original ID of the object to clone.
  /// @return ThingId of the new cloned object.
  ThingId clone(ThingId original);

  /// Destroy an object given a ThingId.
  /// If the given ThingId does not correspond to an object, does nothing.
  /// @param id ThingId of the object to destroy.
  void destroy(ThingId id);

  /// Returns whether a Thing with a particular ThingId exists.
  /// @param id ThingId of the object to find.
  bool exists(ThingId id);

  /// Get the ThingId of Mu (nothingness).
  /// @return The ThingId of Mu.
  static ThingId get_mu();

protected:

  /// Get a reference to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing& get(ThingId data);

  /// Get a reference to the Thing associated with a particular ThingId.
  /// If the ID does not exist, returns Mu.
  Thing const& get(ThingId data) const;

private:
  /// Reference to the game state.
  GameState& m_game;

  /// Boolean indicating whether ThingManager is initialized.
  bool m_initialized = false;

  /// Counter indicating the next ThingId to be created.
  uint64_t m_nextThingId = 0;

  /// Map of ThingIds to Things.
  /// @todo Probably faster to use an unordered_map and use ThingId.id
  ///       as the hash function.
  std::unordered_map<ThingId, std::unique_ptr<Thing>> m_thing_map;
};

#endif // THINGMANAGER_H