#include "stdafx.h"

#include "ModifiablePropertyDictionary.h"

#include "ErrorHandler.h"

ModifiablePropertyDictionary::ModifiablePropertyDictionary()
{
  SET_UP_LOGGER("ModifiablePropertyDictionary", true);
}

ModifiablePropertyDictionary::~ModifiablePropertyDictionary()
{}

void ModifiablePropertyDictionary::_after_set(StringKey key)
{
  m_modified_dictionary.erase(key);
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