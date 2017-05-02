#ifndef MAPFEATURE_H
#define MAPFEATURE_H

#include "stdafx.h"

#include <memory>

#include "types/Direction.h"
#include "types/GeoVector.h"
#include "map/Map.h"
#include "properties/PropertyDictionary.h"

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

  sf::IntRect const& getCoords() const;
  Map& getMap() const;
  PropertyDictionary const& get_settings() const;

  size_t get_num_growth_vectors() const;
  GeoVector const& get_random_growth_vector() const;
  bool erase_growth_vector(GeoVector vec);

  static std::unique_ptr<MapFeature> construct(Map& game_map, PropertyDictionary const& settings, GeoVector vec);

protected:
  MapFeature(Map& m, PropertyDictionary const& settings, GeoVector vec);

  void setCoords(sf::IntRect coords);
  void clear_growth_vectors();
  void add_growth_vector(GeoVector vec, bool highPriority = false);

  /// Check that all tiles within the area bounded by (upper_left.x,
  /// upper_left.y) to (lower_right.x, lower_right.y), inclusive,
  /// meet a specific criterion.
  /// @param upper_left Coordinates of upper-left corner of box.
  /// @param lower_right Coordinates of lower-right corner of box.
  /// @param criterion Functor that is passed a MapTile reference and should
  ///                  return true if the criterion is met and false if not.
  /// @return True if all tiles meet the criterion, false otherwise.
  bool does_box_pass_criterion(IntVec2 upper_left,
                               IntVec2 lower_right,
                               std::function<bool(MapTile&)> criterion);

  /// Set all tiles within the area bounded by (upper_left.x, upper_left.y) to
  /// (lower_right.x, lower_right.y), inclusive, to the specified tile type.
  /// If any tiles are out of bounds for the map, they are ignored.
  /// @param upper_left Coordinates of upper-left corner of box.
  /// @param lower_right Coordinates of lower-right corner of box.
  /// @param tile_type Type to set tiles to.
  void set_box(IntVec2 upper_left, IntVec2 lower_right, std::string tile_type);

  /// Set all tiles bounded by an IntRect (inclusive) to the specified tile type.
  /// @param rect IntRect specifying the rectangle to set.
  /// @param tile_type Type to set tiles to.
  void set_box(sf::IntRect rect, std::string tile_type);

private:

  Map& m_gameMap;
  PropertyDictionary m_settings;
  sf::IntRect m_coords;
  GeoVector m_start_vec;
  std::deque<GeoVector> m_highPriorityVecs;
  std::deque<GeoVector> m_lowPriorityVecs;
};

#endif // MAPFEATURE_H
