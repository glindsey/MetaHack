#ifndef METADATACOLLECTION_H
#define METADATACOLLECTION_H

#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <string>

// Forward declarations
class Metadata;

class MetadataCollection
{
public:
  MetadataCollection(std::string category);
  ~MetadataCollection();

  std::string MetadataCollection::get_category();

  Metadata& MetadataCollection::get(std::string type);

  static MetadataCollection& MetadataCollection::get_collection(std::string category);

protected:

private:
  /// The category of this collection.
  std::string m_category;

  /// The collection of Metadata instances.
  boost::ptr_unordered_map<std::string, Metadata> m_collection;

  /// The static collection of collections -- a metacollection!
  static boost::ptr_unordered_map<std::string, MetadataCollection> m_metacollection;
};

/// Shortcut for typing MetadataCollection
#define MDC MetadataCollection

#endif // METADATACOLLECTION_H
