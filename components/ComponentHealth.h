#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{

  /// Component for entities that have HP and can "die".
  /// (Doesn't necessarily mean the entity is "living" in the real sense
  ///  of the word...)
  class ComponentHealth
  {
  public:

    friend void from_json(json const& j, ComponentHealth& obj);
    friend void to_json(json& j, ComponentHealth const& obj);

    int hp() const;
    void setHp(int hp);

    int maxHp() const;
    void setMaxHp(int maxHp);

    /// Returns whether HP is below zero.
    bool hasHpBelowZero() const;

    /// Returns the "dead" flag.
    /// (This is different from `isDead` so that "just died" actions can be
    /// performed.)
    bool isDead() const;
    void setDead(bool dead);

    /// Returns whether this creature is considered a "living creature".
    /// Golems/undead creatures/etc. can have a "health" component but still
    /// not be considered "alive".
    bool isLivingCreature() const;

  protected:

  private:
    int m_hp = 1;
    int m_maxHp = int(1);
    bool m_dead = false;
    bool m_livingCreature = true;

  };

} // end namespace Components
