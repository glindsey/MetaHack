#ifndef METADATA_H
#define METADATA_H

#include "stdafx.h"

#include "ActionResult.h"
#include "App.h"
#include "ErrorHandler.h"
#include "IntegerRange.h"
#include "LuaObject.h"
#include "MetadataCollection.h"
#include "PropertyDictionary.h"
#include "ThingRef.h"
#include "Tilesheet.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

class Metadata
{
  friend class MetadataCollection;

public:
  Metadata(MetadataCollection& collection, StringKey type);
  virtual ~Metadata();

  MetadataCollection& get_collection();

  StringKey const& get_type() const;

  sf::Vector2u get_tile_coords();

  template <typename ReturnType>
  ReturnType get_intrinsic(StringKey name, ReturnType default_value = ReturnType())
  {
    StringKey type = this->get_type();
    return the_lua_instance.get_type_intrinsic<ReturnType>(type, name, default_value);
  }

  template <typename ValueType>
  void set_intrinsic(StringKey name, ValueType value)
  {
    StringKey type = this->get_type();
    the_lua_instance.set_type_intrinsic<ValueType>(type, name, value);
  }

protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "thing", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  StringKey m_type;
};

#endif // METADATA_H
