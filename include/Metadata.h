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

  /// Try to call a Lua function that takes the caller and a vector of
  /// arguments and returns an ActionResult.
  ///
  /// If the function does not exist, or it returns ActionResult::Pending,
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param args           Vector of arguments to pass to the function
  /// @param default_result The default result if function is not found
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function_actionresult(std::string function_name,
                                              ThingRef caller,
                                              std::vector<lua_Integer> const& args,
                                              ActionResult default_result = ActionResult::Success);

  /// Try to call a Lua function that takes the caller and one argument
  /// and returns a boolean.
  ///
  /// If the function does not exist, or it returns nil, the code attempts
  /// to step up to the parent type and call the function there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param args           Vector of arguments to pass to the function
  /// @param default_result The default result if function is not found
  ///                       (defaults to true).
  /// @return A boolean containing the result of the call.
  bool call_lua_function_bool(std::string function_name,
                              ThingRef caller,
                              std::vector<lua_Integer> const& args,
                              bool default_result = true);

  /// Try to call a Lua function that takes the caller and one argument
  /// and returns a pair of integers.
  ///
  /// If the function does not exist, or it returns nil, the code attempts
  /// to step up to the parent type and call the function there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param args           Vector of arguments to pass to the function
  /// @param default_result The default result if function is not found
  ///                       (defaults to (0, 0)).
  /// @return A boolean containing the result of the call.
  sf::Vector2u call_lua_function_v2u(std::string function_name,
                                     ThingRef caller,
                                     std::vector<lua_Integer> const& args,
                                     sf::Vector2u default_result = sf::Vector2u(0, 0));

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
