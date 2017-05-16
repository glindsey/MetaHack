#include "stdafx.h"
#include "ModifiableInt.h"

#include "utilities/JSONUtils.h"

ModifiableInt::ModifiableInt()
  : m_baseValue{ 0 }
{
}

ModifiableInt::ModifiableInt(int baseValue)
  : m_baseValue{ baseValue }
{
}

int ModifiableInt::value() const
{
  if (m_preMultipliers.size() == 0 && m_postMultipliers.size() == 0)
  {
    // No float conversions required.
    int value = m_baseValue;
    for (auto& adder : m_adders)
    {
      value += adder.second.value;
    }
    return value;
  }
  else
  {
    float value = static_cast<float>(m_baseValue);
    for (auto& preMultiplier : m_preMultipliers)
    {
      value *= preMultiplier.second.value;
    }
    for (auto& adder : m_adders)
    {
      value += static_cast<float>(adder.second.value);
    }
    for (auto& postMultiplier : m_postMultipliers)
    {
      value *= postMultiplier.second.value;
    }
    return static_cast<int>(value);
  }
}

int ModifiableInt::baseValue() const
{
  return m_baseValue;
}

void ModifiableInt::setBaseValue(int baseValue)
{
  m_baseValue = baseValue;
}

bool ModifiableInt::setPreMultiplierFrom(EntityId id, Modifier<float> modifier)
{
  bool hasPreMultiplier = hasPreMultiplierFrom(id);

  if (hasPreMultiplier)
  {
    m_preMultipliers[id].value *= modifier.value;
    m_preMultipliers[id].expirationTime = modifier.expirationTime;
    m_preMultipliers[id].reason = modifier.reason;
  }
  else
  {
    m_preMultipliers[id] = modifier;
  }

  return hasPreMultiplier;
}

bool ModifiableInt::hasPreMultiplierFrom(EntityId id)
{
  return (m_preMultipliers.count(id) != 0);
}

bool ModifiableInt::clearPreMultiplierFrom(EntityId id)
{
  return (m_preMultipliers.erase(id) != 0);
}

void ModifiableInt::clearAllPreMultipliers()
{
  m_preMultipliers.clear();
}

bool ModifiableInt::setAdderFrom(EntityId id, Modifier<int> modifier)
{
  bool hasAdder = hasAdderFrom(id);

  if (hasAdder)
  {
    m_adders[id].value += modifier.value;
    m_adders[id].expirationTime = modifier.expirationTime;
    m_adders[id].reason = modifier.reason;
  }
  else
  {
    m_adders[id] = modifier;
  }

  return hasAdder;
}

bool ModifiableInt::hasAdderFrom(EntityId id)
{
  return (m_adders.count(id) != 0);
}

bool ModifiableInt::clearAdderFrom(EntityId id)
{
  return (m_adders.erase(id) != 0);
}

void ModifiableInt::clearAllAdders()
{
  m_adders.clear();
}

bool ModifiableInt::setPostMultiplierFrom(EntityId id, Modifier<float> modifier)
{
  bool hasPostMultiplier = hasPostMultiplierFrom(id);

  if (hasPostMultiplier)
  {
    m_postMultipliers[id].value *= modifier.value;
    m_postMultipliers[id].expirationTime = modifier.expirationTime;
    m_postMultipliers[id].reason = modifier.reason;
  }
  else
  {
    m_postMultipliers[id] = modifier;
  }

  return hasPostMultiplier;
}

bool ModifiableInt::hasPostMultiplierFrom(EntityId id)
{
  return (m_postMultipliers.count(id) != 0);
}

bool ModifiableInt::clearPostMultiplierFrom(EntityId id)
{
  return (m_postMultipliers.erase(id) != 0);
}

void ModifiableInt::clearAllPostMultipliers()
{
  m_postMultipliers.clear();
}

void ModifiableInt::clearAllModifiersFrom(EntityId id)
{
  clearPreMultiplierFrom(id);
  clearAdderFrom(id);
  clearPostMultiplierFrom(id);
}

void ModifiableInt::clearAllModifiers()
{
  clearAllPreMultipliers();
  clearAllAdders();
  clearAllPostMultipliers();
}

void from_json(json const & j, ModifiableInt & obj)
{
  if (j.is_object())
  {
    obj.m_baseValue = j["base-value"];
    JSONUtils::doIfPresent(j, "pre-multipliers", [&obj](json const& value)
    {
      if (value.is_object())
      {
        obj.clearAllPreMultipliers();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.setPreMultiplierFrom(citer.key(), citer.value());
        }
      }
    });
    JSONUtils::doIfPresent(j, "adders", [&obj](json const& value)
    {
      if (value.is_object())
      {
        obj.clearAllAdders();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.setAdderFrom(citer.key(), citer.value());
        }
      }
    });
    JSONUtils::doIfPresent(j, "post-multipliers", [&obj](json const& value)
    {
      if (value.is_object())
      {
        obj.clearAllPostMultipliers();
        for (auto citer = value.cbegin(); citer != value.cend(); ++citer)
        {
          obj.setPostMultiplierFrom(citer.key(), citer.value());
        }
      }
    });
  }
  else
  {
    // Assume j is just the base value with no modifiers.
    obj.clearAllModifiers();
    obj.m_baseValue = j;
  }
}

void to_json(json & j, ModifiableInt const & obj)
{
  if (obj.m_preMultipliers.size() == 0 && 
      obj.m_adders.size() == 0 && 
      obj.m_postMultipliers.size() == 0)
  {
    j = obj.m_baseValue;
  }
  else
  {
    j = json::object();
    j["base-value"] = obj.m_baseValue;
    j["pre-multipliers"] = json::object();
    auto& preMultipliersJSON = j["pre-multipliers"];
    for (auto& preMultiplier : obj.m_preMultipliers)
    {
      preMultipliersJSON[std::to_string(preMultiplier.first)] = preMultiplier.second;
    }
    j["adders"] = json::object();
    auto& addersJSON = j["adders"];
    for (auto& adder : obj.m_adders)
    {
      addersJSON[std::to_string(adder.first)] = adder.second;
    }
    j["post-multipliers"] = json::object();
    auto& postMultipliersJSON = j["post-multipliers"];
    for (auto& postMultiplier : obj.m_postMultipliers)
    {
      postMultipliersJSON[std::to_string(postMultiplier.first)] = postMultiplier.second;
    }
  }
}
