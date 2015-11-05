#include "PropertyDictionary.h"

PropertyDictionary::PropertyDictionary()
{}

PropertyDictionary::~PropertyDictionary()
{}

AnyMap& PropertyDictionary::get_dictionary()
{
  return m_dictionary;
}
