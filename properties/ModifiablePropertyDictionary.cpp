#include "stdafx.h"

#include "game/App.h"
#include "properties/ModifiablePropertyDictionary.h"

ModifiablePropertyDictionary::ModifiablePropertyDictionary(EntityId owner)
  : 
  PropertyDictionary(owner)
{
  m_modified_dictionary = json::object();
}

ModifiablePropertyDictionary::~ModifiablePropertyDictionary()
{}


/// Get a modified entry from the dictionary.
/// @param name Name of the property to retrieve.
/// @return     The entry requested.
///             If the entry does not exist, it is recalculated via the
///             property modifiers associated with it.

json ModifiablePropertyDictionary::get_modified(std::string name)
{
  // Recalculate if the setting doesn't exist.
  if (m_modified_dictionary.count(name) == 0)
  {
    run_modifiers_for(name);
  }

  return m_modified_dictionary[name];
}

void ModifiablePropertyDictionary::after_set_(std::string name)
{
  m_modified_dictionary.erase(name);
}

size_t ModifiablePropertyDictionary::has_modifier_for(std::string name) const
{
  if (m_modifiers.count(name) != 0)
  {
    auto& ids = m_modifiers.at(name);
    return ids.size();
  }
  return 0;
}

size_t ModifiablePropertyDictionary::has_modifier_for(std::string name, EntityId id) const
{
  if (m_modifiers.count(name) != 0)
  {
    auto& ids = m_modifiers.at(name);
    return ids.count(id);
  }
  return 0;
}

bool ModifiablePropertyDictionary::addModifier(std::string name, 
                                                EntityId id, 
                                                PropertyModifierInfo const & info)
{
  if (m_modifiers.count(name) == 0)
  {
    m_modifiers.emplace(std::make_pair(name, ModifierEntityMap()));
  }

  if (m_modifiers.at(name).count(id) == 0)
  {
    m_modifiers.at(name).emplace(std::make_pair(id, info));
    m_modified_dictionary.erase(name);
    return true;
  }

  return false;
}

size_t ModifiablePropertyDictionary::remove_all_modifiers(std::string name)
{
  if (m_modifiers.count(name) != 0)
  {
    auto size = m_modifiers.at(name).size();
    m_modifiers.erase(name);
    m_modified_dictionary.erase(name);
    return size;
  }

  return 0;
}

size_t ModifiablePropertyDictionary::removeModifier(std::string name, EntityId id)
{
  if (m_modifiers.count(name) != 0)
  {
    if (m_modifiers.at(name).count(id) != 0)
    {
      m_modifiers.at(name).erase(id);
      if (m_modifiers.at(name).size() == 0)
      {
        m_modifiers.erase(name);
      }
      m_modified_dictionary.erase(name);
      return 1;
    }
  }

  return 0;
}

void ModifiablePropertyDictionary::remove_expired_modifiers(std::string name, 
                                                            ElapsedTime current_game_time)
{
  if (m_modifiers.count(name) != 0)
  {
    bool modifier_removed = false;
    auto& expiration_map = m_modifiers.at(name);
    auto& expiration_iter = expiration_map.begin();
    while (expiration_iter != expiration_map.end())
    {
      if (expiration_iter->second.expires_at >= current_game_time)
      {
        expiration_iter = expiration_map.erase(expiration_iter);
        modifier_removed = true;
      }
      else
      {
        ++expiration_iter;
      }
    }

    if (modifier_removed)
    {
      m_modified_dictionary.erase(name);
    }
  }


}

void ModifiablePropertyDictionary::run_modifiers_for(std::string name)
{
  if (m_modified_dictionary.count(name) != 0)
  {
    m_modified_dictionary.erase(name);
  }

  auto unmodified_value = get(name);
  auto modified_value = unmodified_value;
  /// @todo Call the appropriate modifier functions.
  auto& modifiers = m_modifiers[name];

  for (auto& modifier : modifiers)
  {
    auto responsible_id = modifier.first;
    auto& responsible_group = modifier.second.responsible_group;
    auto& suffix = modifier.second.suffix;
    modified_value = the_lua_instance.call_modifier_function(name, 
                                                             unmodified_value,
                                                             get_owner(),
                                                             responsible_group,
                                                             responsible_id,
                                                             suffix);
  }

  m_modified_dictionary.emplace(std::make_pair(name, modified_value));
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