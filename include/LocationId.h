#ifndef _LOCATIONID_H_
#define _LOCATIONID_H_

#include "Location.h"
#include "MapId.h"
#include "ThingId.h"

#include <string>

enum class LocationType
{
  MapTile,
  Thing
};

struct LocationId
{
  LocationId();
  LocationId(ThingId thingId);
  LocationId(MapId mapId, int x, int y);

  bool isValid();

  std::string getDescription();

  Location& getReference();

  bool operator==(LocationId const& other) const;
  bool operator!=(LocationId const& other) const;

  LocationType  type;
  union
  {
    ThingId     thingId;
    struct
    {
      MapId     mapId;
      int x;
      int y;
    } mapTile;
  };
};

#endif // _LOCATIONID_H_
