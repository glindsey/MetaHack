#include "stdafx.h"

#include "metadata/MetadataCollection.h"

#include "metadata/Metadata.h"

namespace fs = boost::filesystem;

MetadataCollection::MetadataCollection(std::string category)
  :
  m_category{ category }
{
  //preload();
}

MetadataCollection::~MetadataCollection()
{}

std::string MetadataCollection::get_category()
{
  return m_category;
}

void MetadataCollection::preload()
{
  CLOG(TRACE, "Metadata") << "Preloading MetadataCollection for \"" << m_category << "\"";

  // Look for the various files containing this metadata.
  fs::path resource_path{ "resources/" + m_category };

  Assert("Metadata", fs::exists(resource_path),
         resource_path << " does not exist");

  Assert("Metadata", fs::is_directory(resource_path),
         resource_path << " is not a directory");

  for (fs::directory_entry& entry : fs::directory_iterator(resource_path))
  {
    auto stem = entry.path().stem().string();
    auto extension = boost::locale::to_lower(entry.path().extension().string());
    if (extension == std::string(".lua"))
    {
      CLOG(TRACE, "Metadata") << "Found: " << stem;
      (void)get(stem);
    }
  }

  return;
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