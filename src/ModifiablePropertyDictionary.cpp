#include "stdafx.h"

#include "ModifiablePropertyDictionary.h"

#include "ErrorHandler.h"

ModifiablePropertyDictionary::ModifiablePropertyDictionary()
{
  SET_UP_LOGGER("ModifiablePropertyDictionary", true);
}

ModifiablePropertyDictionary::~ModifiablePropertyDictionary()
{}

void ModifiablePropertyDictionary::after_set_(std::string key)
{
  m_modified_dictionary.erase(key);
}

size_t ModifiablePropertyDictionary::has_modifier_for(std::string key) const
{
  if (m_modifiers.count(key) != 0)
  {
    auto& ids = m_modifiers.at(key);
    return ids.size();
  }
  return 0;
}

size_t ModifiablePropertyDictionary::has_modifier_for(std::string key, ThingId id) const
{
  if (m_modifiers.count(key) != 0)
  {
    auto& ids = m_modifiers.at(key);
    return ids.count(id);
  }
  return 0;
}

bool ModifiablePropertyDictionary::add_modifier(std::string key, ThingId id, unsigned int expiration_ticks)
{
  if (m_modifiers.count(key) == 0)
  {
    m_modifiers.emplace(std::make_pair(key, ExpirationMap()));
  }

  if (m_modifiers.at(key).count(id) == 0)
  {
    m_modifiers.at(key).emplace(std::make_pair(id, expiration_ticks));
    m_modified_dictionary.erase(key);
    return true;
  }

  return false;
}

size_t ModifiablePropertyDictionary::remove_modifier(std::string key)
{
  if (m_modifiers.count(key) != 0)
  {
    auto size = m_modifiers.at(key).size();
    m_modifiers.erase(key);
    m_modified_dictionary.erase(key);
    return size;
  }

  return 0;
}

size_t ModifiablePropertyDictionary::remove_modifier(std::string key, ThingId id)
{
  if (m_modifiers.count(key) != 0)
  {
    if (m_modifiers.at(key).count(id) != 0)
    {
      m_modifiers.at(key).erase(id);
      if (m_modifiers.at(key).size() == 0)
      {
        m_modifiers.erase(key);
        m_modified_dictionary.erase(key);
      }
      return 1;
    }
  }

  return 0;
}

void ModifiablePropertyDictionary::add_ticks(std::string key, int ticks)
{
  for (auto& modifier_pair : m_modifiers)
  {
    for (auto& property_pair : modifier_pair.second)
    {
      property_pair.second += ticks;
    }
  }
}

/// @todo Verify that this is correct. I *think* it is but it has not been tested.
bool ModifiablePropertyDictionary::operator==(ModifiablePropertyDictionary const & other) const
{
  if (PropertyDictionary::operator==(other) == true)
  {
    /// @todo Check the modifier function maps.
    return true;
  }

  return false;
}