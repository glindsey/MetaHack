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

/// Lighting data for a single maptile.
struct TileLightingData
{
  /// The calculated light levels of this tile and all of its walls.
  /// Mapping to an int is horribly hacky but I see no other alternative
  /// right now.
  std::map<unsigned int, Color> calculatedLightColors;

  /// A map of LightInfluences, representing the amount of light that
  /// each entity is contributing to this map tile.
  /// Levels for the various color channels are interpreted as such:
  /// 0 <= value <= 128: result = (original * (value / 128))
  /// 128 < value <= 255: result = max(original + (value - 128), 255)
  /// The alpha channel is ignored.
  std::map<EntityId, LightInfluence> lights;
};

/// System that handles lighting the map and all entities on it.
class SystemLighting : public SystemCRTP<SystemLighting>
{
public:
  SystemLighting(ComponentMap<ComponentAppearance>& appearance,
                 ComponentMap<ComponentHealth>& health,
                 ComponentMap<ComponentLightSource>& lightSource,
                 ComponentMap<ComponentPosition>& position);

  virtual ~SystemLighting();

  /// Recalculate map lighting.
  virtual void recalculate() override;

  void clearAllLightingData();

  /// Get the light shining on a tile.
  /// Syntactic sugar for getWallLightLevel(coords, Direction::Self).
  Color getLightLevel(IntVec2 coords) const;

  /// Get the light shining on a tile wall.
  Color getWallLightLevel(IntVec2 coords, Direction direction) const;

protected:
  /// Virtual override called after the map is changed.
  virtual void setMapNVO(MapId newMap) override;

  /// Clear lighting data for a tile.
  void clearLightingData(IntVec2 coords);

  /// Apply a light source to a location.
  void applyLightFrom(EntityId lightSource, EntityId location);

  void addLightInfluenceToTile(IntVec2 coords, EntityId source, LightInfluence influence);

  void addLightToMap(EntityId source);

  void doRecursiveLighting(EntityId source,
                           IntVec2 const& origin,
                           Color const& light_color,
                           int const max_depth_squared,
                           int octant,
                           int depth = 1,
                           float slope_A = 1,
                           float slope_B = 0);

private:
  // Components used by this system.
  ComponentMap<ComponentAppearance>& m_appearance;
  ComponentMap<ComponentHealth>& m_health;
  ComponentMap<ComponentLightSource>& m_lightSource;
  ComponentMap<ComponentPosition>& m_position;

  /// Grid of tile lighting data for all map tiles.
  std::unique_ptr<Grid2D<TileLightingData>> m_lightingData;

  /// Color of ambient lighting.
  Color m_ambientLightColor;
};