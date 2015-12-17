#ifndef MAP_H
#define MAP_H

#include <boost/ptr_container/ptr_deque.hpp>
#include <memory>
#include <SFML/Graphics.hpp>

#include "MapFactory.h"
#include "Renderable.h"
#include "Thing.h"

// Forward declarations
class MapFeature;
class MapGenerator;
class MapTile;
class ThingRef;

// VS compatibility
#ifdef WIN32   //WINDOWS
#define constexpr const
#endif

#include "MapImpl.h"
#include "UsesPimpl.h"

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
  void process();

  void update_lighting();

  void add_light(ThingRef source);

  void update_tile_vertices(ThingRef thing);

  void update_thing_vertices(ThingRef thing, int frame);

  void set_view(sf::RenderTarget& target,
                sf::Vector2f center,
                float zoom_level);

  void draw_to(sf::RenderTarget& target);

  MapTile const& get_tile(int x, int y) const;

  MapTile& get_tile(int x, int y);

  MapTile& get_tile(sf::Vector2i tile);

  bool tile_is_opaque(sf::Vector2i tile);

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

  /// Initialize a new Map.
  /// This unfortunately has to be separated from the constructor due to
  /// the fact that Lua scripts refer to Maps by ID. This means the Map
  /// must be inserted into the container of Maps before any Lua scripts
  /// can refer to it.
  void initialize();

  /// Recursively calculates lighting from the origin.
  /// Octant is an integer representing the following:
  /// \ 1|2 /  |
  ///  \ | /   |
  /// 8 \|/ 3  |
  /// ---+---  |
  /// 7 /|\ 4  |
  ///  / | \   |
  /// / 6|5 \  |

  void do_recursive_lighting(ThingRef source,
                             sf::Vector2i const& origin,
                             sf::Color const& light_color,
                             int const max_depth_squared,
                             int octant,
                             int depth = 1,
                             float slope_A = 1,
                             float slope_B = 0);

private:
  Pimpl<MapImpl> pImpl;

  std::unique_ptr<MapGenerator> m_generator;

  /// Lua function to get the tile contents Thing at a specific location.
  /// Takes three parameters:
  ///   - The MapID of the map in question.
  ///   - x, y location of the tile contents to retrieve
  /// It returns:
  ///   - ID of the requested Thing, or nil if it does not exist.
  /// Notes:
  ///   - The Map that the tile is retrieved from is the one the player is on.
  static int LUA_get_tile_contents(lua_State* L);

  /// Lua function to get the start coords for a map.
  /// Takes one parameter:
  ///   - The MapID of the map in question.
  /// It returns:
  ///   - The x, y location of the map's starting position.
  static int LUA_get_start_coords(lua_State* L);

  /// Lua function to add a feature to a map.
  /// Takes two parameters:
  ///   - The MapID of the map in question.
  ///   - A string indicating the type of feature to add.
  /// It returns:
  ///   - A boolean indicating whether the feature could be added to the map.
  static int LUA_map_add_feature(lua_State* L);
};

#endif // MAP_H
