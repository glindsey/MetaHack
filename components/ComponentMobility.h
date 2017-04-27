#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that defines that this entity is capable of moving itself.
class ComponentMobility
{
public:

  friend void from_json(json const& j, ComponentMobility& obj);
  friend void to_json(json& j, ComponentMobility const& obj);

  unsigned int moveSpeed() const;
  void setMoveSpeed(unsigned int moveSpeed);

  unsigned int turnSpeed() const;
  void setTurnSpeed(unsigned int turnSpeed);

  unsigned int maxMoveSpeed() const;
  void setMaxMoveSpeed(unsigned int maxMoveSpeed);

protected:

private:
  /// Normal moving speed, in centimeters per second.
  /// Zero means the entity is fixed in place, but may still be able to turn.
  unsigned int m_moveSpeedInCMPS;

  /// Normal turning speed, in degrees per second.
  /// Zero means the entity is unable to turn.
  unsigned int m_turnSpeedInDPS;

  /// Maximum moving speed, in centimeters per second.
  /// If set below normal moving speed, will be equalized to normal moving speed.
  unsigned int m_maxMoveSpeedInCMPS;

};
