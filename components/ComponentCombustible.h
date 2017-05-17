#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

#include <vector>

/// Component that indicates an entity can catch fire.
class ComponentCombustible
{
public:

  friend void from_json(json const& j, ComponentCombustible& obj);
  friend void to_json(json& j, ComponentCombustible const& obj);

  bool burnsForever() const;
  int burnAmount() const;
  int burnSpeed() const;
  std::vector<std::string> const & burnResults() const;

  void setBurnsForever(bool burnsForever);
  void setBurnAmount(int burnAmount);
  void incBurnAmount(int incAmount);
  void decBurnAmount(int decAmount);
  void setBurnSpeed(int burnSpeed);

protected:

private:
  /// Flag indicating whether the entity can burn forever.
  bool m_burnsForever;

  /// Value indicating how burnt the entity is.
  /// 0 indicates an unburned entity.
  /// When the value hits 10000, it is destroyed and replaced by the contents
  /// of `m_burnResults`, unless `m_burnsForever` is true, in which case the 
  /// value will stop increasing at 5000.
  int m_burnAmount;

  /// Value indicating how fast the entity burns.
  /// This value is added to `m_burnAmount` on each clock tick.
  int m_burnSpeed;

  /// Vector of entity types created when this entity is fully burned.
  std::vector<std::string> m_burnResults;
};
