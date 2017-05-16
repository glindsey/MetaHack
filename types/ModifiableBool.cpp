#include "stdafx.h"
#include "ModifiableBool.h"

#include "utilities/JSONUtils.h"


ModifiableBool::ModifiableBool()
  : m_baseValue{ 0 }
{
}

ModifiableBool::ModifiableBool(bool baseValue)
  : m_baseValue{ baseValue }
{
}

bool ModifiableBool::value() const
{
  auto falseOverrides = (m_falseOverrides.size() != 0);
  auto trueOverrides = (m_trueOverrides.size() != 0);

  return (m_baseValue && !falseOverrides) || (trueOverrides && (!falseOverrides || m_trueComesFirst));
}

bool ModifiableBool::baseValue() const
{
  return m_baseValue;
}

void ModifiableBool::setBaseValue(bool baseValue)
{
  m_baseValue = baseValue;
}

bool ModifiableBool::setFalseOverrideFrom(EntityId id, Modifier<bool> modifier)
{
  bool hasOverride = hasFalseOverrideFrom(id);
  m_falseOverrides[id] = modifier;
  return hasOverride;
}

bool ModifiableBool::hasFalseOverrideFrom(EntityId id)
{
  return (m_falseOverrides.count(id) != 0);
}

bool ModifiableBool::clearFalseOverrideFrom(EntityId id)
{
  return (m_falseOverrides.erase(id) != 0);
}

void ModifiableBool::clearAllFalseOverrides()
{
  m_falseOverrides.clear();
}

bool ModifiableBool::setTrueOverrideFrom(EntityId id, Modifier<bool> modifier)
{
  bool hasOverride = hasTrueOverrideFrom(id);
  m_trueOverrides[id] = modifier;
  return hasOverride;
}

bool ModifiableBool::hasTrueOverrideFrom(EntityId id)
{
  return (m_trueOverrides.count(id) != 0);
}

bool ModifiableBool::clearTrueOverrideFrom(EntityId id)
{
  return (m_trueOverrides.erase(id) != 0);
}

void ModifiableBool::clearAllTrueOverrides()
{
  m_trueOverrides.clear();
}

bool ModifiableBool::trueComesFirst() const
{
  return m_trueComesFirst;
}

void ModifiableBool::setTrueComesFirst(bool trueComesFirst)
{
  trueComesFirst = m_trueComesFirst;
}

void ModifiableBool::clearAllOverridesFrom(EntityId id)
{
  clearFalseOverrideFrom(id);
  clearTrueOverrideFrom(id);
}

void ModifiableBool::clearAllOverrides()
{
  clearAllFalseOverrides();
  clearAllFalseOverrides();
}

void from_json(json const & j, ModifiableBool & obj)
{
  if (j.is_object())
  {
    obj.m_baseValue = j["base-value"];
    JSONUtils::doIfPresent(j, "false-overrides", [&obj](json const& value)
    {
      if (value.is_object())
      {
        obj.clearAllFalseOverrides();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.setFalseOverrideFrom(citer.key(), citer.value());
        }
      }
    });
    JSONUtils::doIfPresent(j, "true-overrides", [&obj](json const& value)
    {
      if (value.is_object())
      {
        obj.clearAllTrueOverrides();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.setTrueOverrideFrom(citer.key(), citer.value());
        }
      }
    });
    JSONUtils::doIfPresent(j, "true-comes-first", [&obj](json const& value) { obj.m_trueComesFirst = value; });
  }
  else
  {
    // Assume j is just the base value with no modifiers.
    obj.clearAllOverrides();
    obj.m_baseValue = j;
  }
}

void to_json(json & j, ModifiableBool const & obj)
{
  if (obj.m_falseOverrides.size() == 0 &&
      obj.m_trueOverrides.size() == 0)
  {
    j = obj.m_baseValue;
  }
  else
  {
    j = json::object();
    j["base-value"] = obj.m_baseValue;
    j["false-overrides"] = json::object();
    auto& falseOverridesJSON = j["false-overrides"];
    for (auto& falseOverride : obj.m_falseOverrides)
    {
      falseOverridesJSON[std::to_string(falseOverride.first)] = falseOverride.second;
    }
    j["true-overrides"] = json::object();
    auto& trueOverridesJSON = j["true-overrides"];
    for (auto& trueOverride : obj.m_trueOverrides)
    {
      trueOverridesJSON[std::to_string(trueOverride.first)] = trueOverride.second;
    }
    j["true-comes-first"] = obj.m_trueComesFirst;
  }
}
