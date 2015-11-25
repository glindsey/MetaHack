#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include "Lua.h"
#include "PropertyDictionary.h"

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

/// Class that contains configuration settings for the entire game.  The class
/// is instantiated as a lazy-initialized singleton.
class ConfigSettings : public PropertyDictionary
{
public:
  virtual ~ConfigSettings();

  /// Get the singleton instance of the class.
  /// If the instance does not yet exist, it is constructed and initialized.
  static ConfigSettings& instance();

  /// Lua function to register.
  static int ConfigSettings::LUA_get_config(lua_State* L);

protected:
private:
  /// Constructor.  Private because this is a singleton.
  ConfigSettings();

  /// Unique pointer to singleton instance.
  static std::unique_ptr<ConfigSettings> instance_;
};

/// Shortcut to the singleton instance, to save on typing.
#define Settings ConfigSettings::instance()

#endif // CONFIGSETTINGS_H
