#include "stdafx.h"

#include "MetadataCollection.h"

#include "Metadata.h"
#include "New.h"

boost::ptr_unordered_map<std::string, MetadataCollection> MetadataCollection::m_metacollection;

MetadataCollection::MetadataCollection(std::string category)
  :
  m_category{ category }
{}

MetadataCollection::~MetadataCollection()
{}

std::string MetadataCollection::get_category()
{
  return m_category;
}

Metadata& MetadataCollection::get(std::string type)
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

MetadataCollection& MetadataCollection::get_collection(std::string category)
{
  if (m_metacollection.count(category) == 0)
  {
    m_metacollection.insert(category, NEW MetadataCollection(category));
  }

  return m_metacollection.at(category);
}