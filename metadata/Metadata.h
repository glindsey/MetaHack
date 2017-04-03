#ifndef METADATA_H
#define METADATA_H

#include "stdafx.h"

#include "json.hpp"
using json = ::nlohmann::json;

// Forward declarations
class MetadataCollection;

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Metadata
{
  friend class MetadataCollection;

public:
  Metadata(MetadataCollection& collection, std::string type);
  virtual ~Metadata();

  MetadataCollection& getMetadataCollection();

  std::string const& getType() const;

  UintVec2 getTileCoords() const;

  json get(std::string name, json default_value) const;
  json get(json::json_pointer name, json default_value) const;


  void set(std::string name, json value);
  void set(json::json_pointer name, json value);

  void add(Metadata const& metadata);
  
protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "entity", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;

  /// Property dictionary containing the metadata.
  json m_metadata;
};

#endif // METADATA_H
