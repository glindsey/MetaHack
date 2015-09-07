#ifndef MAPTILE_H
#define MAPTILE_H

#include <SFML/Graphics.hpp>

#include "GameObject.h"
#include "Inventory.h"
#include "MapId.h"
#include "MapTileType.h"
#include "Thing.h"
#include "ThingRef.h"

// Forward declarations
class Entity;
class Floor;

class MapTile : public GameObject
{
  friend class Map;

  public:
    virtual ~MapTile();

    struct LightInfluence
    {
      sf::Vector2i coords;  ///< Coordinates of the light.
      sf::Color color;      ///< Color of the light.
      double intensity;     ///< Intensity of the light.
    };

    /// Get the tile's floor object.
    ThingRef get_floor() const;

    /// Return this tile's description.
    virtual std::string get_pretty_name() const override final;

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
    /// @param tileType MapTileType of the tile.
    /// @return None.
    void set_type(MapTileType tileType);

    /// Gets the current map tile type.
    /// @return MapTileType of the tile.
    MapTileType get_type() const;

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
    /// Constructor, callable only by ThingFactory.
    MapTile(sf::Vector2i coords, MapId mapId);

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif // MAPTILE_H
