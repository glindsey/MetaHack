#pragma once

#include "stdafx.h"

#include "types/Direction.h"
#include "ErrorHandler.h"
#include "metadata/Metadata.h"
#include "properties/Property.h"

/// Class that handles a dictionary that associates data of various types
/// with keys.
class PropertyDictionary
{
public:
  PropertyDictionary();

  virtual ~PropertyDictionary();

  /// Clears the entire dictionary out.
  void clear();

  /// Check if a particular key exists.
  bool contains(std::string key) const;

  /// Get a base entry from the dictionary.
  /// @param key  Key of the setting to retrieve.
  /// @return     The entry requested.
  ///             If the entry does not exist, returns a null Property.
  Property const& get(std::string key) const;

  /// Add/alter an entry in the base dictionary.
  /// Erases any entry in the modified dictionary, if one exists.
  ///
  /// @note         The type being added must be copyable.
  /// @param key    Key of the entry to add/alter.
  /// @param value  Value to set it to.
  /// @return       True if the entry already existed and has been changed.
  ///               False if a new entry was added. 
  bool set(std::string key, Property const& value);

  template <typename T> bool set(std::string key, T value) 
  { 
    Property property = Property::from(value);
    return set(key, property); 
  }

  /// Overridable function to be called after a set() is performed.
  /// Default behavior is to do nothing.
  /// @param key  Key that was set.
  virtual void after_set_(std::string key);

  /// Overloaded equality operator.
  bool operator==(PropertyDictionary const& other) const;

  /// Overloaded inequality operator.
  bool operator!=(PropertyDictionary const& other) const
  {
    return !(*this == other);
  }

protected:
  /// Get a reference to the dictionary map itself.
  PropertyMap& get_dictionary();

private:
  /// The base property dictionary.
  PropertyMap m_dictionary;
};
