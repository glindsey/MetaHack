#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include "stdafx.h"

#include "LuaObject.h"
#include "PropertyDictionary.h"

/// Class that contains configuration settings for the entire game.  The class
/// is instantiated as a lazy-initialized singleton.
class ConfigSettings : public PropertyDictionary
{
public:
  /// Constructor.  Private because this is a singleton.
  ConfigSettings();

  virtual ~ConfigSettings();
};

#endif // CONFIGSETTINGS_H
