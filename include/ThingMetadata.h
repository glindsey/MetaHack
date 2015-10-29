#ifndef THINGMETADATA_H
#define THINGMETADATA_H

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <SFML/Graphics.hpp>

#include "common_types.h"

#include "ActionResult.h"
#include "ThingRef.h"

class ThingMetadata
{
public:
  ThingMetadata(std::string type);
  virtual ~ThingMetadata();

  std::string const& get_pretty_name() const;
  std::string const& get_pretty_plural() const;
  std::string const& get_description() const;
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

  sf::Vector2u get_tile_coords() const;

  /// Try to call a Lua function that takes one ThingId argument: the caller.
  ///
  /// If the function does not exist, attempts to step up to the parent type
  /// and call the function there, up until there's no parent to call.
  /// @param function_name  Name of the function to call
  /// @param caller         ThingId of the thing calling the function
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function_1(std::string function_name,
                                   ThingId caller,
                                   ActionResult default_result = ActionResult::Success);

  /// Try to call a Lua function that takes two ThingId arguments: the caller,
  /// and a target.
  ///
  /// If the function does not exist, attempts to step up to the parent type
  /// and call the function there, up until there's no parent to call.
  /// @param function_name  Name of the function to call
  /// @param caller         ThingId of the thing calling the function
  /// @param target         ThingId of the target of the function
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function_2(std::string function_name,
                                   ThingId caller,
                                   ThingId target,
                                   ActionResult default_result = ActionResult::Success);

  /// Try to call a Lua function that takes two ThingId arguments: the caller,
  /// and two targets.
  ///
  /// If the function does not exist, attempts to step up to the parent type
  /// and call the function there, up until there's no parent to call.
  /// @param function_name  Name of the function to call
  /// @param caller         ThingId of the thing calling the function
  /// @param target1        ThingId of target #1 of the function
  /// @param target2        ThingId of target #1 of the function
  /// @param default_result The default result if function is not found 
  ///                       (defaults to ActionResult::Success).
  /// @return An ActionResult containing the result of the call.
  ActionResult call_lua_function_3(std::string function_name,
                                   ThingId caller,
                                   ThingId target1,
                                   ThingId target2,
                                   ActionResult default_result = ActionResult::Success);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

};

#endif // THINGMETADATA_H