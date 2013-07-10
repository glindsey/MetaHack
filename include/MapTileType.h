#ifndef MAPTILETYPE_H
#define MAPTILETYPE_H

#include <SFML/Graphics.hpp>
#include <string>

// MapTileType is an enum of all the valid tile types that can be created.
enum class MapTileType
{
  Unknown,
  FloorStone,
  WallStone,

  Count
};

/// Helper function which returns the name of a map tile type.
std::string const& getMapTileTypeName(MapTileType type);

/// Helper function which returns the description of a map tile type.
std::string const& getMapTileTypeDescription(MapTileType type);

/// Helper function which returns the tile sheet coords of a map tile type.
sf::Vector2u const getMapTileTypeTileSheetCoords(MapTileType type);

/// Helper function which returns whether a map tile type is opaque or not.
bool const getMapTileTypeOpaque(MapTileType type);

/// Helper function which returns whether a map tile type is passable or not.
bool const getMapTileTypePassable(MapTileType type);

#endif // MAPTILETYPE_H
