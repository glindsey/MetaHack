#ifndef THINGMETADATA_H
#define THINGMETADATA_H

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <SFML/Graphics.hpp>

#include "common_types.h"

#include "ActionResult.h"
#include "Direction.h"
#include "Metadata.h"
#include "ThingRef.h"

class ThingMetadata : public Metadata
{
public:
  virtual ~ThingMetadata();

  static ThingMetadata& get(std::string type);

  template<typename T>
  T get_intrinsic(std::string key) const
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
        ThingMetadata parent_metadata = get(parent);
        return parent_metadata.get_intrinsic<T>(key);
      }
      else
      {
        return T();
      }
    }
  }

  template<typename T>
  T get_default(std::string key) const
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
        ThingMetadata parent_metadata = get(parent);
        return parent_metadata.get_default<T>(key);
      }
      else
      {
        return T();
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

private:
  /// Constructor is private; new instances are obtained using get().
  ThingMetadata(std::string type);

  /// Static collection of ThingMetadata instances.
  static boost::ptr_unordered_map<std::string, ThingMetadata> ThingMetadata::s_collection;
};

#endif // THINGMETADATA_H