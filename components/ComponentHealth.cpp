#include "components/ComponentHealth.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentHealth& obj)
{
  obj = ComponentHealth();

  // Max HP: If it's an array, pick from a uniform range. Otherwise, set it.
  JSONUtils::doIfPresent(j, "max-hp", [&](auto& value) { obj.m_maxHp = JSONUtils::getFancyInteger(value); });
  obj.m_hp = obj.m_maxHp;
  JSONUtils::doIfPresent(j, "hp", [&](auto& value) { obj.m_hp = JSONUtils::getFancyInteger(value); });
  JSONUtils::doIfPresent(j, "dead", [&](auto& value) { obj.m_dead = value; });

}

void to_json(json& j, ComponentHealth const& obj)
{
  j = json::object();
  j["max-hp"] = obj.m_maxHp;
  j["hp"] = obj.m_hp;
  j["dead"] = obj.m_dead;
}

int & ComponentHealth::hp()
{
  return m_hp;
}

int const & ComponentHealth::hp() const
{
  return m_hp;
}

int & ComponentHealth::maxHp()
{
  return m_maxHp;
}

int const & ComponentHealth::maxHp() const
{
  return m_maxHp;
}

bool ComponentHealth::isDead() const
{
  return (m_hp <= 0);
}

bool & ComponentHealth::dead()
{
  return m_dead;
}

bool const & ComponentHealth::dead() const
{
  return m_dead;
}
