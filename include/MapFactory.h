#ifndef MAPFACTORY_H
#define MAPFACTORY_H

#include <boost/ptr_container/ptr_map.hpp>
#include <memory>

#include "MapId.h"
#include "MetadataCollection.h"

// Forward declarations
class GameState;
class Map;

// MapFactory is both a Factory for Maps and a store containing all of
// the Maps currently in use.

class MapFactory
{
public:
  explicit MapFactory(GameState& game_state);
  virtual ~MapFactory();

  /// Gets a reference to a Map by ID.
  Map const& get(MapId map_id) const;

  /// Gets a reference to a Map by ID.
  Map& get(MapId map_id);

  MapId create(int x, int y);

  bool destroy(MapId map_id);

  static const MapId null_map_id;

protected:
private:
  /// Reference to owning GameState.
  GameState& m_game_state;

  /// Collection of maps.
  boost::ptr_map<MapId, Map> m_maps;

  MapId current_map_id;
};

#endif // MAPFACTORY_H
