#pragma once

#include "entity/EntityId.h"

// Forward declarations
class GameState;
class Entity;
class MapTile;
namespace Systems
{
  class Manager;
}

/// EntityFactory is a factory to create and manage all the Entities in the game.
class EntityFactory
{
  friend class EntityId;
  friend class EntityView;

public:
  /// Constructor.
  EntityFactory(GameState& state);
  ~EntityFactory();

  /// Create a particular object given the type name.
  /// @param category The category name of the object to create.
  /// @param material If present, the material to use for the object.
  /// @return EntityId of the new object created.
  EntityId create(std::string category, std::string material = "");

  /// Create an entity bound to a tile (e.g. the floor, or the space above the floor).
  /// @param mapTile Pointer to the map tile associated.
  /// @param category Entity to use for the space (e.g. "TileSpace", "TileWall", "TileFloor", "TilePit", etc.)
  /// @param material If present, the material to use for the space.
  /// @return EntityId of the new object created.
  EntityId createTileEntity(MapTile* mapTile, std::string category, std::string material = "");

  /// Clone a particular object.
  /// @param original ID of the object to clone.
  /// @return EntityId of the new cloned object.
  EntityId clone(EntityId original);

  /// Apply a material/template to an object.
  /// Replaces components in the object with the ones from the subtype data
  /// provided. If any component of the subtype data is null, deletes that
  /// component from the object.
  /// Does not touch components that are not present in the subtype data.
  /// @todo Handle inventory properly -- right now, if it is deleted,
  ///       it will put any contained entities into a bad state.
  /// @todo It might be prudent to back up any data that has been replaced,
  ///       so a material/template can be "unapplied" -- for example, if
  ///       something were transmuted from lead to gold, and back to lead.
  ///       I don't know how this could be done, though, without basically
  ///       maintaining a journal of changes to the object, and that seems
  ///       like some *massive* overkill.
  void applyCategoryData(EntityId id, std::string subType, std::string name);

  /// Morph an object into a new category.
  /// Deletes the existing object components, and creates new ones based on
  /// the category provided.
  /// @todo Handle inventory properly -- right now it is summarily deleted,
  ///       which will put any contained entities into a bad state.
  void morph(EntityId id, std::string category, std::string material = "");

  /// Destroy an object given a EntityId.
  /// If the given EntityId does not correspond to an object, does nothing.
  /// @param id EntityId of the object to destroy.
  void destroy(EntityId id);

protected:

private:
  /// Reference to the game state.
  GameState& m_gameState;

  /// Boolean indicating whether EntityPool is initialized.
  bool m_initialized = false;

  /// Counter indicating the next EntityId to be created.
  uint64_t m_nextEntityId = 0;
};