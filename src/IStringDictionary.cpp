#include "stdafx.h"

#include "New.h"
#include "NullStringDictionary.h"

IStringDictionary::~IStringDictionary() {}

IStringDictionary* IStringDictionary::getNull()
{
  return NEW NullStringDictionary();
}