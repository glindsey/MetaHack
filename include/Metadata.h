#ifndef METADATA_H
#define METADATA_H

#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <SFML/Graphics.hpp>

#include "common_functions.h"
#include "common_types.h"

#include "ActionResult.h"
#include "App.h"
#include "ErrorHandler.h"
#include "Exceptions.h"
#include "IntegerRange.h"
#include "Lua.h"
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
  Metadata(MetadataCollection& collection, std::string type);
  virtual ~Metadata();

  MetadataCollection& get_collection();

  std::string const& get_type() const;

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
  ActionResult call_lua_function(std::string function_name,
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
  T get_intrinsic(std::string name, T default_value = T())
  {
    return static_cast<T>(get_intrinsic_value(name, default_value));
  }

  template<> bool get_intrinsic(std::string name, bool default_value)
  { 
    return get_intrinsic_bool(name, default_value); 
  }

  template<> std::string get_intrinsic(std::string name, std::string default_value)
  { 
    return get_intrinsic_string(name, default_value); 
  }
  
  bool get_intrinsic_bool(std::string name, bool default_value);
  double get_intrinsic_value(std::string name, double default_value);
  std::string get_intrinsic_string(std::string name, std::string default_value);

  template <typename T>
  T get_default(std::string name, T default_value = T())
  {
    return static_cast<T>(get_default_value(name, default_value));
  }

  template<> bool get_default(std::string name, bool default_value)
  {
    return get_default_bool(name, default_value);
  }

  template<> std::string get_default(std::string name, std::string default_value)
  {
    return get_default_string(name, default_value);
  }

  template<> IntegerRange get_default(std::string name, IntegerRange default_value)
  {
    return get_default_range(name, default_value);
  }

  bool get_default_bool(std::string name, bool default_value);
  double get_default_value(std::string name, double default_value);
  std::string get_default_string(std::string name, std::string default_value);
  IntegerRange get_default_range(std::string name, IntegerRange default_value);

  template <typename T>
  void set_intrinsic(std::string name, T value)
  {
    set_intrinsic_value(name, static_cast<double>(value));
  }

  template<> void set_intrinsic(std::string name, bool value)
  {
    return set_intrinsic_bool(name, value);
  }

  template<> void set_intrinsic(std::string name, std::string value)
  {
    return set_intrinsic_string(name, value);
  }

  void set_intrinsic_bool(std::string name, bool value);
  void set_intrinsic_value(std::string name, double value);
  void set_intrinsic_string(std::string name, std::string value);

protected:

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "thing", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;
};

#endif // METADATA_H

