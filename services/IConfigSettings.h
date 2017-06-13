#pragma once



#include "properties/PropertyDictionary.h"

/// Service that contains configuration settings for the entire game.
class IConfigSettings : public PropertyDictionary
{
public:
  virtual ~IConfigSettings();

  /// Return a Null Object that implements this interface.
  static IConfigSettings* getDefault();

};
