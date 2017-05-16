#pragma once

#include "stdafx.h"

#include "entity/EntityId.h"

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
  EntityPool(GameState& state);
  ~EntityPool();

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive) const
  {
    /// @todo GSL -- KEEP GOING HERE
    //archive(m_thing_map);
  }

  /// Create a particular object given the type name.
  /// @param category The category name of the object to create.
  /// @return EntityId of the new object created.
  EntityId create(std::string category);

  /// Create a floor object.
  /// @param map_tile Pointer to the map tile associated.
  /// @return EntityId of the new object created.
  EntityId createTileContents(MapTile* map_tile);

  /// Clone a particular object.
  /// @param original ID of the object to clone.
  /// @return EntityId of the new cloned object.
  EntityId clone(EntityId original);

  /// Apply a material/template to an object.
  /// Replaces components in the object with the ones from the data
  /// provided.
  /// @todo Handle inventory properly -- right now, if it is deleted,
  ///       it will put any contained entities into a bad state.
  void applyCategoryData(EntityId id, std::string subType, std::string name);

  /// Morph an object into a new category.
  /// Deletes the existing object components, and creates new ones based on
  /// the category provided.
  /// @todo Handle inventory properly -- right now it is summarily deleted,
  ///       which will put any contained entities into a bad state.
  void morph(EntityId id, std::string category);

  /// Destroy an object given a EntityId.
  /// If the given EntityId does not correspond to an object, does nothing.
  /// @param id EntityId of the object to destroy.
  void destroy(EntityId id);

  /// Returns whether a Entity with a particular EntityId exists.
  /// @param id EntityId of the object to find.
  bool exists(EntityId id);

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
  GameState& m_state;

  /// Boolean indicating whether EntityPool is initialized.
  bool m_initialized = false;

  /// Counter indicating the next EntityId to be created.
  uint64_t m_nextEntityId = 0;

  /// Map of EntityIds to Entities.
  /// @todo Probably faster to use an unordered_map and use EntityId.id
  ///       as the hash function.
  std::unordered_map<EntityId, std::unique_ptr<Entity>> m_thing_map;
};