#ifndef MAPIMPL_H
#define MAPIMPL_H

#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <SFML/Graphics.hpp>

#include "MapId.h"
#include "MapFeature.h"
#include "MapTile.h"

struct MapImpl
{
  MapImpl(MapId map_id_, int width_, int height_)
    :
    map_id{ map_id_ },
    map_size{ width_, height_ }
  {}

  ~MapImpl()
  {
    tiles.clear();
    features.clear();
  }

  /// Map ID.
  MapId map_id;

  /// Map size.
  sf::Vector2i map_size;

  /// "Seen" map vertex array.
  sf::VertexArray map_seen_vertices;

  /// Outlines map vertex array.
  sf::VertexArray map_outline_vertices;

  /// "Memory" map vertex array.
  sf::VertexArray map_memory_vertices;

  /// Thing vertex array.
  sf::VertexArray thing_vertices;

  /// Pointer vector of tiles.
  boost::ptr_vector< MapTile > tiles;

  /// Player starting location.
  sf::Vector2i start_coords;

  /// Pointer deque of map features.
  boost::ptr_deque<MapFeature> features;
};

#endif // MAPIMPL_H
