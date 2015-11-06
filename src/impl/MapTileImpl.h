#ifndef MAPTILEIMPL_H
#define MAPTILEIMPL_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "LightInfluence.h"
#include "MapTileMetadata.h"
#include "ThingRef.h"

class MapTileImpl
{
public:

  MapTileImpl(sf::Vector2i coords_, Metadata& metadata_, MapId map_id_)
    :
    map_id{ map_id_ },
    coords{ coords_ },
    p_metadata{ &metadata_ },
    ambient_light_color{ sf::Color(192, 192, 192, 255) }
  {}

  ~MapTileImpl()
  {
    lights.clear();
  }

  MapId map_id;

  sf::Vector2i coords;

  /// Pointer to this MapTile's metadata.
  /// This has to be a pointer rather than a reference because it can be
  /// modified after MapTile construction.
  Metadata* p_metadata;

  /// Reference to the Thing that represents this tile's floor.
  ThingRef floor;

  /// Tile's light level.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  sf::Color ambient_light_color;

  /// A map of LightInfluences, representing the amount of light that
  /// each thing is contributing to this map tile.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  std::unordered_map<ThingRef, LightInfluence> lights;

};


#endif // MAPTILEIMPL_H

