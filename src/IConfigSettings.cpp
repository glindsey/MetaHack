#include "stdafx.h"

#include "New.h"
#include "FallbackConfigSettings.h"

IConfigSettings::~IConfigSettings() {}

IConfigSettings* IConfigSettings::getNull()
{
  return NEW FallbackConfigSettings();
}
