#ifndef MODIFIABLEPROPERTYDICTIONARY_H
#define MODIFIABLEPROPERTYDICTIONARY_H

#include "stdafx.h"

#include "PropertyDictionary.h"

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
      /// @todo WRITE ME
      return get<T>(key);
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
  virtual void _after_set(StringKey key);

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
};

#endif // MODIFIABLEPROPERTYDICTIONARY_H
