#ifndef MAPFACTORY_H
#define MAPFACTORY_H

#include "stdafx.h"

#include "MetadataCollection.h"

// Forward declarations
class GameState;
class Map;

// Using declarations
using MapId = unsigned int;

// MapFactory is both a Factory for Maps and a store containing all of
// the Maps currently in use.

class MapFactory
{
public:
  MapFactory();
  virtual ~MapFactory();

  /// Serialization function.
  template<class Archive>
  void serialize(Archive& archive)
  {
    /// @todo WRITE ME
    //archive(m_maps, current_map_id);
  }

  /// Gets a reference to a Map by ID.
  Map const& get(MapId map_id) const;

  /// Gets a reference to a Map by ID.
  Map& get(MapId map_id);

  MapId create(int x, int y);

  bool destroy(MapId map_id);

  static const MapId null_map_id;

protected:
private:
  /// Collection of maps.
  std::unordered_map<MapId, std::unique_ptr<Map>> m_maps;

  MapId current_map_id;
};

#endif // MAPFACTORY_H