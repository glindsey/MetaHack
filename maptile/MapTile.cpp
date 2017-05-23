#include "stdafx.h"

#include "maptile/MapTile.h"

#include "AssertHelper.h"
#include "components/ComponentPhysical.h"
#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "types/Color.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

bool MapTile::initialized = false;

MapTile::~MapTile()
{}

EntityId MapTile::getTileContents() const
{
  return m_tile_contents;
}

std::string MapTile::getDisplayName() const
{
  return getCategoryData().value("name", "MTUnknown");
}

void MapTile::setTileType(std::string category)
{
  m_category = category;
}

std::string MapTile::getTileType() const
{
  return m_category;
}

bool MapTile::isPassable() const
{
  auto& category = getCategoryData();
  Assert("Map",
         category.count("components") > 0 && category["components"].is_object(),
         "MapTile is missing components");

  auto& components = category["components"];
  Assert("Map",
         components.count("physical") > 0 && components["physical"].is_object(),
         "MapTile is missing \"physical\" component");

  // If the tile has a MatterState component and is non-solid, return true.
  // A tile containing a liquid/gas/plasma/etc. is passable. It may kill you,
  // but technically it's passable. ;)
  if (components.count("matter-state") > 0 && 
    components["matter-state"].get<std::string>().find("solid") == std::string::npos)
  {
    return true;
  }

  // Return whether the volume of the tile fills the entire tile.
  return (components["physical"].value("volume", 0) < ComponentPhysical::VOLUME_MAX_CC);
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal DynamicEntity can move through solid matter.
bool MapTile::canBeTraversedBy(EntityId entity) const
{
  return isPassable();
}

void MapTile::setCoords(IntVec2 coords)
{
  m_coords = coords;
}

IntVec2 const& MapTile::getCoords() const
{
  return m_coords;
}

MapID MapTile::map() const
{
  return m_mapID;
}

Color MapTile::getOpacity() const
{
  auto& category = getCategoryData();
  Assert("Map",
         category.count("components") > 0 && category["components"].is_object() &&
         category["components"].count("appearance") > 0 && category["components"]["appearance"].is_object(),
         "MapTile is missing \"appearance\" component");

  return getCategoryData()["components"]["appearance"].value("opacity", Color::White);
}

bool MapTile::isTotallyOpaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  return (getOpacity() == Color::White);
}

RealVec2 MapTile::getPixelCoords(IntVec2 tile)
{
  auto& config = Service<IConfigSettings>::get();
  float map_tile_size = config.get("map-tile-size");

  return RealVec2(static_cast<float>(tile.x) * map_tile_size,
                  static_cast<float>(tile.y) * map_tile_size);
}

// === PROTECTED METHODS ======================================================

MapTile::MapTile(IntVec2 coords, std::string category, MapID mapID)
  :
  Object({}),
  m_mapID{ mapID },
  m_coords{ coords },
  m_category{ category }
{
  if (!initialized)
  {
    // TODO: any static class initialization that must be performed
    initialized = true;
  }

  // Tile contents entity is created here, or else the pImpl would need the
  // "this" pointer passed in.
  /// @todo The type of this floor should eventually be specified as
  ///       part of the constructor.
  m_tile_contents = GAME.entities().createTileContents(this);
}

MapTile const& MapTile::getAdjacentTile(Direction direction) const
{
  IntVec2 coords = getCoords();
  IntVec2 adjacent_coords = coords + (IntVec2)direction;
  return MAPS.get(m_mapID).getTile(adjacent_coords);
}

json const& MapTile::getCategoryData() const
{
  return Service<IGameRules>::get().category(m_category);
}
