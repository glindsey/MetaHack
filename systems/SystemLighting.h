#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "map/MapId.h"
#include "systems/SystemCRTP.h"
#include "types/Color.h"
#include "types/Direction.h"
#include "types/Grid2D.h"
#include "types/LightInfluence.h"

// Forward declarations
class ComponentAppearance;
class ComponentHealth;
class ComponentLightSource;
class ComponentPosition;

/// System that handles lighting the map and all entities on it.
class SystemLighting : public SystemCRTP<SystemLighting>
{
public:
  SystemLighting(ComponentMap<ComponentAppearance> const& appearance,
                 ComponentMap<ComponentHealth> const& health,
                 ComponentMap<ComponentLightSource>& lightSource,
                 ComponentMap<ComponentPosition> const& position);

  virtual ~SystemLighting();

  /// Recalculate map lighting.
  virtual void doCycleUpdate() override;

  void resetAllMapLightingData(MapId map);

  void clearMapLightingCalculations();

  /// Get the light shining on a tile.
  /// Syntactic sugar for getWallLightLevel(coords, Direction::Self).
  Color getLightLevel(IntVec2 coords) const;

  /// Get the light shining on a tile wall.
  Color getWallLightLevel(IntVec2 coords, Direction direction) const;

protected:
  /// Virtual override called after the map is changed.
  virtual void setMapNVO(MapId newMap) override;

  /// Apply a light source to a location.
  /// Traverses up the location chain until it finds either a map tile or an
  /// opaque container. If it makes it all the way to the map tile, adds the
  /// light to the map.
  void applyLightFrom(EntityId lightSource, EntityId location);

  /// Tally all lights shining on this tile and calculate resulting light levels.
  void calculateTileLightLevels(IntVec2 coords);

  /// Add the light from the specified source to the tile's light level.
  void addLightToTileLightLevels(IntVec2 coords, EntityId source);

  /// Add the specified light source to the map.
  /// Does a recursive raycasting search to determine which tiles the light
  /// ends up shining on, and marks those tiles as needing recalculation.
  void addLightToMap(EntityId source);

  /// Removes the specified light source from the map.
  /// Marks all tiles previously shined on by this light source as needing
  /// recalculation.
  void removeLightFromMap(EntityId source);

  /// Mark a tile as being shined upon by the specified light source.
  /// @todo Objects on the tile should have `on_lit()` called. The call should
  ///       travel down inventory chains until hitting an opaque container.
  void addLightToTile(IntVec2 coords, EntityId source);

  /// Remove a light source as shining on a tile.
  void removeLightFromTile(IntVec2 coords, EntityId source);

  /// Recursive function used to raycast lighting.
  void doRecursiveLighting(EntityId source,
                           IntVec2 const& origin,
                           int const max_depth_squared,
                           int octant,
                           int depth = 1,
                           float slope_A = 1,
                           float slope_B = 0);

  virtual bool onEvent(Event const & event) override;

private:
  // Components used by this system.
  ComponentMap<ComponentAppearance> const& m_appearance;
  ComponentMap<ComponentHealth> const& m_health;
  ComponentMap<ComponentLightSource>& m_lightSource;
  ComponentMap<ComponentPosition> const& m_position;

  /// Flags indicating whether tile light are valid.
  /// Clear flag to indicate a tile needs recalculation.
  using TileCalculationValidFlags = Grid2D<bool>;
  std::unique_ptr<TileCalculationValidFlags> m_tileCalculationValid;

  /// Calculated light colors for map tile floors and walls.
  using TileCalculatedLightColors = Grid2D<std::map<unsigned int, Color>>;
  std::unique_ptr<TileCalculatedLightColors> m_tileCalculatedLightColors;

  /// Map of coordinates to sets of lights that shine on them.
  using TileLightData = Grid2D<std::set<EntityId>>;
  std::unique_ptr<TileLightData> m_tileLightSet;

  /// Map of lights to sets of coordinates that they are influencing.
  /// Basically the inverse of `m_tileLightSet` stored redundantly for lookup
  /// speed.
  using LightTileData = std::unordered_map<EntityId, std::unordered_set<IntVec2>>;
  LightTileData m_lightTileSet;

  /// Boolean indicating if all lights should be recalculated.
  /// (Instead of adding every single light source to the recalculate set,
  ///  which is a waste of time.)
  bool m_recalculateAllLights = false;

  /// Set of lights that need their influences recalculated.
  std::unordered_set<EntityId> m_lightsToRecalculate;

  /// Color of ambient lighting.
  Color m_ambientLightColor;
};