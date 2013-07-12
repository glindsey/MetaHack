#ifndef MAPTILE_H
#define MAPTILE_H

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "Inventory.h"
#include "MapId.h"
#include "MapTileType.h"
#include "Thing.h"

// Forward declarations
class Entity;

class MapTile : public Thing
{
  friend class ThingFactory;

  public:
    virtual ~MapTile();

    struct LightInfluence
    {
      sf::Vector2i coords;  ///< Coordinates of the light.
      sf::Color color;      ///< Color of the light.
      float intensity;   ///< Intensity of the light.
    };

    /// Return this tile's description.
    virtual std::string get_description() const override;

    /// Return the coordinates of the tile representing the thing.
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const;

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
    bool can_be_traversed_by(Entity& entity) const;

    /// Set the current tile's location.
    void set_coords(int x, int y);

    /// Get the current tile's location.
    sf::Vector2i const& get_coords() const;

    /// Get a reference to the map this tile belongs to.
    MapId get_map_id() const;

    /// Set the current tile's light level.
    void set_ambient_light_level(sf::Color level);

    /// Clear light influences.
    void clear_light_influences();

    /// Add a light influence to the tile.
    void add_light_influence(ThingId source, LightInfluence influence);

    /// Get the light shining on a tile.
    sf::Color get_light_level() const;

    /// Get whether the tile is opaque or not.
    bool is_opaque() const;

    void draw_highlight(sf::RenderTarget& target,
                       sf::Vector2f location,
                       sf::Color fgColor,
                       sf::Color bgColor,
                       int frame);

    virtual bool can_contain(Thing& thing) const override;

    virtual bool can_be_read_by(Entity const& entity) const override;
    virtual ActionResult do_action_read_by(Entity& entity) override;

    /// Get the coordinates associated with a tile.
    static sf::Vector2f get_pixel_coords(int x, int y);

    /// Get the coordinates associated with a tile.
    static sf::Vector2f get_pixel_coords(sf::Vector2i tile);

  protected:
    /// Constructor, callable only by ThingFactory.
    MapTile(MapId mapId, int x, int y);


  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // MAPTILE_H
