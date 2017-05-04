#ifndef MAP_H
#define MAP_H

#include "stdafx.h"

#include "Subject.h"
#include "map/MapFactory.h"
#include "types/Grid2D.h"
#include "types/IRenderable.h"
#include "entity/Entity.h"

// Forward declarations
class Color;
class GameState;
class MapFeature;
class MapGenerator;
class MapTile;
class EntityId;

// VS compatibility
#ifdef WIN32   //WINDOWS
#define constexpr const
#endif

/// Class representing a map, which is a grid of locations for Entities.
class Map
  :
  public Subject
{
  friend class MapFactory;
  friend class MapView;

public:
  ~Map();

  /// The maximum length of one side.
  static constexpr int max_dimension = 128;

  /// The maximum area of a map.
  static constexpr int max_area = max_dimension * max_dimension;

  /// Process all Entities on this map.
  void processEntities();

  MapTile const & getTile(IntVec2 tile) const;

  MapTile& getTile(IntVec2 tile);

  bool tileIsOpaque(IntVec2 tile);

  /// Get the map's size.
  IntVec2 const& getSize() const;

  /// Get player's starting location.
  IntVec2 const& getStartCoords() const;

  /// Set player's starting location.
  bool setStartCoords(IntVec2 start_coords);

  /// Get the index of a particular X/Y coordinate.
  int getIndex(IntVec2 coords) const;

  /// Get whether a particular X/Y coordinate is in bounds.
  bool isInBounds(IntVec2 coords) const;

  /// Calculate coordinates corresponding to a direction, if possible.
  bool calcCoords(IntVec2 origin,
                  Direction direction,
                  IntVec2& result);

  /// Get Map ID.
  MapId getMapId() const;

  /// @todo Not sure all the "feature" stuff should be public.
  ///       But not sure how to scope it better either.

  /// Clear the collection of map features.
  void clearMapFeatures();

  /// Get the collection of map features.
  boost::ptr_deque<MapFeature> const& getMapFeatures() const;

  /// Get a random map feature from the deque.
  MapFeature& getRandomMapFeature();

  /// Add a map feature to this map.
  /// The feature must be dynamically generated, and the map takes ownership
  /// of it when it is passed in.
  MapFeature& addMapFeature(MapFeature* feature);

protected:
  Map(GameState& game, MapId mapId, int width, int height);

  /// Initialize a new Map.
  /// This unfortunately has to be separated from the constructor due to
  /// the fact that Lua scripts refer to Maps by ID. This means the Map
  /// must be inserted into the container of Maps before any Lua scripts
  /// can refer to it.
  void initialize();

private:
  /// Reference to game state.
  GameState& m_game;

  /// Map ID.
  MapId m_map_id;

  /// Map size.
  IntVec2 m_map_size;

  std::unique_ptr<MapGenerator> m_generator;

  /// Grid of tiles.
  std::unique_ptr< Grid2D< MapTile > > m_tiles;

  /// Player starting location.
  IntVec2 m_start_coords;

  /// Pointer deque of map features.
  boost::ptr_deque<MapFeature> m_features;

  /// Lua function to get the tile contents Entity at a specific location.
  /// Takes three parameters:
  ///   - The MapID of the map in question.
  ///   - x, y location of the tile contents to retrieve
  /// It returns:
  ///   - ID of the requested Entity, or nil if it does not exist.
  /// Notes:
  ///   - The Map that the tile is retrieved from is the one the player is on.
  static int LUA_getTileContents(lua_State* L);

  /// Lua function to get the start coords for a map.
  /// Takes one parameter:
  ///   - The MapID of the map in question.
  /// It returns:
  ///   - The x, y location of the map's starting position.
  static int LUA_getStartCoords(lua_State* L);

  /// Lua function to add a feature to a map.
  /// Takes two parameters:
  ///   - The MapID of the map in question.
  ///   - A string indicating the type of feature to add.
  /// It returns:
  ///   - A boolean indicating whether the feature could be added to the map.
  static int LUA_mapAddFeature(lua_State* L);
};

#endif // MAP_H
