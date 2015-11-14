#ifndef MAPFACTORY_H
#define MAPFACTORY_H

#include <boost/ptr_container/ptr_map.hpp>
#include <memory>

#include "MapId.h"
#include "MetadataCollection.h"

// Forward declarations
class Map;

// MapFactory is both a Factory for Maps and a store containing all of
// the Maps currently in use.

class MapFactory
{
  public:
    virtual ~MapFactory();

    static MapFactory& instance();

    /// Gets a reference to a Map by ID.
    Map const& get(MapId map_id) const;

    /// Gets a reference to a Map by ID.
    Map& get(MapId map_id);

    MapId create(int x, int y);

    bool destroy(MapId map_id);

    static const MapId null_map_id;

  protected:
  private:
    MapFactory();

    /// Collection of maps.
    boost::ptr_map<MapId, Map> maps_;

    MapId current_map_id;
    static std::unique_ptr<MapFactory> instance_;
};

// The following is a shortcut for something that will be typed A LOT.
#define MF        MapFactory::instance()

#endif // MAPFACTORY_H
