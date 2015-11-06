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

  std::string const& get_parent() const;

  std::string const& get_display_name() const;

  std::string const& get_display_plural() const;

  std::string const& get_description() const;

  sf::Vector2u const& get_tile_coords() const;

  template<typename T>
  T get_intrinsic(std::string key, T default_value = T())
  {
    PropertyDictionary const& intrinsics = get_intrinsics();
    if (intrinsics.contains(key))
    {
      return get_intrinsics().get<T>(key);
    }
    else
    {
      std::string parent = get_parent();
      if (!parent.empty())
      {
        Metadata& parent_metadata = get_collection().get(parent);
        return parent_metadata.get_intrinsic<T>(key);
      }
      else
      {
        return default_value;
      }
    }
  }

  template<typename T>
  T get_default(std::string key, T default_value = T())
  {
    PropertyDictionary const& defaults = get_defaults();
    if (defaults.contains(key))
    {
      return get_defaults().get<T>(key);
    }
    else
    {
      std::string parent = get_parent();
      if (!parent.empty())
      {
        Metadata& parent_metadata = get_collection().get(parent);
        return parent_metadata.get_default<T>(key);
      }
      else
      {
        return default_value;
      }
    }
  }

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

protected:

  /// Get the entire PropertyDictionary of defaults.
  PropertyDictionary const& get_defaults() const;

  /// Get the entire PropertyDictionary of intrinsics.
  PropertyDictionary const& get_intrinsics() const;

  /// Recursive function that iterates through the tree and prints the values.
  void trace_tree(pt::ptree const* pTree, std::string prefix);

  /// Get the raw property tree containing metadata.
  pt::ptree const& get_ptree();

  /// Clear the raw property tree. Should only be done after all data needed
  /// has been read from it.
  void clear_ptree();

private:
  /// Reference to the collection this metadata is in (e.g. "maptile", "thing", etc.)
  MetadataCollection& m_collection;

  /// The type associated with this metadata.
  std::string m_type;

  /// The parent type, if any.
  std::string m_parent;

  /// The display name.
  std::string m_display_name;

  /// The display plural.
  std::string m_display_plural;

  /// A brief description.
  std::string m_description;

  /// Read-only properties (intrinsics).
  PropertyDictionary m_intrinsics;

  /// Read-write properties. The ones contained here are the defaults
  /// which are copied to a target (Thing, MapTile, etc.) on creation.
  PropertyDictionary m_defaults;

  /// A vector containing the children of this object.
  std::vector<std::string> m_children;

  /// The raw metadata. It is a bit wasteful to save this
  /// independently of other data, so it should probably only be saved
  /// for as long as it is needed.
  pt::ptree m_raw_ptree;

  /// Boolean indicating whether this metadata has graphics associated with it.
  bool m_has_tiles;

  /// Location of this metadata's graphics on the tilesheet.
  sf::Vector2u m_tile_location;
};

#endif // METADATA_H

