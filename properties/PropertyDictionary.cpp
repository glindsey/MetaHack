#include "stdafx.h"

#include "entity/EntityId.h"
#include "properties/PropertyDictionary.h"

PropertyDictionary::PropertyDictionary(EntityId owner)
  :
  m_owner{ owner }
{
  m_dictionary = json::object();
}

PropertyDictionary::~PropertyDictionary()
{}

void PropertyDictionary::clear()
{
  m_dictionary.clear();
}

bool PropertyDictionary::contains(std::string key) const
{
  return (m_dictionary.count(key) != 0);
}

json const& PropertyDictionary::get(std::string key, json const& default_value) const
{
  if (m_dictionary.count(key) == 0)
  {
    return default_value;
  }
  else
  {
    return m_dictionary[key];
  }
}

/// Get a base entry from the dictionary.
/// @param key  Key of the setting to retrieve.
/// @return     The entry requested.
///             If the entry does not exist, returns a null object.

json const & PropertyDictionary::get(std::string key) const
{
  return get(key, json());
}

/// Add/alter an entry in the base dictionary.
/// Erases any entry in the modified dictionary, if one exists.
///
/// @note         The type being added must be copyable.
/// @param key    Key of the entry to add/alter.
/// @param value  Value to set it to.
/// @return       True if the entry already existed and has been changed.
///               False if a new entry was added. 

bool PropertyDictionary::set(std::string key, json const& value)
{
  bool existed = (m_dictionary.count(key) != 0);

  if (existed)
  {
    m_dictionary.erase(key);
  }

  m_dictionary[key] = value;
  after_set_(key);

  return existed;
}

json& PropertyDictionary::get_dictionary()
{
  return m_dictionary;
}

void PropertyDictionary::after_set_(std::string key)
{
}

EntityId PropertyDictionary::get_owner()
{
  return m_owner;
}

/// @todo Verify that this is correct. I *think* it is but it has not been tested.
bool PropertyDictionary::operator==(PropertyDictionary const& other) const
{
  return m_dictionary == other.m_dictionary;
}