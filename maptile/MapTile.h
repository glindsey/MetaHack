#ifndef MAPTILE_H
#define MAPTILE_H



#include "components/ComponentInventory.h"
#include "entity/EntityId.h"
#include "types/Direction.h"
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

  /// Get the tile's space object.
  EntityId getTileSpace() const;

  /// Get the tile's floor object.
  EntityId getTileFloor() const;

  /// Return this tile's description.
  virtual std::string getDisplayName() const override final;

  /// Sets the space entity for this tile (e.g. "TileSpace", "TileWall", etc.)
  /// Optionally also sets the material to use.
  /// @todo If the old entity contains any objects, move them into the new entity.
  /// @param type The entity type for this tile's space.
  /// @param material If present, the material to use for the space.
  void setTileSpace(std::string type, std::string material = "");

  /// Sets the floor entity for this tile (e.g. "TileFloor", "TilePit", etc.)
  /// @todo If the old entity contains any objects, move them into the new entity.
  /// @param type The entity type for this tile's floor.
  /// @param material If present, the material to use for the floor.
  void setTileFloor(std::string type, std::string material = "");

  /// Sets the tile type, without doing gameplay checks.
  /// Used to set up the map before gameplay begins.
  /// @param type Type of the tile.
  /// @return None.
  void setTileType(std::string type);

  /// Gets the current tile type.
  /// @return Type of the tile.
  std::string getTileType() const;

  /// Returns whether a tile is empty space, e.g. no wall in the way.
  bool isPassable() const;

  /// Returns whether a tile can be traversed by a certain Entity.
  bool canBeTraversedBy(EntityId entity) const;

  /// Set the current tile's location.
  void setCoords(IntVec2 coords);

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

  /// Get a const reference to this tile's metadata.
  json const& getCategoryData() const;

protected:
  /// Constructor, callable only by Map class.
  MapTile(IntVec2 coords, 
          std::string category,
          MapID mapID,
          EntityFactory& entities,
          Components::ComponentManager& components);

private:
  static bool initialized;

  /// Reference to the EntityFactory.
  EntityFactory& m_entities;

  /// Reference to the ComponentManager.
  Components::ComponentManager& m_components;

  /// The ID of the Map this MapTile belongs to.
  MapID m_mapID;

  /// This MapTile's coordinates on the map.
  IntVec2 m_coords;

  /// Entity representing the tile's floor.
  EntityId m_tileFloor;

  /// Entity representing the tile's contents.
  EntityId m_tileSpace;

  /// Type of the MapTile contents, as a string.
  std::string m_category;
};

#endif // MAPTILE_H
