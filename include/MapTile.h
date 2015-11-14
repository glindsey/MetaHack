#ifndef MAPTILE_H
#define MAPTILE_H

#include <cstdbool>
#include <SFML/Graphics.hpp>

#include "Direction.h"
#include "GameObject.h"
#include "Inventory.h"
#include "LightInfluence.h"
#include "MapId.h"
#include "Thing.h"
#include "ThingRef.h"
#include "TileSheet.h"

// Forward declarations
class Entity;
class Floor;
class Metadata;

class MapTile : public GameObject
{
  friend class Map;

  public:
    virtual ~MapTile();

    /// Get the tile's floor object.
    ThingRef get_floor() const;

    /// Return this tile's description.
    virtual std::string get_display_name() const override final;

    /// Return the coordinates of the tile representing the thing.
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const;

    /// Add this MapTile to a VertexArray to be drawn.
    /// @param vertices Array to add vertices to.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    virtual void add_vertices_to(sf::VertexArray& vertices,
                                 bool use_lighting = true,
                                 int frame = 0) override;

    /// Draw this MapTile onto a RenderTexture, at the specified coordinates.
    /// @param target Texture to draw onto.
    /// @param target_coords Coordinates to draw the MapTile at.
    /// @param target_size Target size of thing, in pixels.
    /// @param use_lighting If true, calculate lighting when adding.
    ///                     If false, store directly w/white bg color.
    /// @param frame Animation frame number.
    virtual void draw_to(sf::RenderTexture& target,
                         sf::Vector2f target_coords,
                         unsigned int target_size = 0,
                         bool use_lighting = true,
                         int frame = 0) override;

    /// Sets the map tile type, without doing gameplay checks.
    /// Used to set up the map before gameplay begins.
    /// @param tileType Type of the tile.
    /// @return None.
    void set_type(std::string tileType);

    /// Gets the current map tile type.
    /// @return Type of the tile.
    std::string get_type() const;

    /// Returns whether a tile is empty space, e.g. no wall in the way.
    bool is_empty_space() const;

    /// Returns whether a tile can be traversed by a certain Entity.
    bool can_be_traversed_by(ThingRef thing) const;

    /// Set the current tile's location.
    void set_coords(int x, int y);

    /// Get the current tile's location.
    sf::Vector2i const& get_coords() const;

    /// Get a reference to the map this tile belongs to.
    MapId get_map_id() const;

    /// Set the current tile's light level.
    void set_ambient_light_level(sf::Color level);

    /// Receive light from the specified LightSource.
    /// Gets the Map this tile belongs to and does a recursive
    /// raycasting algorithm on it.
    virtual void be_lit_by(ThingRef light);

    /// Clear light influences.
    void clear_light_influences();

    /// Add a light influence to the tile.
    void add_light_influence(ThingRef source,
                             LightInfluence influence);

    /// Get the light shining on a tile.
    sf::Color get_light_level() const;

    /// Get the light shining on a tile wall.
    sf::Color get_wall_light_level(Direction direction) const;

    /// Get whether the tile is opaque or not.
    bool is_opaque() const;

    void draw_highlight(sf::RenderTarget& target,
                        sf::Vector2f location,
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
    void add_walls_to(sf::VertexArray& vertices,
                      bool use_lighting,
                      bool nw_is_empty, bool n_is_empty,
                      bool ne_is_empty, bool e_is_empty,
                      bool se_is_empty, bool s_is_empty,
                      bool sw_is_empty, bool w_is_empty);

    /// Get the coordinates associated with a tile.
    static sf::Vector2f get_pixel_coords(int x, int y);

    /// Get the coordinates associated with a tile.
    static sf::Vector2f get_pixel_coords(sf::Vector2i tile);

  protected:
    /// Constructor, callable only by Map class.
    MapTile(sf::Vector2i coords, Metadata& metadata, MapId map_id);

    /// Get a reference to an adjacent tile.
    MapTile const & get_adjacent_tile(Direction direction) const;

  private:
    static bool initialized;

    /// The ID of the Map this MapTile belongs to.
    MapId m_map_id;

    /// This MapTile's coordinates on the map.
    sf::Vector2i m_coords;

    /// Pointer to this MapTile's metadata.
    /// This has to be a pointer rather than a reference because it can be
    /// modified after MapTile construction.
    Metadata* m_pMetadata;

    /// Reference to the Thing that represents this tile's floor.
    ThingRef m_floor;

    /// Tile's light level.
    /// Levels for the various color channels are interpreted as such:
    /// 0 <= value <= 128: result = (original * (value / 128))
    /// 128 < value <= 255: result = max(original + (value - 128), 255)
    /// The alpha channel is ignored.
    sf::Color m_ambient_light_color;

    /// A map of LightInfluences, representing the amount of light that
    /// each thing is contributing to this map tile.
    /// Levels for the various color channels are interpreted as such:
    /// 0 <= value <= 128: result = (original * (value / 128))
    /// 128 < value <= 255: result = max(original + (value - 128), 255)
    /// The alpha channel is ignored.
    std::unordered_map<ThingRef, LightInfluence> m_lights;

    /// Random tile offset.
    int m_tile_offset;
};

#endif // MAPTILE_H
