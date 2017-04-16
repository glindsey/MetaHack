#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "map/MapId.h"
#include "types/Vec2.h"

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

  /// Set this entity's mass.
  void setMass(int mass);

  /// Set this entity's quantity.
  void setQuantity(unsigned int quantity);

  /// Set this entity's volume.
  void setVolume(unsigned int volume);

  /// Get this entity's mass.
  int mass();

  /// Get this entity's quantity.
  unsigned int quantity();

  /// Get this entity's volume.
  unsigned int volume();

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

