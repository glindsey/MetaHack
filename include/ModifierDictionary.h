#ifndef MODIFIERDICTIONARY_H
#define MODIFIERDICTIONARY_H

#include "stdafx.h"

#include "ErrorHandler.h"
#include "Metadata.h"

// Namespace aliases
namespace pt = boost::property_tree;

/// Struct describing a property modifier.
struct PropertyModifier
{
  /// ID of the Thing responsible for doing the modification.
  ThingId       id;

  /// String indicating the Lua function to be called.
  StringKey     lua_function;
};

/// Class that handles a dictionary that stores modifiers for specific
/// properties.
class ModifierDictionary
{
public:
  ModifierDictionary();

  virtual ~ModifierDictionary();

  /// Check if modifier functions exist for a particular key.
  /// @return Number of modifiers for the key.
  unsigned int contains(StringKey key) const;

  /// Clear all modifier functions for a given key.
  /// @return True if modifiers were cleared, false if no modifiers existed.
  bool clear(StringKey key);

  /// Run all the modifier functions for a key given a value to modify.
  /// @param  key     Key to run the modifiers for.
  /// @param  value   Value to run them on.
  /// @return The resulting value.
  /// @todo WRITE ME
  template<typename T>
  T run(StringKey key, T value)
  {
    return value;
  }

  /// Add a modifier function for a given key.
  /// @param  key     Key to add a modifier function for.
  /// @param  id      ThingId of the Thing responsible for this function.
  /// @param  value   Name of the modifier function to add.
  bool add(StringKey key, ThingId id, StringKey value);

private:
  std::unordered_multimap<StringKey, PropertyModifier> m_modifiers;
};

#endif // PROPERTYDICTIONARY_H
