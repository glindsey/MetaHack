#include "stdafx.h"

#include "maptile/MapTile.h"

#include "AssertHelper.h"
#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "entity/EntityFactory.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "types/Color.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

bool MapTile::initialized = false;

MapTile::~MapTile()
{}

EntityId MapTile::getSpaceEntity() const
{
  return m_tileSpace;
}

EntityId MapTile::getFloorEntity() const
{
  return m_tileFloor;
}

EntityId MapTile::getDisplayEntity() const
{
  return (isTotallyOpaque() ? getSpaceEntity() : getFloorEntity());
}

std::string MapTile::getDisplayName() const
{
  auto& spaceCategory = m_components.category.of(m_tileSpace);
  auto& floorCategory = m_components.category.of(m_tileFloor);

  /// @todo FINISH ME
  return spaceCategory;
}

void MapTile::setTileSpace(EntitySpecs specs)
{
  m_entities.morph(m_tileSpace, specs);
}

void MapTile::setTileFloor(EntitySpecs specs)
{
  m_entities.morph(m_tileFloor, specs);
}

void MapTile::setTileType(EntitySpecs floor, EntitySpecs space)
{
  setTileFloor(floor);
  setTileSpace(space);
}

EntitySpecs MapTile::getTileFloorSpecs() const
{
  auto& category = m_components.category.of(m_tileFloor);
  auto& material = m_components.material.existsFor(m_tileFloor) ? 
    m_components.material.of(m_tileFloor) : "";
  return { category, material };
}

EntitySpecs MapTile::getTileSpaceSpecs() const
{
  auto& category = m_components.category.of(m_tileSpace);
  auto& material = m_components.material.existsFor(m_tileSpace) ?
    m_components.material.of(m_tileSpace) : "";
  return { category, material };
}

bool MapTile::isPassable() const
{
  // If the tile space has no Physical component, it's passable.
  if (!m_components.physical.existsFor(m_tileSpace))
  {
    return true;
  }

  auto& physical = m_components.physical.of(m_tileSpace);

  // If the tile has a MatterState component and is non-solid, return true.
  // A tile containing a liquid/gas/plasma/etc. is passable. It may kill you,
  // but technically it's passable. ;)
  if (m_components.matterState.existsFor(m_tileSpace))
  {
    auto& matterState = m_components.matterState.of(m_tileSpace);

    if (matterState.isFluid())
    {
      return true;
    }
  }

  // Return whether the volume of the tile fills the entire tile.
  return (physical.volume().value() < Components::ComponentPhysical::VOLUME_MAX_CC);
}

/// @todo: Implement this to cover different entity types.
///        For example, a non-corporeal DynamicEntity can move through solid matter.
bool MapTile::canBeTraversedBy(EntityId entity) const
{
  return isPassable();
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
  // If the tile space has no opacity data, opacity is max (i.e. totally opaque).
  if (!m_components.appearance.existsFor(m_tileSpace))
  {
    return Color::White;
  }

  auto& appearance = m_components.appearance.of(m_tileSpace);
  return appearance.opacity();
}

bool MapTile::isTotallyOpaque() const
{
  /// @todo Check the tile's inventory to see if there's anything huge enough
  ///       to block the view of stuff behind it.
  return (getOpacity() == Color::White);
}

RealVec2 MapTile::getPixelCoords(IntVec2 tile)
{
  auto& config = S<IConfigSettings>();
  float map_tile_size = config.get("map-tile-size");

  return RealVec2(static_cast<float>(tile.x) * map_tile_size,
                  static_cast<float>(tile.y) * map_tile_size);
}

// === PROTECTED METHODS ======================================================

MapTile::MapTile(IntVec2 coords, 
                 MapID mapID, 
                 EntityFactory& entities,
                 Components::ComponentManager& components,
                 std::string floorCategory,
                 std::string spaceCategory)
  :
  Object({}),
  m_entities{ entities },
  m_components{ components },
  m_mapID{ mapID },
  m_coords{ coords }
{
  if (!initialized)
  {
    // TODO: any static class initialization that must be performed
    initialized = true;
  }

  // Tile entities are created here.
  m_tileSpace = entities.createTileEntity(this, floorCategory.empty() ? "Pit" : floorCategory);
  m_tileFloor = entities.createTileEntity(this, spaceCategory.empty() ? "OpenSpace" : spaceCategory);
}

MapTile const& MapTile::getAdjacentTile(Direction direction) const
{
  IntVec2 coords = getCoords();
  IntVec2 adjacent_coords = coords + (IntVec2)direction;
  return MAPS.get(m_mapID).getTile(adjacent_coords);
}
