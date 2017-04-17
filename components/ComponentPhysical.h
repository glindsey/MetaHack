#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "map/MapId.h"

/// Component that describes physical dimensions of the entity:
/// mass, quantity (for aggregate entities), and volume.
class ComponentPhysical final
{
public:
  ComponentPhysical();
  ComponentPhysical(int mass, unsigned int quantity, unsigned int volume);
  ~ComponentPhysical();

  friend void from_json(json const& j, ComponentPhysical& obj);
  friend void to_json(json& j, ComponentPhysical const& obj);

  /// Get this entity's mass.
  int& mass();
  int const& mass() const;

  /// Get this entity's quantity.
  unsigned int& quantity();
  unsigned int const& quantity() const;

  /// Get this entity's volume.
  unsigned int& volume();
  unsigned int const& volume() const;

  /// Get this entity's total mass, which is mass * quantity.
  int totalMass();

  /// Get this entity's total volume, which is volume * quantity.
  int totalVolume();

protected:

private:
  int m_mass;
  unsigned int m_quantity;
  unsigned int m_volume;
};

