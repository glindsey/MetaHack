#ifndef MAP_H
#define MAP_H

#include <boost/ptr_container/ptr_deque.hpp>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "MapFactory.h"
#include "Renderable.h"

// Forward declarations
class LightSource;
class MapFeature;
class MapTile;

/// Class representing a map, which is a grid of locations for Things.
class Map
{
  friend class MapFactory;

  public:
    virtual ~Map();

    /// The maximum length of one side.
    static constexpr int max_dimension = 128;

    /// The maximum area of a map.
    static constexpr int max_area = max_dimension * max_dimension;

    /// The default ambient light level.
    static const sf::Color ambient_light_level;

    /// Process all Things on this map.
    void do_process();

    void update_lighting();

    void add_light(LightSource& source);

    void update_tile_vertices(Entity& entity);

    void update_thing_vertices(Entity& entity, int frame);

    void set_view(sf::RenderTarget& target,
                  sf::Vector2f center,
                  float zoom_level);

    void draw_to(sf::RenderTarget& target);

    std::unique_ptr<MapTile>& get_tile(int x, int y) const;

    std::unique_ptr<MapTile>& get_tile(sf::Vector2i tile) const;

    /// Get the map's size.
    sf::Vector2i const& get_size() const;

    /// Get player's starting location.
    sf::Vector2i const& get_start_coords() const;

    /// Set player's starting location.
    bool set_start_coords(sf::Vector2i start_coords);

    /// Get the index of a particular X/Y coordinate.
    int get_index(int x, int y) const;

    /// Get whether a particular X/Y coordinate is in bounds.
    bool is_in_bounds(int x, int y) const;

    /// Calculate coordinates corresponding to a direction, if possible.
    bool calc_coords(sf::Vector2i origin,
                     Direction direction,
                     sf::Vector2i& result);

    /// Get Map ID.
    MapId get_map_id() const;

    /// Clear the collection of map features.
    void clear_map_features();

    /// Get the collection of map features.
    boost::ptr_deque<MapFeature> const& get_map_features() const;

    /// Get a random map feature from the deque.
    MapFeature& get_random_map_feature();

    /// Add a map feature to this map.
    /// The feature must be dynamically generated, and the map takes ownership
    /// of it when it is passed in.
    MapFeature& add_map_feature(MapFeature* feature);

  protected:
    Map(MapId mapId, int width, int height);

    /// Recursively calculates lighting from the origin.
    /// Octant is an integer representing the following:
    /// \ 1|2 /  |
    ///  \ | /   |
    /// 8 \|/ 3  |
    /// ---+---  |
    /// 7 /|\ 4  |
    ///  / | \   |
    /// / 6|5 \  |

    void do_recursive_lighting(LightSource& source,
                               sf::Vector2i const& origin,
                               sf::Color const& light_color,
                               int const max_depth_squared,
                               int octant,
                               int depth = 1,
                               float slope_A = 1,
                               float slope_B = 0);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;

};

#endif // MAP_H
