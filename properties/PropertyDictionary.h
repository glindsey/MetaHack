#pragma once

#include "stdafx.h"

#include "json.hpp"
using json = ::nlohmann::json;

#include "entity/EntityId.h"

/// Class that handles a dictionary that associates data of various types
/// with keys.
class PropertyDictionary
{
public:
  PropertyDictionary() = default;
  PropertyDictionary(EntityId owner);

  virtual ~PropertyDictionary();

  /// Clears the entire dictionary out.
  void clear();

  /// Check if a particular key exists.
  bool contains(std::string key) const;
  bool contains(json::json_pointer key) const;

  /// Get a base entry from the dictionary.
  /// @param key            Key of the setting to retrieve.
  /// @param default_value  Default to use if the key doesn't exist.
  /// @return               The entry requested.
  ///                       If the entry does not exist, returns the default provided..
  json const& get(std::string key, json const& default_value) const;

  /// Get a base entry from the dictionary.
  /// @param key  Key of the setting to retrieve.
  /// @return     The entry requested.
  ///             If the entry does not exist, returns a null object.
  json const& get(std::string key) const;

  /// Add/alter an entry in the base dictionary.
  /// Erases any entry in the modified dictionary, if one exists.
  ///
  /// @note         The type being added must be copyable.
  /// @param key    Key of the entry to add/alter.
  /// @param value  Value to set it to.
  /// @return       True if the entry already existed and has been changed.
  ///               False if a new entry was added. 
  bool set(std::string key, json const& value);

  /// Overridable function to be called after a set() is performed.
  /// Default behavior is to do nothing.
  /// @param key  Key that was set.
  virtual void after_set_(std::string key);

  /// Get the owner of this dictionary, if any.
  EntityId get_owner();

  /// Overloaded equality operator.
  bool operator==(PropertyDictionary const& other) const;

  /// Overloaded inequality operator.
  bool operator!=(PropertyDictionary const& other) const
  {
    return !(*this == other);
  }

protected:
  /// Get a reference to the dictionary map itself.
  json& get_dictionary();

private:
  /// The base property dictionary.
  json m_dictionary;

  /// The owner of this dictionary, if any.
  EntityId m_owner;
};
