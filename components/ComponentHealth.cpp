#include "components/ComponentHealth.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentHealth& obj)
  {
    obj = ComponentHealth();

    // Max HP: If it's an array, pick from a uniform range. Otherwise, set it.
    JSONUtils::doIfPresent(j, "max-hp", [&](auto& value) { obj.m_maxHp = int(JSONUtils::getFancyInteger(value)); });
    obj.m_hp = obj.m_maxHp;
    JSONUtils::doIfPresent(j, "hp", [&](auto& value) { obj.m_hp = JSONUtils::getFancyInteger(value); });
    JSONUtils::doIfPresent(j, "dead", [&](auto& value) { obj.m_dead = value; });
    JSONUtils::doIfPresent(j, "living-creature", [&](auto& value) { obj.m_livingCreature = value; });
  }

  void to_json(json& j, ComponentHealth const& obj)
  {
    j = json::object();
    j["max-hp"] = obj.m_maxHp;
    j["hp"] = obj.m_hp;
    j["dead"] = obj.m_dead;
    j["living-creature"] = obj.m_livingCreature;
  }

  int ComponentHealth::hp() const
  {
    return m_hp;
  }

  void ComponentHealth::setHp(int hp)
  {
    m_hp = hp;
  }

  int ComponentHealth::maxHp() const
  {
    return m_maxHp;
  }

  void ComponentHealth::setMaxHp(int maxHp)
  {
    m_maxHp = maxHp;
  }

  bool ComponentHealth::hasHpBelowZero() const
  {
    return (m_hp <= 0);
  }

  bool ComponentHealth::isDead() const
  {
    return m_dead;
  }

  void ComponentHealth::setDead(bool dead)
  {
    m_dead = dead;
  }

  bool ComponentHealth::isLivingCreature() const
  {
    return m_livingCreature;
  }

} // end namespace
