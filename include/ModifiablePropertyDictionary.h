#ifndef MODIFIABLEPROPERTYDICTIONARY_H
#define MODIFIABLEPROPERTYDICTIONARY_H

#include "stdafx.h"

#include "PropertyDictionary.h"

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
  template<typename T>
  T get_modified(StringKey key)
  {
    // Recalculate if the setting doesn't exist.
    if (m_modified_dictionary.count(key) == 0)
    {
      run_modifiers<T>(key);
    }

    boost::any value = m_modified_dictionary.at(key);

    /// Try to cast the setting to the desired type.
    T* p_value = boost::any_cast<T>(&value);

    // Bail if it didn't work.
    if (p_value == nullptr)
    {
      return T();
    }

    // Return the requested value.
    return *(p_value);
  }

  // Various template specializations for certain types.
  // See get() specalizations for explanation.
  template<> int get_modified(StringKey key) { return static_cast<int>(get_modified<double>(key)); }
  template<> unsigned int get_modified(StringKey key) { return static_cast<unsigned int>(get_modified<double>(key)); }
  template<> long int get_modified(StringKey key) { return static_cast<long int>(get_modified<double>(key)); }
  template<> unsigned long int get_modified(StringKey key) { return static_cast<unsigned long int>(get_modified<double>(key)); }
  template<> float get_modified(StringKey key) { return static_cast<float>(get_modified<double>(key)); }

  /// Overridden function to be called after a set() is performed.
  /// Erases the equivalent key in the modified dictionary.
  /// @param key  Key that was set.
  virtual void after_set_(StringKey key);

  /// Check if modifier functions exist for a particular key.
  /// @return Number of modifiers for the key.
  unsigned int has_modifier_for(StringKey key) const;

  /// Check if a modifier function exists for a particular key and ID.
  /// @return Number of modifiers for the key and ID. (Should be 0 or 1.)
  unsigned int has_modifier_for(StringKey key, ThingId id) const;

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
  bool add_modifier(StringKey key, ThingId id, unsigned int expiration_ticks = 0);

  /// Remove all modifier functions for a given key.
  /// @return The number of modifiers erased.
  unsigned int remove_modifier(StringKey key);

  /// Remove all modifier functions for a given key and thing ID.
  /// @return The number of modifiers erased.
  unsigned int remove_modifier(StringKey key, ThingId id);

  /// Run all the modifier functions for a property given a value to modify.
  /// @param  key     Property to run the modifiers for.
  /// @return The resulting value.
  template<typename T>
  void run_modifiers(StringKey key)
  {
    if (m_modified_dictionary.count(key) != 0)
    {
      m_modified_dictionary.erase(key);
    }

    T value = get<T>(key);
    /// @todo Call the appropriate modifier functions.
    m_modified_dictionary.emplace(std::make_pair(key, value));
  }

  /// Add/subtract ticks from all modifier expiration_ticks, if applicable.
  /// Upon expiration a modifier will be automatically removed.
  /// @param  ticks   Number of ticks to add/subtract.
  void add_ticks(StringKey key, int ticks);

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
  AnyMap m_modified_dictionary;

  /// The map of property modifier functions.
  ModifierMap m_modifiers;
};

#endif // MODIFIABLEPROPERTYDICTIONARY_H
