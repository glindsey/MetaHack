#include "stdafx.h"

#include "utilities/New.h"
#include "services/FallbackConfigSettings.h"

IConfigSettings::~IConfigSettings() {}

IConfigSettings* IConfigSettings::getNull()
{
  return NEW FallbackConfigSettings();
}
