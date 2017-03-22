#ifndef MODIFIABLEPROPERTYDICTIONARY_H
#define MODIFIABLEPROPERTYDICTIONARY_H

#include "stdafx.h"

#include "properties/PropertyDictionary.h"

// Forward declarations
class EntityId;

/// Struct with property modifier details.
struct PropertyModifierInfo
{
  PropertyModifierInfo(std::string responsible_group_,                       
                       ElapsedTime expires_at_, 
                       std::string reason_,
                       std::string suffix_)
    :
    responsible_group{ responsible_group_ },
    expires_at{ expires_at_ },
    reason{ reason_ },
    suffix{ suffix_ }
  {}

  /// The Lua class responsible for this modifier.
  std::string responsible_group;

  /// Game time that this modifier expires at, or 0 if no expiration.
  ElapsedTime expires_at;

  /// Optional string key to use as a "reason" description.
  std::string reason;

  /// Optional suffix to tack onto the name of the modifier function.
  std::string suffix;
};

// Using aliases
using ModifierEntityMap = std::unordered_map<EntityId, PropertyModifierInfo>;
using ModifierMap = std::unordered_map<std::string, ModifierEntityMap>;

using ModifierEntityPair = std::pair<EntityId, PropertyModifierInfo>;
using ModifierInfo = std::pair<std::string, ModifierEntityMap>;

/// Class that extends PropertyDictionary with the ability to have "modifier"
/// functions that temporarily modify base properties.
class ModifiablePropertyDictionary
  :
  public PropertyDictionary
{
public:
  ModifiablePropertyDictionary() = default;
  ModifiablePropertyDictionary(EntityId owner);

  virtual ~ModifiablePropertyDictionary();

  /// Get a modified entry from the dictionary.
  /// @param name Name of the property to retrieve.
  /// @return     The entry requested.
  ///             If the entry does not exist, it is recalculated via the
  ///             property modifiers associated with it.
  Property get_modified(std::string name);

  /// Overridden function to be called after a set() is performed.
  /// Erases the equivalent key in the modified dictionary.
  /// @param name  Name of property that was set.
  virtual void after_set_(std::string name);

  /// Check if modifier functions exist for a particular property.
  /// @param name  Name of property to check.
  /// @return Number of modifiers for the property.
  size_t has_modifier_for(std::string name) const;

  /// Check if a modifier function exists for a particular key and ID.
  /// @param name Name of property to check.
  /// @param id   ID of the Entity responsible for the modifier.
  /// @return Number of modifiers for the key and ID. (Should be 0 or 1.)
  size_t has_modifier_for(std::string name, EntityId id) const;

  /// Add a modifier function for a given property and ID.
  /// @param  name        Name of property to add a modifier function for.
  /// @param  id          EntityId of the Entity responsible for this function.
  /// @param  info        Info regarding this modifier, in a PropertyModifierInfo struct.
  ///
  /// @see LuaObject::call_modifier_function
  ///
  /// If a modifier function already exists for a given property and ID, no
  /// new one will be added.
  ///
  /// @return True if the function was added; false if it already existed.
  bool add_modifier(std::string name, 
                    EntityId id, 
                    PropertyModifierInfo const& info);

  /// Remove all modifier functions for a given property.
  /// @param name   Name of the property to remove modifiers from.
  /// @return The number of modifiers erased.
  size_t remove_all_modifiers(std::string name);

  /// Remove all modifier functions for a given key and entity ID.
  /// @param name   Name of the property to remove modifiers from.
  /// @param id     Entity responsible for the modifier.
  /// @return The number of modifiers erased.
  size_t remove_modifier(std::string name, EntityId id);

  /// Given a current game time, remove all expired modifiers for a key.
  /// @param name               Name of the property to remove modifiers from.
  /// @param current_game_time  The elapsed game time, in ticks.
  void remove_expired_modifiers(std::string name, ElapsedTime current_game_time);

  /// Run all the modifier functions for a property given a value to modify.
  /// @param  name  Property to run the modifiers for.
  /// @return The resulting value.
  void run_modifiers_for(std::string name);

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
