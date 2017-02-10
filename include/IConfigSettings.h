#pragma once

#include "stdafx.h"

#include "PropertyDictionary.h"

/// Service that contains configuration settings for the entire game.
class IConfigSettings : public PropertyDictionary
{
public:
  virtual ~IConfigSettings() {}

  /// Return a Null Object that implements this interface.
  static IConfigSettings* getNull();

};
