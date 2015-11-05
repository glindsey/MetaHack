#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include "Lua.h"

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

/// Class that contains configuration settings for the entire game.  The class
/// is instantiated as a lazy-initialized singleton.
class ConfigSettings
{
  public:
    virtual ~ConfigSettings();

    /// Get the singleton instance of the class.
    /// If the instance does not yet exist, it is constructed and initialized.
    static ConfigSettings& instance();

    /// Get a setting from the map.
    /// @param key  Key of the setting to retrieve.
    /// @return     The setting requested.
    ///             If the setting does not exist, returns a new instance of T.
    template<typename T>
    T get(std::string key)
    {
      // Bail if the setting doesn't exist.
      if (settings.count(key) == 0)
      {
        return T();
      }

      boost::any value = settings.at(key);

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
    // These are required because otherwise Lua access of these types will
    // break round-tripping. (i.e. Any time Lua writes a value it will be
    // stored as a double does to Lua's "a number is a number" typing. No way
    // to get around that.)
    template<> int get(std::string key)               { return static_cast<int>(get<double>(key)); }
    template<> unsigned int get(std::string key)      { return static_cast<unsigned int>(get<double>(key)); }
    template<> long int get(std::string key)          { return static_cast<long int>(get<double>(key)); }
    template<> unsigned long int get(std::string key) { return static_cast<unsigned long int>(get<double>(key)); }
    template<> float get(std::string key)             { return static_cast<float>(get<double>(key)); }
    
    /// Set a setting in the map.
    /// If the setting does not exist, it is created.
    /// @note         The type being added must be copyable... I think.
    /// @param key    Key of the setting to set.
    /// @param value  Value to set it to.    
    /// @return       True if the setting already existed and has been changed.
    ///               False if a new setting was added.
    template<typename T>
    bool set(std::string key, T value)
    {
      bool existed = (settings.count(key) != 0);
      boost::any insert_value = value;

      settings.insert(std::pair<std::string, boost::any>(key, insert_value));
      return existed;
    }

    // Various template specializations for certain types.
    // See get() specalizations for explanation.
    template<> bool set(std::string key, int value)               { return set<double>(key, static_cast<double>(value)); }
    template<> bool set(std::string key, unsigned int value)      { return set<double>(key, static_cast<double>(value)); }
    template<> bool set(std::string key, long int value)          { return set<double>(key, static_cast<double>(value)); }
    template<> bool set(std::string key, unsigned long int value) { return set<double>(key, static_cast<double>(value)); }
    template<> bool set(std::string key, float value)             { return set<double>(key, static_cast<double>(value)); }
    template<> bool set(std::string key, char const* value)       { return set<std::string>(key, std::string(value)); }

    // Common non-templated set() functions, used to aid in typing.
    bool set(std::string key, int value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, unsigned int value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, long int value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, unsigned long int value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, float value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, double value) { return set<double>(key, static_cast<double>(value)); }
    bool set(std::string key, bool value) { return set<bool>(key, value); }
    bool set(std::string key, char const* value) { return set<std::string>(key, std::string(value)); }
    bool set(std::string key, std::string value) { return set<std::string>(key, value); }
    bool set(std::string key, sf::Vector2i value) { return set<sf::Vector2i>(key, value); }
    bool set(std::string key, sf::Vector2u value) { return set<sf::Vector2i>(key, static_cast<sf::Vector2i>(value)); }
    bool set(std::string key, sf::Color value)    { return set<sf::Color>(key, value); }

    /// Lua function to register.
    static int ConfigSettings::LUA_get_config(lua_State* L);

  protected:
  private:
    /// Constructor.  Private because this is a singleton.
    ConfigSettings();

    /// Map of settings.
    AnyMap settings;

    /// Unique pointer to singleton instance.
    static std::unique_ptr<ConfigSettings> instance_;
};

/// Shortcut to the singleton instance, to save on typing.
#define Settings ConfigSettings::instance()

#endif // CONFIGSETTINGS_H
