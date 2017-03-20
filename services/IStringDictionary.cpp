#include "stdafx.h"

#include "utilities/New.h"
#include "services/NullStringDictionary.h"

IStringDictionary::~IStringDictionary() {}

IStringDictionary* IStringDictionary::getNull()
{
  return NEW NullStringDictionary();
}
