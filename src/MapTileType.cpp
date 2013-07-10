#include "MapTileType.h"

struct TileTypeInfo
{
  std::string name;
  std::string description;
  unsigned int x_coord;     ///< X coordinate, divided by 4
  unsigned int y_coord;     ///< Y coordinate
  bool opaque;
  bool passable;
};

TileTypeInfo const mapTileTypeInfo[] =
{
  { "Unknown",    "unexplored space",     1, 0, true,  false },
  { "FloorStone", "stone floor",          0, 1, false, true },
  { "WallStone",  "stone wall",           0, 2, true,  false },

  { "???",        "indescribable place",  0, 0, false, false }
};

std::string const& getMapTileTypeName(MapTileType type)
{
  if ((int)type < (int)MapTileType::Count)
  {
    return mapTileTypeInfo[(int)type].name;
  }
  else
  {
    return mapTileTypeInfo[(int)MapTileType::Count].name;
  }
}

std::string const& getMapTileTypeDescription(MapTileType type)
{
  if ((int)type < (int)MapTileType::Count)
  {
    return mapTileTypeInfo[(int)type].description;
  }
  else
  {
    return mapTileTypeInfo[(int)MapTileType::Count].description;
  }
}

sf::Vector2u const getMapTileTypeTileSheetCoords(MapTileType type)
{
  if ((int)type < (int)MapTileType::Count)
  {
    return sf::Vector2u(mapTileTypeInfo[(int)type].x_coord * 4,
                        mapTileTypeInfo[(int)type].y_coord);
  }
  else
  {
    return sf::Vector2u(mapTileTypeInfo[(int)MapTileType::Count].x_coord * 4,
                        mapTileTypeInfo[(int)MapTileType::Count].y_coord);
  }
}

bool const getMapTileTypeOpaque(MapTileType type)
{
  if ((int)type < (int)MapTileType::Count)
  {
    return mapTileTypeInfo[(int)type].opaque;
  }
  else
  {
    return mapTileTypeInfo[(int)MapTileType::Count].opaque;
  }
}

bool const getMapTileTypePassable(MapTileType type)
{
  if ((int)type < (int)MapTileType::Count)
  {
    return mapTileTypeInfo[(int)type].passable;
  }
  else
  {
    return mapTileTypeInfo[(int)MapTileType::Count].passable;
  }
}
