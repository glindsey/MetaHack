#pragma once

#include "services/IConfigSettings.h"

/// Hardcoded fallback config settings for the game.
class FallbackConfigSettings : public IConfigSettings
{
public:
  FallbackConfigSettings();
  virtual ~FallbackConfigSettings();
};
