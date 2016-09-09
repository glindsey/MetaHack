#ifndef MAPTILE_H
#define MAPTILE_H

#include "stdafx.h"

#include "Direction.h"
#include "GameObject.h"
#include "Inventory.h"
#include "LightInfluence.h"
#include "MapFactory.h"
#include "Thing.h"
#include "ThingId.h"
#include "TileSheet.h"

// Forward declarations
class Entity;
class Floor;
class Metadata;

class MapTile
  :
  public GameObject
{
  friend class Map;

public:
  virtual ~MapTile();

  /// Get the tile's contents object.
  ThingId get_tile_contents() const;

  /// Return this tile's description.
  virtual std::string get_display_name() const override final;

  /// Return the coordinates of the tile on the tilesheet.
  Vec2u get_tile_sheet_coords() const;

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

  /// Returns whether a tile can be traversed by a certain Entity.
  bool can_be_traversed_by(ThingId thing) const;

  /// Set the current tile's location.
  void set_coords(int x, int y);

  /// Get the current tile's location.
  Vec2i const& get_coords() const;

  /// Get a reference to the map this tile belongs to.
  MapId get_map_id() const;

  /// Set the current tile's light level.
  void set_ambient_light_level(sf::Color level);

  /// Receive light from the specified LightSource.
  /// Gets the Map this tile belongs to and does a recursive
  /// raycasting algorithm on it.
  virtual void be_lit_by(ThingId light);

  /// Clear light influences.
  void clear_light_influences();

  /// Add a light influence to the tile.
  void add_light_influence(ThingId source,
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

  void draw_highlight(sf::RenderTarget& target,
                      Vec2f location,
                      sf::Color fgColor,
                      sf::Color bgColor,
                      int frame);

  /// Add this MapTile's walls to a VertexArray to be drawn.
  /// @param vertices Array to add vertices to.
  /// @param use_lighting If true, calculate lighting when adding.
  ///                     If false, store directly w/white bg color.
  /// @param nw_is_empty  Whether tile to the northwest is empty.
  /// @param n_is_empty   Whether tile to the north is empty.
  /// @param ne_is_empty  Whether tile to the northeast is empty.
  /// @param e_is_empty   Whether tile to the east is empty.
  /// @param se_is_empty  Whether tile to the southeast is empty.
  /// @param s_is_empty   Whether tile to the south is empty.
  /// @param sw_is_empty  Whether tile to the southwest is empty.
  /// @param w_is_empty   Whether tile to the west is empty.
  void add_wall_vertices_to(sf::VertexArray& vertices,
                            bool use_lighting,
                            bool nw_is_empty, bool n_is_empty,
                            bool ne_is_empty, bool e_is_empty,
                            bool se_is_empty, bool s_is_empty,
                            bool sw_is_empty, bool w_is_empty);

  /// Get the coordinates associated with a tile.
  static Vec2f get_pixel_coords(int x, int y);

  /// Get the coordinates associated with a tile.
  static Vec2f get_pixel_coords(Vec2i tile);

protected:
  /// Constructor, callable only by Map class.
  MapTile(Vec2i coords, Metadata& metadata, MapId map_id);

  /// Get a reference to an adjacent tile.
  MapTile const & get_adjacent_tile(Direction direction) const;

private:
  static bool initialized;

  /// The ID of the Map this MapTile belongs to.
  MapId m_map_id;

  /// This MapTile's coordinates on the map.
  Vec2i m_coords;

  /// Pointer to this MapTile's metadata.
  /// This has to be a pointer rather than a reference because it can be
  /// modified after MapTile construction.
  Metadata* m_p_metadata;

  /// Reference to the Thing that represents this tile's contents.
  ThingId m_tile_contents;

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
  /// each thing is contributing to this map tile.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  std::map<ThingId, LightInfluence> m_lights;

  /// Random tile offset.
  int m_tile_offset;
};

#endif // MAPTILE_H
