#include "stdafx.h"

#include "properties/PropertyDictionary.h"

#include "properties/Property.h"

PropertyDictionary::PropertyDictionary()
{
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

/// Get a base entry from the dictionary.
/// @param key  Key of the setting to retrieve.
/// @return     The entry requested.
///             If the entry does not exist, returns a null Property.

Property const & PropertyDictionary::get(std::string key) const
{
  if (m_dictionary.count(key) == 0)
  {
    return Property::null();
  }
  else
  {
    return m_dictionary.at(key);
  }
}

/// Add/alter an entry in the base dictionary.
/// Erases any entry in the modified dictionary, if one exists.
///
/// @note         The type being added must be copyable.
/// @param key    Key of the entry to add/alter.
/// @param value  Value to set it to.
/// @return       True if the entry already existed and has been changed.
///               False if a new entry was added. 

bool PropertyDictionary::set(std::string key, Property const & value)
{
  bool existed = (m_dictionary.count(key) != 0);

  if (existed)
  {
    m_dictionary.erase(key);
  }

  m_dictionary.insert(PropertyPair(key, value));
  after_set_(key);

  return existed;
}

PropertyMap& PropertyDictionary::get_dictionary()
{
  return m_dictionary;
}

void PropertyDictionary::after_set_(std::string key)
{
}

/// @todo Verify that this is correct. I *think* it is but it has not been tested.
bool PropertyDictionary::operator==(PropertyDictionary const& other) const
{
  // If the dictionary counts don't match, return false.
  if (m_dictionary.size() != other.m_dictionary.size())
  {
    return false;
  }

  // Step through each key in our metadictionary.
  for (auto& pair : m_dictionary)
  {
    auto key = pair.first;

    if (other.m_dictionary.count(key) == 0)
    {
      return false;
    }

    auto our_value = pair.second;
    auto other_value = other.m_dictionary.at(key);

    if (our_value != other_value)
    {
      return false;
    }
  }

  return true;
}