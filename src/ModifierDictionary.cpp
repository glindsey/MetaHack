#include "stdafx.h"

#include "ModifierDictionary.h"

ModifierDictionary::ModifierDictionary()
{
}

ModifierDictionary::~ModifierDictionary()
{
}

unsigned int ModifierDictionary::contains(StringKey key) const
{
  if (m_modifiers.count(key) != 0)
  {
    auto& ids = m_modifiers.at(key);
    return ids.size();
  }
  return 0;
}

unsigned int ModifierDictionary::contains(StringKey key, ThingId id) const
{
  if (m_modifiers.count(key) != 0)
  {
    auto& ids = m_modifiers.at(key);
    return ids.count(id);
  }
  return 0;
}

bool ModifierDictionary::add(StringKey key, ThingId id, unsigned int expiration_ticks)
{
  if (m_modifiers.count(key) == 0)
  {
    m_modifiers.emplace(std::make_pair(key, ExpirationMap()));
  }

  if (m_modifiers.at(key).count(id) == 0)
  {
    m_modifiers.at(key).emplace(std::make_pair(id, expiration_ticks));
    return true;
  }

  return false;
}

unsigned int ModifierDictionary::remove(StringKey key)
{
  if (m_modifiers.count(key) != 0)
  {
    auto size = m_modifiers.at(key).size();
    m_modifiers.erase(key);
    return size;
  }

  return 0;
}

unsigned int ModifierDictionary::remove(StringKey key, ThingId id)
{
  if (m_modifiers.count(key) != 0)
  {
    if (m_modifiers.at(key).count(id) != 0)
    {
      m_modifiers.at(key).erase(id);
      if (m_modifiers.at(key).size() == 0)
      {
        m_modifiers.erase(key);
      }
      return 1;
    }
  }

  return 0;
}

void ModifierDictionary::add_ticks(StringKey key, int ticks)
{
  for (auto& modifier_pair : m_modifiers)
  {
    for (auto& property_pair : modifier_pair.second)
    {
      property_pair.second += ticks;
    }
  }
}