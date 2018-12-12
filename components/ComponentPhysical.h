#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component that describes physical dimensions of the entity:
  /// mass, quantity (for aggregate entities), volume, etc.
  class ComponentPhysical final
  {
  public:
    friend void from_json(json const& j, ComponentPhysical& obj);
    friend void to_json(json& j, ComponentPhysical const& obj);

    /// Get this entity's mass in grams.
    /// If the entity also has a Quantity component, this is the mass of a
    /// *single* piece of the aggregate, not the whole.
    int& mass();
    int const& mass() const;

    /// Get this entity's volume in cubic centimeters.
    /// If the entity also has a Quantity component, this is the volume of a
    /// *single* piece of the aggregate, not the whole.
    int& volume();
    int const& volume() const;

    /// Upper limit of volume that can fit in a single map tile.
    /// Each tile on the map is roughly 8 m^3 (e.g. a cube with 2m sides).
    /// That's 8,000,000 cubic centimeters.
    /// Thus, that's the upper limit that can fit in a single tile.
    static const unsigned int VOLUME_MAX_CC = 8000000;

  protected:

  private:
    int m_mass = int(1000);
    int m_volume = int(1000);
  };

} // end namespace Components
