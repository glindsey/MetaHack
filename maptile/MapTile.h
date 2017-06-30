#ifndef MAPTILE_H
#define MAPTILE_H

#include "components/ComponentInventory.h"
#include "entity/EntityId.h"
#include "types/Direction.h"
#include "types/EntitySpecs.h"
#include "types/GameObject.h"
#include "types/LightInfluence.h"
#include "map/MapFactory.h"
#include "Object.h"

// Forward declarations
namespace Components
{
  class ComponentManager;
}
class EntityFactory;

/// Class representing one tile of the map.
/// @todo Add notifyObservers calls where needed
class MapTile
  :
  public GameObject,
  public Object
{
  friend class Map;

public:
  virtual ~MapTile();

  /// Get the tile's space entity.
  EntityId getSpaceEntity() const;

  /// Get the tile's floor entity.
  EntityId getFloorEntity() const;

  /// Gets whichever entity should be used for MapTile display.
  /// This will generally be the floor if the tile is opaque, or
  /// the space if the tile is not.
  /// @todo Refactor the MapTile view to display both if the space is not
  ///       totally opaque; once that is done this method should no longer
  ///       be necessary.
  EntityId getDisplayEntity() const;

  /// Return this tile's description.
  virtual std::string getDisplayName() const override final;

  /// Sets the space entity for this tile (e.g. "TileSpace", "TileWall", etc.)
  /// Optionally also sets the material to use.
  /// @todo If the old entity contains any objects, move them into the new entity.
  /// @param specs The entity category and optional material to use for the space.
  void setTileSpace(EntitySpecs specs);

  /// Sets the floor entity for this tile (e.g. "TileFloor", "TilePit", etc.)
  /// @todo If the old entity contains any objects, move them into the new entity.
  /// @param specs The entity category and optional material to use for the floor.
  /// @param category The entity type for this tile's floor.
  /// @param material If present, the material to use for the floor.
  void setTileFloor(EntitySpecs specs);

  /// Sets the tile type, without doing gameplay checks.
  /// Used to set up the map before gameplay begins.
  /// @param floorCategory Category of the tile's floor.
  /// @param spaceCategory Category of the tile's space.
  /// @return None.
  void setTileType(EntitySpecs floor, EntitySpecs space);

  /// Gets the current tile floor specs.
  /// @return Category/material of the tile's floor entity.
  EntitySpecs getTileFloorSpecs() const;

  /// Gets the current tile space specs.
  /// @return Category/material of the tile's space entity.
  EntitySpecs getTileSpaceSpecs() const;

  /// Returns whether a tile is empty space, e.g. no wall in the way.
  bool isPassable() const;

  /// Returns whether a tile can be traversed by a certain Entity.
  bool canBeTraversedBy(EntityId entity) const;

  /// Get the current tile's location.
  IntVec2 const& getCoords() const;

  /// Get a reference to the map this tile belongs to.
  MapID map() const;

  /// Get the opacity of this tile.
  Color getOpacity() const;

  /// Get whether the tile is opaque or not.
  bool isTotallyOpaque() const;

  /// Get the coordinates associated with a tile.
  static RealVec2 getPixelCoords(IntVec2 tile);

  /// Get a reference to an adjacent tile.
  MapTile const& getAdjacentTile(Direction direction) const;

protected:
  /// Constructor, callable only by Map class.
  MapTile(IntVec2 coords,
          MapID mapID,
          EntityFactory& entities,
          Components::ComponentManager& components,
          std::string floorCategory = "",
          std::string spaceCategory = "");

private:
  static bool initialized;

  /// This MapTile's coordinates on the map.
  IntVec2 m_coords;

  /// The ID of the Map this MapTile belongs to.
  MapID m_mapID;

  /// Reference to the EntityFactory.
  EntityFactory& m_entities;

  /// Reference to the ComponentManager.
  Components::ComponentManager& m_components;

  /// Entity representing the tile's floor.
  EntityId m_tileFloor;

  /// Entity representing the tile's contents.
  EntityId m_tileSpace;
};

#endif // MAPTILE_H
