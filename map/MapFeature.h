#ifndef MAPFEATURE_H
#define MAPFEATURE_H

#include <memory>

#include "types/Direction.h"
#include "types/EntitySpecs.h"
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
  PropertyDictionary const& getSettings() const;

  size_t getNumOfGrowthVectors() const;
  GeoVector const& getRandomGrowthVector() const;
  bool eraseGrowthVector(GeoVector vec);

  static std::unique_ptr<MapFeature> construct(Map& gameMap, PropertyDictionary const& settings, GeoVector vec);

protected:
  MapFeature(Map& m, PropertyDictionary const& settings, GeoVector vec);

  void setCoords(sf::IntRect coords);
  void clearGrowthVectors();
  void addGrowthVector(GeoVector vec, bool highPriority = false);

  /// Check that all tiles within the area bounded by (upperLeft.x,
  /// upperLeft.y) to (lowerRight.x, lowerRight.y), inclusive,
  /// meet a specific criterion.
  /// @param upperLeft Coordinates of upper-left corner of box.
  /// @param lowerRight Coordinates of lower-right corner of box.
  /// @param criterion Functor that is passed a MapTile reference and should
  ///                  return true if the criterion is met and false if not.
  /// @return True if all tiles meet the criterion, false otherwise.
  bool doesBoxPassCriterion(IntVec2 upperLeft,
                            IntVec2 lowerRight,
                            std::function<bool(MapTile&)> criterion);

  /// Set all tiles within the area bounded by (upperLeft.x, upperLeft.y) to
  /// (lower_right.x, lower_right.y), inclusive, to the specified tile type.
  /// If any tiles are out of bounds for the map, they are ignored.
  /// @param upperLeft Coordinates of upper-left corner of box.
  /// @param lowerRight Coordinates of lower-right corner of box.
  /// @param floor Category and optional material to set tile floor to.
  /// @param space Category and optional material to set tile space to.
  void setBox(IntVec2 upperLeft, IntVec2 lowerRight, EntitySpecs floor, EntitySpecs space);

  /// Set all tiles bounded by an IntRect (inclusive) to the specified tile type.
  /// @param rect IntRect specifying the rectangle to set.
  /// @param floor Category and optional material to set tile floor to.
  /// @param space Category and optional material to set tile space to.
  void setBox(sf::IntRect rect, EntitySpecs floor, EntitySpecs space);

private:

  Map& m_gameMap;
  PropertyDictionary m_settings;
  sf::IntRect m_coords;
  GeoVector m_startVec;
  std::deque<GeoVector> m_highPriorityVecs;
  std::deque<GeoVector> m_lowPriorityVecs;
};

#endif // MAPFEATURE_H
