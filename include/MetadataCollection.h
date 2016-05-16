#ifndef METADATACOLLECTION_H
#define METADATACOLLECTION_H

#include "stdafx.h"

// Forward declarations
class Metadata;

class MetadataCollection
{
public:
  explicit MetadataCollection(StringKey category);
  ~MetadataCollection();

  StringKey get_category();

  Metadata& get(StringKey type);

protected:

private:
  /// The category of this collection.
  StringKey m_category;

  /// The collection of Metadata instances.
  boost::ptr_unordered_map<StringKey, Metadata> m_collection;
};

#endif // METADATACOLLECTION_H
