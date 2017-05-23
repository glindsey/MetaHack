#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include "types/ModifiableBool.h"
#include "types/ModifiableInt.h"

#include <vector>

/// Component that indicates an entity can catch fire.
class ComponentCombustible
{
public:

  friend void from_json(json const& j, ComponentCombustible& obj);
  friend void to_json(json& j, ComponentCombustible const& obj);

  ModifiableBool& burnsForever();
  ModifiableBool const& burnsForever() const;

  int burnAmount() const;
  void setBurnAmount(int burnAmount);
  void incBurnAmount(int incAmount);
  void decBurnAmount(int decAmount);

  ModifiableInt& burnSpeed();
  ModifiableInt const& burnSpeed() const;

  std::vector<std::string> const & burnResults() const;

protected:

private:
  /// Flag indicating whether the entity can burn forever.
  ModifiableBool m_burnsForever;

  /// Value indicating how burnt the entity is.
  /// 0 indicates an unburned entity.
  /// When the value hits 100 * mass, it is destroyed and replaced by the contents
  /// of `m_burnResults`, unless `m_burnsForever` is true, in which case the 
  /// value will stop increasing at 50 * mass.
  int m_burnAmount;

  /// Value indicating how fast the entity burns.
  /// This value is added to `m_burnAmount` on each clock tick while the
  /// entity is burning.
  ModifiableInt m_burnSpeed;

  /// Vector of entity types created when this entity is fully burned.
  std::vector<std::string> m_burnResults;
};
