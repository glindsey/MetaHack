#include "stdafx.h"

#include "properties/ModifiablePropertyDictionary.h"

ModifiablePropertyDictionary::ModifiablePropertyDictionary()
{
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

size_t ModifiablePropertyDictionary::has_modifier_for(std::string key, EntityId id) const
{
  if (m_modifiers.count(key) != 0)
  {
    auto& ids = m_modifiers.at(key);
    return ids.count(id);
  }
  return 0;
}

bool ModifiablePropertyDictionary::add_modifier(std::string key, EntityId id, ElapsedTime expires_at)
{
  if (m_modifiers.count(key) == 0)
  {
    m_modifiers.emplace(std::make_pair(key, ExpirationMap()));
  }

  if (m_modifiers.at(key).count(id) == 0)
  {
    m_modifiers.at(key).emplace(std::make_pair(id, expires_at));
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

size_t ModifiablePropertyDictionary::remove_modifier(std::string key, EntityId id)
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

void ModifiablePropertyDictionary::remove_expired_modifiers(std::string key, ElapsedTime current_game_time)
{
  if (m_modifiers.count(key) != 0)
  {
    auto& expiration_map = m_modifiers.at(key);
    auto& expiration_iter = expiration_map.begin();
    while (expiration_iter != expiration_map.end())
    {
      if (expiration_iter->second >= current_game_time)
      {
        expiration_iter = expiration_map.erase(expiration_iter);
      }
      else
      {
        ++expiration_iter;
      }
    }
  }


}

void ModifiablePropertyDictionary::add_ticks(std::string key, ElapsedTime ticks)
{
  if (m_modifiers.count(key) != 0)
  {
    auto& expiration_map = m_modifiers.at(key);
    for (auto& expiration_pair : expiration_map)
    {
      expiration_pair.second += ticks;
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