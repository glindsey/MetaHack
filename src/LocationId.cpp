#include "LocationId.h"

#include "ErrorHandler.h"
#include "IsType.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "Thing.h"
#include "ThingFactory.h"

#include <sstream>

LocationId::LocationId()
{
  type = LocationType::Thing;
  thingId = 0;
}

LocationId::LocationId(ThingId id)
{
  type = LocationType::Thing;
  thingId = id;
}

LocationId::LocationId(MapId m, int x, int y)
{
  type = LocationType::MapTile;
  mapTile.mapId = m;
  mapTile.x = x;
  mapTile.y = y;
}

bool LocationId::isValid()
{
  switch (type)
  {
  case LocationType::MapTile:
    return (MF.get(mapTile.mapId).inBounds(mapTile.x, mapTile.y));

  case LocationType::Thing:
    return (isType(&TF.get(thingId), Location));

  default:
    return false;
  }
}

// TODO: Hide or obscure thing numbers based on a debug setting.
std::string LocationId::getDescription()
{
  std::stringstream s;

  switch (type)
  {
  case LocationType::MapTile:
    s << "tile (" << mapTile.x << ", " << mapTile.y <<
         ") of map #" << mapTile.mapId;
    return s.str();
  case LocationType::Thing:
    s << TF.get(thingId).getName() << " (Thing #" << thingId << ")";
    return s.str();
  default:
    s << "a LocationId with unknown type " << static_cast<int>(type);
    return s.str();
  }
}

Location& LocationId::getReference()
{
  switch (type)
  {
  case LocationType::MapTile:
    {
      try
      {
        Map& m = MF.get(mapTile.mapId);
        MapTile& tile = m.getTile(mapTile.x, mapTile.y);
        return dynamic_cast<Location&>(tile);
      }
      catch (std::bad_cast const& e)
      {
        // All MapTiles are Locations, so this should not be possible...
        FATAL_ERROR("Could not cast tile (%d, %d) of Map %d to a Location!", mapTile.x, mapTile.y, mapTile.mapId);
        // won't get here, but placed to shut compiler up
        return dynamic_cast<Location&>(TF.getLimbo());
      }
    }
    case LocationType::Thing:
      try
      {
        return dynamic_cast<Location&>(TF.get(thingId));
      }
      catch (std::bad_cast const& e)
      {
        // This shouldn't happen, but other bad code might try to move a Thing
        // into a non-Location Thing.
        MINOR_ERROR("Attempted to cast Thing #%d (\"%s\") to a Location.  Returning Limbo instead.",
                    thingId, TF.get(thingId).getDescription().c_str());
        return dynamic_cast<Location&>(TF.getLimbo());
      }

    default:
      MAJOR_ERROR("Invalid location type in LocationId!");
      return dynamic_cast<Location&>(TF.getLimbo());
  }
}

bool LocationId::operator==(LocationId const& other) const
{
  if (this->type == other.type)
  {
    switch (this->type)
    {
    case LocationType::Thing:
      return (this->thingId == other.thingId);
    case LocationType::MapTile:
      return ((this->mapTile.mapId == other.mapTile.mapId) &&
              (this->mapTile.x == other.mapTile.x) &&
              (this->mapTile.y == other.mapTile.y));
    default:
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool LocationId::operator!=(LocationId const& other) const
{
  return !(*this == other);
}
