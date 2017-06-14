#include "stdafx.h"

#include "components/ComponentMobility.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentMobility& obj)
  {
    obj = ComponentMobility();
    JSONUtils::doIfPresent(j, "move-speed", [&](auto& value) { obj.m_moveSpeedInCMPS = value; });
    JSONUtils::doIfPresent(j, "turn-speed", [&](auto& value) { obj.m_turnSpeedInDPS = value; });
    JSONUtils::doIfPresent(j, "max-move-speed", [&](auto& value) { obj.m_maxMoveSpeedInCMPS = value; });
  }

  void to_json(json& j, ComponentMobility const& obj)
  {
    j = json::object();
    j["move-speed"] = obj.m_moveSpeedInCMPS;
    j["turn-speed"] = obj.m_turnSpeedInDPS;
    j["max-move-speed"] = obj.m_maxMoveSpeedInCMPS;
  }

  unsigned int ComponentMobility::moveSpeed() const
  {
    return m_moveSpeedInCMPS;
  }

  void ComponentMobility::setMoveSpeed(unsigned int moveSpeed)
  {
    m_moveSpeedInCMPS = moveSpeed;
    m_maxMoveSpeedInCMPS = std::max(m_maxMoveSpeedInCMPS, moveSpeed);
  }

  unsigned int ComponentMobility::turnSpeed() const
  {
    return m_turnSpeedInDPS;
  }

  void ComponentMobility::setTurnSpeed(unsigned int turnSpeed)
  {
    m_turnSpeedInDPS = turnSpeed;
  }

  unsigned int ComponentMobility::maxMoveSpeed() const
  {
    return m_maxMoveSpeedInCMPS;
  }

  void ComponentMobility::setMaxMoveSpeed(unsigned int maxMoveSpeed)
  {
    m_maxMoveSpeedInCMPS = std::max(m_moveSpeedInCMPS, maxMoveSpeed);
  }

} // end namespace