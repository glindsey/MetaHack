#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "types/Vec2.h"

/// Template to use for making new components.
class ComponentPosition final
{
public:
  ComponentPosition();
  ComponentPosition(EntityId id);
  ComponentPosition(IntVec2 coords);
  ~ComponentPosition();

  friend void from_json(json const& j, ComponentPosition& obj);
  friend void to_json(json& j, ComponentPosition const& obj);

  /// Sets this entity's position to be inside the specified entity.
  /// Its coordinates are set to (0, 0) because they are retrieved from the
  /// surrounding entity.
  void set(EntityId id);

  /// Sets this entity's position to be outside, on the map.
  /// Its entity location is set to Mu accordingly.
  void set(IntVec2 coords);

  /// Get surrounding entity, if any.
  /// If there's no surrounding entity, returns Mu.
  EntityId parent();

  /// Get coordinates.
  /// If this entity is inside another, returns the coordinates of the
  /// parent.
  IntVec2 coords();

protected:

private:
  EntityId m_parent;
  IntVec2 m_coords;
};

