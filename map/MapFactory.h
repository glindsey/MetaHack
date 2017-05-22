#ifndef MAPFACTORY_H
#define MAPFACTORY_H

#include "stdafx.h"

#include "map/MapId.h"

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

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    /// @todo WRITE ME
    //archive(m_maps, m_currentMapId);
  }

  /// Returns whether a specified MapId exists in the map.
  bool exists(MapId map_id) const;

  /// Gets a reference to a Map by ID.
  Map const& get(MapId map_id) const;

  /// Gets a reference to a Map by ID.
  Map& get(MapId map_id);

  MapId create(int x, int y);

  bool destroy(MapId map_id);

  static const MapId nullMapId;

protected:
private:
  /// Reference to current game state.
  GameState& m_gameState;

  /// Collection of maps.
  std::unordered_map<MapId, std::unique_ptr<Map>> m_maps;

  MapId m_currentMapId;
};

#endif // MAPFACTORY_H