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

  /// Expiration time, in ticks, for this modifier, or 0 if no expiration.
  unsigned int  expiration_ticks;
};

// Using aliases
using ExpirationMap = std::unordered_map<ThingId, unsigned int>;
using ModifierMap = std::unordered_map<StringKey, ExpirationMap>;

/// Class that handles a dictionary that stores modifiers for specific
/// properties.
class ModifierDictionary final
{
public:
  ModifierDictionary();

  ~ModifierDictionary();

  /// Check if modifier functions exist for a particular key.
  /// @return Number of modifiers for the key.
  unsigned int contains(StringKey key) const;

  /// Check if a modifier function exists for a particular key and ID.
  /// @return Number of modifiers for the key and ID. (Should be 0 or 1.)
  unsigned int contains(StringKey key, ThingId id) const;

  /// Add a modifier function for a given property and ID.
  /// @param  key               Name of property to add a modifier function for.
  /// @param  id                ThingId of the Thing responsible for this function.
  /// @param  expiration_ticks  Number of ticks until modifier expires, or 0 if it
  ///                           never expires. Defaults to 0.
  ///
  /// The resulting function should be named "modify_property_XXX"
  /// where XXX is the name of the property to modify, and it should be
  /// a member of a Thing-derived Lua class. It should take two parameters:
  /// The ID of the Thing being affected, and the property value prior to
  /// modification; and it should return the property value after modification.
  ///
  /// If a modifier function already exists for a given property and ID, no
  /// new one will be added.
  ///
  /// @return True if the function was added; false if it already existed.
  bool add(StringKey key, ThingId id, unsigned int expiration_ticks = 0);

  /// Remove all modifier functions for a given key.
  /// @return The number of modifiers erased.
  unsigned int remove(StringKey key);

  /// Remove all modifier functions for a given key and thing ID.
  /// @return The number of modifiers erased.
  unsigned int remove(StringKey key, ThingId id);

  /// Run all the modifier functions for a property given a value to modify.
  /// @param  key     Property to run the modifiers for.
  /// @param  value   Value to run them on.
  /// @return The resulting value.
  /// @todo WRITE ME
  template<typename T>
  T run(StringKey key, T value)
  {
    return value;
  }

  /// Add/subtract ticks from all modifier expiration_ticks, if applicable.
  /// Upon expiration a modifier will be automatically removed.
  /// @param  ticks   Number of ticks to add/subtract.
  void add_ticks(StringKey key, int ticks);

private:
  ModifierMap m_modifiers;
};

#endif // PROPERTYDICTIONARY_H
