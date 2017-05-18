#include "components/ComponentCombustible.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentCombustible& obj)
{
  obj = ComponentCombustible();
  JSONUtils::doIfPresent(j, "burns-forever", [&](auto& value) { obj.m_burnsForever = value; });
  JSONUtils::doIfPresent(j, "burn-amount",   [&](auto& value) { obj.m_burnAmount = value; });
  JSONUtils::doIfPresent(j, "burn-speed",    [&](auto& value) { obj.m_burnSpeed = value; });
  JSONUtils::doIfPresent(j, "burn-results",  [&](auto& value) 
  { 
    if (value.is_array())
    {
      obj.m_burnResults.clear();
      for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
      {
        obj.m_burnResults.push_back(value.get<std::string>());
      }
    }
  });
}

void to_json(json& j, ComponentCombustible const& obj)
{
  j = json::object();
  j["burns-forever"] = obj.m_burnsForever;
  j["burn-amount"] = obj.m_burnAmount;
  j["burn-speed"] = obj.m_burnSpeed;
  json results = json::array();
  for (auto& item : obj.m_burnResults)
  {
    results.push_back(item);
  }
  j["burn-results"] = results;
}

ModifiableBool& ComponentCombustible::burnsForever()
{
  return m_burnsForever;
}

ModifiableBool const& ComponentCombustible::burnsForever() const
{
  return m_burnsForever;
}

int ComponentCombustible::burnAmount() const
{
  return m_burnAmount;
}

void ComponentCombustible::setBurnAmount(int burnAmount)
{
  m_burnAmount = burnAmount;
}

void ComponentCombustible::incBurnAmount(int incAmount)
{
  m_burnAmount += incAmount;
  // Disabled because this component doesn't know about the entity's mass.
  //int limit = (m_burnsForever) ? 5000 : 10000;
  //if (m_burnAmount > limit) m_burnAmount = limit;
}

void ComponentCombustible::decBurnAmount(int decAmount)
{
  incBurnAmount(-decAmount);
}


ModifiableInt& ComponentCombustible::burnSpeed()
{
  return m_burnSpeed;
}

ModifiableInt const& ComponentCombustible::burnSpeed() const
{
  return m_burnSpeed;
}

std::vector<std::string> const& ComponentCombustible::burnResults() const
{
  return m_burnResults;
}
