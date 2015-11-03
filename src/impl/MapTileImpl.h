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

  MapTileImpl(sf::Vector2i coords_, std::string type_, MapId map_id_)
    :
    map_id{ map_id_ },
    coords{ coords_ },
    type{ type_ },
    ambient_light_color{ sf::Color(192, 192, 192, 255) },
    p_metadata{ MapTileMetadata::get(type_) }
  {}

  ~MapTileImpl()
  {
    lights.clear();
  }

  MapId map_id;

  sf::Vector2i coords;

  std::string type;

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

  MapTileMetadata* p_metadata;
};


#endif // MAPTILEIMPL_H

