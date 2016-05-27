#ifndef PROPERTYDICTIONARY_H
#define PROPERTYDICTIONARY_H

#include "stdafx.h"

#include "ErrorHandler.h"
#include "Metadata.h"

// Namespace aliases
namespace pt = boost::property_tree;

/// Class that handles a dictionary that associates data of various types
/// with keys.
class PropertyDictionary
{
public:
  PropertyDictionary();

  virtual ~PropertyDictionary();

  /// Check if a particular key exists.
  bool contains(StringKey key) const;

  /// Get a base entry from the dictionary.
  /// @param key  Key of the setting to retrieve.
  /// @return     The entry requested.
  ///             If the entry does not exist, returns a new instance of T.
  template<typename T>
  T get(StringKey key) const
  {
    boost::any value = get<boost::any>(key);

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

  /// Template specialization for boost::any.
  template<>
  boost::any get(StringKey key) const
  {
    // Bail if the setting doesn't exist.
    if (m_dictionary.count(key) == 0)
    {
      CLOG(WARNING, "PropertyDictionary") <<
        "Attempted to retrieve nonexistent key \"" << key << "\"";
      return boost::any();
    }

    // Return the requested value.
    return m_dictionary.at(key);
  }

  // Various template specializations for certain types.
  // These are required because otherwise Lua access of these types will
  // break round-tripping. (i.e. Any time Lua writes a value it will be
  // stored as a double does to Lua's "a number is a number" typing. No way
  // to get around that.)
  template<> int get(StringKey key) const { return static_cast<int>(get<double>(key)); }
  template<> unsigned int get(StringKey key) const { return static_cast<unsigned int>(get<double>(key)); }
  template<> long int get(StringKey key) const { return static_cast<long int>(get<double>(key)); }
  template<> unsigned long int get(StringKey key) const { return static_cast<unsigned long int>(get<double>(key)); }
  template<> float get(StringKey key) const { return static_cast<float>(get<double>(key)); }

  /// Add/alter an entry in the base dictionary.
  /// Erases any entry in the modified dictionary, if one exists.
  ///
  /// @note         The type being added must be copyable... I think.
  /// @param key    Key of the entry to add/alter.
  /// @param value  Value to set it to.
  /// @return       True if the entry already existed and has been changed.
  ///               False if a new entry was added.
  template<typename T>
  bool set(StringKey key, T value)
  {
    bool existed = (m_dictionary.count(key) != 0);
    boost::any insert_value = value;

    if (existed)
    {
      m_dictionary.erase(key);
    }

    m_dictionary.insert(std::pair<StringKey, boost::any>(key, insert_value));
    after_set_(key);

    return existed;
  }

  // Various template specializations for certain types.
  // See get() specalizations for explanation.
  template<> bool set(StringKey key, int value) { return set<double>(key, static_cast<double>(value)); }
  template<> bool set(StringKey key, unsigned int value) { return set<double>(key, static_cast<double>(value)); }
  template<> bool set(StringKey key, long int value) { return set<double>(key, static_cast<double>(value)); }
  template<> bool set(StringKey key, unsigned long int value) { return set<double>(key, static_cast<double>(value)); }
  template<> bool set(StringKey key, float value) { return set<double>(key, static_cast<double>(value)); }
  template<> bool set(StringKey key, char const* value) { return set<std::string>(key, std::string(value)); }

  /// Overridable function to be called after a set() is performed.
  /// Default behavior is to do nothing.
  /// @param key  Key that was set.
  virtual void after_set_(StringKey key);

  /// Overloaded equality operator.
  bool operator==(PropertyDictionary const& other) const;

  /// Overloaded inequality operator.
  bool operator!=(PropertyDictionary const& other) const
  {
    return !(*this == other);
  }

protected:
  /// Get a reference to the dictionary map itself.
  AnyMap& get_dictionary();

private:
  /// The base property dictionary.
  AnyMap m_dictionary;
};

/// Equality operator overload for boost::any.
/// Only works for certain values, which is unfortunate, but there's no other
/// safe way to handle equality for boost::any.
inline bool operator==(boost::any const& lhs, boost::any const& rhs)
{
  auto& ltype = lhs.type();
  auto& rtype = rhs.type();

  if (ltype != rtype)
  {
    return false;
  }

  if (ltype == typeid(std::string))
  {
    return boost::any_cast<std::string>(lhs) == boost::any_cast<std::string>(rhs);
  }

  if (ltype == typeid(double))
  {
    return boost::any_cast<double>(lhs) == boost::any_cast<double>(rhs);
  }

  if (ltype == typeid(bool))
  {
    return boost::any_cast<bool>(lhs) == boost::any_cast<bool>(rhs);
  }

  if (ltype == typeid(sf::Color))
  {
    return boost::any_cast<sf::Color>(lhs) == boost::any_cast<sf::Color>(rhs);
  }

  if (ltype == typeid(sf::Vector2i))
  {
    return boost::any_cast<sf::Vector2i>(lhs) == boost::any_cast<sf::Vector2i>(rhs);
  }

  if (ltype == typeid(sf::Vector2u))
  {
    return boost::any_cast<sf::Vector2u>(lhs) == boost::any_cast<sf::Vector2u>(rhs);
  }

  throw std::runtime_error("Comparison of boost::any type \"" + std::string(ltype.name()) + "\" is not supported");
}

inline bool operator!=(boost::any const& op1, boost::any const& op2)
{
  return !(op1 == op2);
}

#endif // PROPERTYDICTIONARY_H
