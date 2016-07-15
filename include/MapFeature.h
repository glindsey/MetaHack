#ifndef MAPFEATURE_H
#define MAPFEATURE_H

#include "stdafx.h"

#include <memory>

#include "Direction.h"
#include "GeoVector.h"
#include "Map.h"

/// Exception class for map features.
class MapFeatureException : std::exception
{
public:
  MapFeatureException()
    :
    std::exception(),
    m_what{ "Unknown MapFeatureException" }
  {}

  MapFeatureException(char const* what)
    :
    std::exception(),
    m_what{ what }
  {}

  virtual ~MapFeatureException()
  {}

  virtual const char* what() const
  {
    return m_what;
  }

private:
  char const* m_what;
};

/// Superclass for all map features (rooms, corridors, et cetera).  Maintains
/// a list of vectors at which new features can (theoretically) be attached.
class MapFeature
{
  friend class MapCorridor;
  friend class MapDiamond;
  friend class MapDonutRoom;
  friend class MapLRoom;
  friend class MapRoom;

public:
  virtual ~MapFeature();

  sf::IntRect const& get_coords() const;
  Map& get_map() const;
  PropertyDictionary const& get_settings() const;

  unsigned int get_num_growth_vectors() const;
  GeoVector const& get_random_growth_vector() const;
  bool erase_growth_vector(GeoVector vec);

  static std::unique_ptr<MapFeature> construct(Map& game_map, PropertyDictionary const& settings, GeoVector vec);

protected:
  MapFeature(Map& m, PropertyDictionary const& settings, GeoVector vec);

  void set_coords(sf::IntRect coords);
  void clear_growth_vectors();
  void add_growth_vector(GeoVector vec);

  /// Set all tiles within the area bounded by (upper_left.x, upper_left.y) to
  /// (lower_right.x, lower_right.y), inclusive, to the specified tile type.
  /// If any tiles are out of bounds for the map, they are ignored.
  /// @param upper_left Coordinates of upper-left corner of box.
  /// @param lower_right Coordinates of lower-right corner of box.
  /// @param tile_type Type to set tiles to.
  void set_box(sf::Vector2u upper_left, sf::Vector2u lower_right, std::string tile_type);

  /// Set all tiles bounded by an IntRect (inclusive) to the specified tile type.
  /// @param rect IntRect specifying the rectangle to set.
  /// @param tile_type Type to set tiles to.
  void set_box(sf::IntRect rect, std::string tile_type);

private:

  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // MAPFEATURE_H
