#pragma once

#include "stdafx.h"

#include "MetadataCollection.h"
#include "EntityId.h"

// Forward declarations
class GameState;
class MapTile;
class Entity;

/// EntityPool is a factory to create and manage all the Entities in the game.
class EntityPool
{
  friend class EntityId;
  friend class EntityView;

public:
  /// Constructor.
  EntityPool(GameState& game);
  ~EntityPool();

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
  /// @return EntityId of the new object created.
  EntityId create(std::string type);

  /// Create a floor object.
  /// @param map_tile Pointer to the map tile associated.
  /// @return EntityId of the new object created.
  EntityId create_tile_contents(MapTile* map_tile);

  /// Clone a particular object.
  /// @param original ID of the object to clone.
  /// @return EntityId of the new cloned object.
  EntityId clone(EntityId original);

  /// Destroy an object given a EntityId.
  /// If the given EntityId does not correspond to an object, does nothing.
  /// @param id EntityId of the object to destroy.
  void destroy(EntityId id);

  /// Returns whether a Entity with a particular EntityId exists.
  /// @param id EntityId of the object to find.
  bool exists(EntityId id);

  /// Get the EntityId of Mu (nothingness).
  /// @return The EntityId of Mu.
  static EntityId get_mu();

  /// Get a reference to the Entity associated with a particular EntityId.
  /// If the ID does not exist, returns Mu.
  /// @note Should only be called by views.
  Entity& get(EntityId data);

  /// Get a reference to the Entity associated with a particular EntityId.
  /// If the ID does not exist, returns Mu.
  /// @note Should only be called by views.
  Entity const& get(EntityId data) const;

protected:

private:
  /// Reference to the game state.
  GameState& m_game;

  /// Boolean indicating whether EntityPool is initialized.
  bool m_initialized = false;

  /// Counter indicating the next EntityId to be created.
  uint64_t m_nextEntityId = 0;

  /// Map of EntityIds to Entities.
  /// @todo Probably faster to use an unordered_map and use EntityId.id
  ///       as the hash function.
  std::unordered_map<EntityId, std::unique_ptr<Entity>> m_thing_map;
};