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

  std::string const& get_parent() const;

  bool get_intrinsic_flag(std::string key, bool default_value = false) const;
  int get_intrinsic_value(std::string key, int default_value = 0) const;
  std::string get_intrinsic_string(std::string key, std::string default_value = "") const;

  FlagsMap const& get_intrinsic_flags() const;
  ValuesMap const& get_intrinsic_values() const;
  StringsMap const& get_intrinsic_strings() const;

  bool get_default_flag(std::string key, bool default_value = false) const;
  int get_default_value(std::string key, int default_value = 0) const;
  std::string get_default_string(std::string key, std::string default_value = "") const;

  FlagsMap const& get_default_flags() const;
  ValuesMap const& get_default_values() const;
  StringsMap const& get_default_strings() const;

  /// Try to call a Lua function given the caller and no other arguments.
  ///
  /// If the function does not exist, or it returns ActionResult::Pending,
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function(std::string function_name,
                                   ThingRef caller,
                                   ActionResult default_result = ActionResult::Success);

  /// Try to call a Lua function given the caller and one argument.
  ///
  /// If the function does not exist, or it returns ActionResult::Pending,
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param arg            Argument to pass to the function
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function(std::string function_name,
                                   ThingRef caller,
                                   lua_Integer arg,
                                   ActionResult default_result = ActionResult::Success);

  /// Try to call a Lua function given the caller and two arguments.
  ///
  /// If the function does not exist, or it returns ActionResult::Pending,
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param arg1           Argument #1 to pass to the function.
  /// @param arg2           Argument #2 to pass to the function.
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function(std::string function_name,
                                   ThingRef caller,
                                   lua_Integer arg1,
                                   lua_Integer arg2,
                                   ActionResult default_result = ActionResult::Success);

private:
  /// Constructor is private; new instances are obtained using get().
  ThingMetadata(std::string type);

  struct Impl;
  std::unique_ptr<Impl> pImpl;

  /// Static collection of ThingMetadata instances.
  static boost::ptr_unordered_map<std::string, ThingMetadata> ThingMetadata::collection;
};

#endif // THINGMETADATA_H