#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component for entities that have HP and can "die".
/// (Doesn't necessarily mean the entity is "living" in the real sense
///  of the word...)
class ComponentHealth
{
public:

  friend void from_json(json const& j, ComponentHealth& obj);
  friend void to_json(json& j, ComponentHealth const& obj);

  int& hp();
  int const& hp() const;

  int& maxHp();
  int const& maxHp() const;

  /// Returns whether HP is below zero.
  bool isDead() const;

  /// Returns the "dead" flag.
  /// (This is different from `isDead` so that "just died" actions can be 
  /// performed.)
  bool& dead();
  bool const& dead() const;

protected:

private:
  int m_hp = 1;
  int m_maxHp = 1;
  bool m_dead = false;

};
