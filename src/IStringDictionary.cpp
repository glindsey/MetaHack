#include "stdafx.h"

#include "New.h"
#include "NullStringDictionary.h"

IStringDictionary* IStringDictionary::getNull()
{
  return NEW NullStringDictionary();
}
