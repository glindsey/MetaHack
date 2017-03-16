#ifndef MAPTILE_H
#define MAPTILE_H

#include "stdafx.h"

#include "Direction.h"
#include "GameObject.h"
#include "Inventory.h"
#include "LightInfluence.h"
#include "MapFactory.h"
#include "Subject.h"
#include "Entity.h"
#include "EntityId.h"
#include "TileSheet.h"

// Forward declarations
class DynamicEntity;
class Floor;
class Metadata;

/// Class representing one tile of the map.
/// @todo Add notifyObservers calls where needed
class MapTile
  :
  public GameObject,
  public Subject
{
  friend class Map;

public:
  virtual ~MapTile();

  /// Get the tile's contents object.
  EntityId get_tile_contents() const;

  /// Return this tile's description.
  virtual std::string get_display_name() const override final;

  /// Sets the tile type, without doing gameplay checks.
  /// Used to set up the map before gameplay begins.
  /// @param type Type of the tile.
  /// @return None.
  void set_tile_type(std::string type);

  /// Gets the current tile type.
  /// @return Type of the tile.
  std::string get_tile_type() const;

  /// Returns whether a tile is empty space, e.g. no wall in the way.
  bool is_empty_space() const;

  /// Returns whether a tile can be traversed by a certain DynamicEntity.
  bool can_be_traversed_by(EntityId entity) const;

  /// Set the current tile's location.
  void set_coords(int x, int y);

  void set_coords(IntegerVec2 coords);

  /// Get the current tile's location.
  IntegerVec2 const& get_coords() const;

  /// Get a reference to the map this tile belongs to.
  MapId get_map_id() const;

  /// Set the current tile's light level.
  void set_ambient_light_level(sf::Color level);

  /// Receive light from the specified LightSource.
  /// Gets the Map this tile belongs to and does a recursive
  /// raycasting algorithm on it.
  virtual void be_lit_by(EntityId light);

  /// Clear light influences.
  void clear_light_influences();

  /// Add a light influence to the tile.
  void add_light_influence(EntityId source,
                           LightInfluence influence);

  /// Get the light shining on a tile.
  /// Syntactic sugar for get_wall_light_level(Direction::Self).
  sf::Color get_light_level() const;

  /// Get the light shining on a tile wall.
  sf::Color get_wall_light_level(Direction direction) const;

  /// Get the opacity of this tile.
  sf::Color get_opacity() const;

  /// Get whether the tile is opaque or not.
  bool is_opaque() const;

    /// Get the coordinates associated with a tile.
  static RealVec2 get_pixel_coords(int x, int y);

  /// Get the coordinates associated with a tile.
  static RealVec2 get_pixel_coords(IntegerVec2 tile);

  /// Get a reference to an adjacent tile.
  MapTile const & get_adjacent_tile(Direction direction) const;

  /// Get a const reference to this tile's metadata.
  Metadata const & get_metadata() const;

protected:
  /// Constructor, callable only by Map class.
  MapTile(IntegerVec2 coords, Metadata& metadata, MapId map_id);

private:
  static bool initialized;

  /// The ID of the Map this MapTile belongs to.
  MapId m_map_id;

  /// This MapTile's coordinates on the map.
  IntegerVec2 m_coords;

  /// Pointer to this MapTile's metadata.
  /// This has to be a pointer rather than a reference because it can be
  /// modified after MapTile construction.
  Metadata* m_p_metadata;

  /// Reference to the Entity that represents this tile's contents.
  EntityId m_tile_contents;

  /// Tile's light level.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  sf::Color m_ambient_light_color;

  /// The calculated light levels of this tile and all of its walls.
  /// Mapping to an int is horribly hacky but I see no other alternative
  /// right now.
  std::map<unsigned int, sf::Color> m_calculated_light_colors;

  /// A map of LightInfluences, representing the amount of light that
  /// each entity is contributing to this map tile.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  std::map<EntityId, LightInfluence> m_lights;
};

#endif // MAPTILE_H
