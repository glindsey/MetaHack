#ifndef MAPFACTORY_H
#define MAPFACTORY_H

// Forward declarations
class GameState;
class Map;

// MapFactory is both a Factory for Maps and a store containing all of
// the Maps currently in use.

class MapFactory
{
public:
  MapFactory(GameState& game);
  virtual ~MapFactory();

  /// Returns whether a specified Map exists.
  bool exists(MapID id) const
  {
    return (m_maps.count(id) != 0);
  }

  /// Gets a reference to a Map by ID.
  Map const& get(MapID map_id) const;

  /// Gets a reference to a Map by ID.
  Map& get(MapID map_id);

  /// @todo Update so we pass in the MapID we want, and the function will
  ///       either use that ID, or create a similar one (e.g. if "main" is
  ///       passed in, and already exists, it will make a map named "main2"
  ///       or something to that effect.)
  MapID create(int x, int y);

  bool destroy(MapID map_id);

protected:
private:
  /// Reference to current game state.
  GameState& m_gameState;

  /// Collection of maps.
  std::unordered_map<MapID, std::unique_ptr<Map>> m_maps;

  MapID m_currentMapID;
};

#endif // MAPFACTORY_H