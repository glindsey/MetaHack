#ifndef MODIFIABLEPROPERTYDICTIONARY_H
#define MODIFIABLEPROPERTYDICTIONARY_H

#include "stdafx.h"

#include "PropertyDictionary.h"

/// Struct describing a property modifier.
struct PropertyModifier
{
  /// ID of the Entity responsible for doing the modification.
  EntityId id;

  /// Game time that this modifier expires at, or 0 if no expiration.
  ElapsedTime expires_at;
};

// Using aliases
using ExpirationMap = std::unordered_map<EntityId, ElapsedTime>;
using ModifierMap = std::unordered_map<std::string, ExpirationMap>;

/// Class that extends PropertyDictionary with the ability to have "modifier"
/// functions that temporarily modify base properties.
class ModifiablePropertyDictionary
  :
  public PropertyDictionary
{
public:
  ModifiablePropertyDictionary();

  virtual ~ModifiablePropertyDictionary();

  /// Get a modified entry from the dictionary.
  /// @param key  Key of the setting to retrieve.
  /// @return     The entry requested.
  ///             If the entry does not exist, it is recalculated via the
  ///             property modifiers associated with it.
  Property get_modified(std::string key)
  {
    // Recalculate if the setting doesn't exist.
    if (m_modified_dictionary.count(key) == 0)
    {
      run_modifiers_for(key);
    }

    return m_modified_dictionary.at(key);
  }

  /// Overridden function to be called after a set() is performed.
  /// Erases the equivalent key in the modified dictionary.
  /// @param key  Key that was set.
  virtual void after_set_(std::string key);

  /// Check if modifier functions exist for a particular key.
  /// @return Number of modifiers for the key.
  size_t has_modifier_for(std::string key) const;

  /// Check if a modifier function exists for a particular key and ID.
  /// @return Number of modifiers for the key and ID. (Should be 0 or 1.)
  size_t has_modifier_for(std::string key, EntityId id) const;

  /// Add a modifier function for a given property and ID.
  /// @param  key               Name of property to add a modifier function for.
  /// @param  id                EntityId of the Entity responsible for this function.
  /// @param  expiration_ticks  Number of ticks until modifier expires, or 0 if it
  ///                           never expires. Defaults to 0.
  ///
  /// The resulting function should be named "modify_property_XXX"
  /// where XXX is the name of the property to modify, and it should be
  /// a member of a Entity-derived Lua class. It should take two parameters:
  /// The ID of the Entity being affected, and the property value prior to
  /// modification; and it should return the property value after modification.
  ///
  /// If a modifier function already exists for a given property and ID, no
  /// new one will be added.
  ///
  /// @return True if the function was added; false if it already existed.
  bool add_modifier(std::string key, EntityId id, ElapsedTime expires_at = ElapsedTime(0));

  /// Remove all modifier functions for a given key.
  /// @return The number of modifiers erased.
  size_t remove_modifier(std::string key);

  /// Remove all modifier functions for a given key and entity ID.
  /// @return The number of modifiers erased.
  size_t remove_modifier(std::string key, EntityId id);

  /// Given a current game time, remove all expired modifiers for a key.
  void remove_expired_modifiers(std::string key, ElapsedTime current_game_time);

  /// Run all the modifier functions for a property given a value to modify.
  /// @param  key     Property to run the modifiers for.
  /// @return The resulting value.
  void run_modifiers_for(std::string key)
  {
    if (m_modified_dictionary.count(key) != 0)
    {
      m_modified_dictionary.erase(key);
    }

    Property value = get(key);
    /// @todo Call the appropriate modifier functions.
    auto& modifiers = m_modifiers[key];

    for (auto& modifier : modifiers)
    {
      //value = Lua::call_modifier_function<T>(key, id, value);
    }

    m_modified_dictionary.emplace(std::make_pair(key, value));
  }

  /// Add/subtract ticks from all modifier expiration_ticks, if applicable.
  /// Upon expiration a modifier will be automatically removed.
  /// @param  ticks   Number of ticks to add/subtract.
  void add_ticks(std::string key, ElapsedTime ticks);

  /// Overloaded equality operator.
  bool operator==(ModifiablePropertyDictionary const& other) const;

  /// Overloaded inequality operator.
  bool operator!=(ModifiablePropertyDictionary const& other) const
  {
    return !(*this == other);
  }

protected:

private:
  /// The modified property dictionary.
  PropertyMap m_modified_dictionary;

  /// The map of property modifier functions.
  ModifierMap m_modifiers;
};

#endif // MODIFIABLEPROPERTYDICTIONARY_H
