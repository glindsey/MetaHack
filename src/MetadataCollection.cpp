#include "stdafx.h"

#include "MetadataCollection.h"

#include "Metadata.h"
#include "New.h"

boost::ptr_unordered_map<StringKey, MetadataCollection> MetadataCollection::m_metacollection;

MetadataCollection::MetadataCollection(StringKey category)
  :
  m_category{ category }
{}

MetadataCollection::~MetadataCollection()
{}

StringKey MetadataCollection::get_category()
{
  return m_category;
}

Metadata& MetadataCollection::get(StringKey type)
{
  if (type.empty())
  {
    type = "Unknown";
  }

  if (m_collection.count(type) == 0)
  {
    m_collection.insert(type, NEW Metadata(*this, type));
  }

  return m_collection.at(type);
}

MetadataCollection& MetadataCollection::get_collection(StringKey category)
{
  if (m_metacollection.count(category) == 0)
  {
    m_metacollection.insert(category, NEW MetadataCollection(category));
  }

  return m_metacollection.at(category);
}