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
#include "EntityId.h"
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

  template <typename ReturnType>
  ReturnType get_intrinsic(std::string name, ReturnType default_value = ReturnType())
  {
    std::string type = this->get_type();
    return the_lua_instance.get_type_intrinsic<ReturnType>(type, name, default_value);
  }

  template <typename ReturnType>
  ReturnType get_intrinsic(std::string name, ReturnType default_value = ReturnType()) const
  {
    std::string type = this->get_type();
    return the_lua_instance.get_type_intrinsic<ReturnType>(type, name, default_value);
  }

  template <typename ValueType>
  void set_intrinsic(std::string name, ValueType value)
  {
    std::string type = this->get_type();
    the_lua_instance.set_type_intrinsic<ValueType>(type, name, value);
  }

protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "entity", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;
};

#endif // METADATA_H
