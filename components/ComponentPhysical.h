#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"
#include "game/GameState.h"
#include "map/MapId.h"

/// Component that describes physical dimensions of the entity:
/// mass, quantity (for aggregate entities), volume, etc.
class ComponentPhysical final
{
public:
  ComponentPhysical();
  ComponentPhysical(int mass, unsigned int quantity, unsigned int volume);
  ~ComponentPhysical();

  friend void from_json(json const& j, ComponentPhysical& obj);
  friend void to_json(json& j, ComponentPhysical const& obj);

  /// Get this entity's mass in grams.
  int& mass();
  int const& mass() const;

  /// Get this entity's quantity.
  unsigned int& quantity();
  unsigned int const& quantity() const;

  /// Get this entity's volume in cubic centimeters.
  unsigned int& volume();
  unsigned int const& volume() const;

  /// Get this entity's total mass, which is mass * quantity.
  int totalMass();

  /// Get this entity's total volume, which is volume * quantity.
  int totalVolume();

  /// Upper limit of volume that can fit in a single map tile.
  /// Each tile on the map is roughly 8 m^3 (e.g. a cube with 2m sides).
  /// That's 8,000,000 cubic centimeters.
  /// Thus, that's the upper limit that can fit in a single tile.
  static const unsigned int VOLUME_MAX_CC = 8000000;

protected:

private:
  int m_mass;
  unsigned int m_quantity;
  unsigned int m_volume;
};

