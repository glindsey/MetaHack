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
  /// @return EntityId of the new object created.
  EntityId create(std::string category);

  /// Create an entity bound to a tile (e.g. the floor, or the space above the floor).
  /// @param mapTile Pointer to the map tile associated.
  /// @param category Entity to use for the space (e.g. "TileSpace", "TileWall", "TileFloor", "TilePit", etc.)
  /// @param material If present, the material to use for the space.
  /// @todo Implement the use of the material parameter.
  /// @return EntityId of the new object created.
  EntityId createTileEntity(MapTile* mapTile, std::string category, std::string material = "");

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

protected:

private:
  /// Reference to the game state.
  GameState& m_gameState;

  /// Boolean indicating whether EntityPool is initialized.
  bool m_initialized = false;

  /// Counter indicating the next EntityId to be created.
  uint64_t m_nextEntityId = 0;
};