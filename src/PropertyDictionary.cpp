#include "stdafx.h"

#include "PropertyDictionary.h"

#include "ErrorHandler.h"
#include "IntegerRange.h"

PropertyDictionary::PropertyDictionary()
{
  SET_UP_LOGGER("PropertyDictionary", true);
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

AnyMap& PropertyDictionary::get_dictionary()
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

    auto& our_type = our_value.type();
    auto& other_type = other_value.type();

    if (our_type != other_type)
    {
      return false;
    }

    if (our_value != other_value)
    {
      return false;
    }
  }

  return true;
}