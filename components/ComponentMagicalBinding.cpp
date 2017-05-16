#include "components/ComponentMagicalBinding.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentMagicalBinding& obj)
{
  obj = ComponentMagicalBinding();

  JSONUtils::doIfPresent(j, "binder", [&](auto& value) { obj.m_binder = EntityId(value.get<unsigned int>()); });
  JSONUtils::doIfPresent(j, "against", [&](auto& value) { obj.m_against = value; }); // ComponentMagicalBinding::Against(value.get<Bits32>());
  JSONUtils::doIfPresent(j, "active", [&](auto& value) { obj.m_active = value; });
  JSONUtils::doIfPresent(j, "auto-expiration-time", [&](auto& value) { obj.m_autoExpirationTime = value; });
  JSONUtils::doIfPresent(j, "active-time-remaining", [&](auto& value) { obj.m_activeTimeRemaining = value; });

}

void to_json(json& j, ComponentMagicalBinding const& obj)
{
  j = json::object();
  j["binder"] = obj.m_binder;
  j["against"] = obj.m_against;
  j["active"] = obj.m_active;
  j["auto-expiration-time"] = obj.m_autoExpirationTime;
  j["active-time-remaining"] = obj.m_activeTimeRemaining;
}

ComponentMagicalBinding::ComponentMagicalBinding() :
  m_binder{ EntityId::Mu() },
  m_against{ 0 },
  m_active{ false },
  m_autoExpirationTime{ 0 },
  m_activeTimeRemaining{ 0 }
{}

EntityId ComponentMagicalBinding::binder() const
{
  return m_binder;
}

void ComponentMagicalBinding::setBinder(EntityId binder)
{
  m_binder = binder;
}

ComponentMagicalBinding::Against& ComponentMagicalBinding::against()
{
  return m_against;
}

ComponentMagicalBinding::Against const& ComponentMagicalBinding::against() const
{
  return m_against;
}

bool ComponentMagicalBinding::isAgainst(Bits32 value) const
{
  return against().action(value);
}

bool ComponentMagicalBinding::isActive() const
{
  return m_active;
}

void ComponentMagicalBinding::activate()
{
  m_active = true;
  m_activeTimeRemaining = m_autoExpirationTime;
}

void ComponentMagicalBinding::deactivate()
{
  m_active = false;
}

ElapsedTime ComponentMagicalBinding::autoExpirationTime() const
{
  return m_autoExpirationTime;
}

void ComponentMagicalBinding::setAutoExpirationTime(ElapsedTime time)
{
  m_autoExpirationTime = time;
}

ElapsedTime ComponentMagicalBinding::activeTimeRemaining() const
{
  return m_activeTimeRemaining;
}

void ComponentMagicalBinding::setActiveTime(ElapsedTime time)
{
  m_activeTimeRemaining = time;
}
