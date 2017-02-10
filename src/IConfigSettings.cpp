#include "stdafx.h"

#include "New.h"
#include "FallbackConfigSettings.h"

IConfigSettings* IConfigSettings::getNull()
{
  return NEW FallbackConfigSettings();
}
