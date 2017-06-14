#include "stdafx.h"

#include "utilities/New.h"
#include "services/NullStrings.h"

IStrings::~IStrings() {}

IStrings* IStrings::getDefault()
{
  return NEW NullStrings();
}
