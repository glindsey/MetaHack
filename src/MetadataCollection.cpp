#include "stdafx.h"

#include "MetadataCollection.h"

#include "Metadata.h"

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