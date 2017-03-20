#ifndef METADATACOLLECTION_H
#define METADATACOLLECTION_H

#include "stdafx.h"

// Forward declarations
class Metadata;

class MetadataCollection
{
public:
  explicit MetadataCollection(std::string category);
  ~MetadataCollection();

  std::string get_category();

  Metadata& get(std::string type);

protected:

private:
  /// The category of this collection.
  std::string m_category;

  /// The collection of Metadata instances.
  boost::ptr_unordered_map<std::string, Metadata> m_collection;
};

#endif // METADATACOLLECTION_H
