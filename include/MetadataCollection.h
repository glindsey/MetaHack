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

  StringKey MetadataCollection::get_category();

  Metadata& MetadataCollection::get(StringKey type);

  static MetadataCollection& MetadataCollection::get_collection(StringKey category);

protected:

private:
  /// The category of this collection.
  StringKey m_category;

  /// The collection of Metadata instances.
  boost::ptr_unordered_map<StringKey, Metadata> m_collection;

  /// The static collection of collections -- a metacollection!
  static boost::ptr_unordered_map<StringKey, MetadataCollection> m_metacollection;
};

/// Shortcut for typing MetadataCollection
#define MDC MetadataCollection

#endif // METADATACOLLECTION_H
