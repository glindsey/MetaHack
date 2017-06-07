#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "types/Vec2.h"

namespace Components
{

  /// Component that describes an entity's position -- either straight on a
  /// map, or inside another entity.
  class ComponentPosition final
  {
  public:
    ComponentPosition();
    explicit ComponentPosition(EntityId id);
    ComponentPosition(MapID map, IntVec2 coords);
    ~ComponentPosition();

    friend void from_json(json const& j, ComponentPosition& obj);
    friend void to_json(json& j, ComponentPosition const& obj);

    friend std::ostream& operator<<(std::ostream& os, Components::ComponentPosition const& obj)
    {
      os << "[Map: " << obj.m_map <<
        ", Coords: " << obj.m_coords <<
        ", Parent: " << obj.m_parent << "]";
      return os;
    }

    /// Sets this entity's position to be inside the specified entity.
    /// Its coordinates/MapID are set to (0, 0) because they are retrieved
    /// from the surrounding entity.
    void set(EntityId id);

    /// Sets this entity's position to be outside, on a map.
    /// Its entity location is set to Mu accordingly.
    void set(IntVec2 coords);

    /// Sets this entity's position to be outside, on a map.
    /// Its entity location is set to Mu accordingly.
    void set(MapID map, IntVec2 coords);

    /// Get surrounding entity, if any.
    /// If there's no surrounding entity, returns Mu.
    EntityId parent() const;

    /// Get map.
    /// If this entity is inside another, returns the ID of the parent's map.
    MapID map() const;

    /// Get coordinates.
    /// If this entity is inside another, returns the coordinates of the
    /// parent.
    IntVec2 coords() const;

    /// Return whether this entity is inside another entity or not.
    bool isInsideAnotherEntity() const;

    /// Return whether this entity is inside the specified entity.
    bool isInside(EntityId id) const;

    /// Return whether this entity is adjacent to the specified entity.
    bool isAdjacentTo(EntityId id) const;

  protected:

  private:
    EntityId m_parent;
    MapID m_map;
    IntVec2 m_coords;
  };

} // end namespace Components
