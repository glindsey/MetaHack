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

  template <typename T>
  T get_intrinsic(StringKey name, T default_value = T())
  {
    return static_cast<T>(get_intrinsic_value(name, default_value));
  }

  template<> bool get_intrinsic(StringKey name, bool default_value)
  {
    return get_intrinsic_bool(name, default_value);
  }

  template<> std::string get_intrinsic(StringKey name, std::string default_value)
  {
    return get_intrinsic_string(name, default_value);
  }

  template<> IntegerRange get_intrinsic(StringKey name, IntegerRange default_value)
  {
    return get_intrinsic_range(name, default_value);
  }

  bool get_intrinsic_bool(StringKey name, bool default_value);
  double get_intrinsic_value(StringKey name, double default_value);
  std::string get_intrinsic_string(StringKey name, std::string default_value);
  IntegerRange get_intrinsic_range(StringKey name, IntegerRange default_value);

  template <typename T>
  void set_intrinsic(StringKey name, T value)
  {
    set_intrinsic_value(name, static_cast<double>(value));
  }

  template<> void set_intrinsic(StringKey name, bool value)
  {
    return set_intrinsic_bool(name, value);
  }

  template<> void set_intrinsic(StringKey name, std::string value)
  {
    return set_intrinsic_string(name, value);
  }

  void set_intrinsic_bool(StringKey name, bool value);
  void set_intrinsic_value(StringKey name, double value);
  void set_intrinsic_string(StringKey name, std::string value);

protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "thing", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  StringKey m_type;
};

#endif // METADATA_H
