#ifndef METADATA_H
#define METADATA_H

#include "stdafx.h"

#include "actions/ActionResult.h"
#include "App.h"
#include "EntityId.h"
#include "ErrorHandler.h"
#include "IntegerRange.h"
#include "LuaObject.h"
#include "MetadataCollection.h"
#include "Property.h"
#include "Tilesheet.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Metadata
{
  friend class MetadataCollection;

public:
  Metadata(MetadataCollection& collection, std::string type);
  virtual ~Metadata();

  MetadataCollection& get_metadata_collection();

  std::string const& get_type() const;

  Vec2u get_tile_coords() const;

  Property get_intrinsic(std::string name, Property::Type type, Property default_value) const;
  Property get_intrinsic(std::string name, Property::Type type) const;
  void set_intrinsic(std::string name, Property value);
  
protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "entity", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;
};

#endif // METADATA_H
