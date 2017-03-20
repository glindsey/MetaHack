#ifndef METADATA_H
#define METADATA_H

#include "stdafx.h"

// Forward declarations
class MetadataCollection;
class Property;

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Metadata
{
  friend class MetadataCollection;

public:
  Metadata(MetadataCollection& collection, std::string type);
  virtual ~Metadata();

  MetadataCollection& get_metadata_collection();

  std::string const& get_type() const;

  UintVec2 get_tile_coords() const;

  Property get_intrinsic(std::string name, Property default_value) const;
  Property get_intrinsic(std::string name) const;
  void set_intrinsic(std::string name, Property value);
  
protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "entity", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;
};

#endif // METADATA_H
