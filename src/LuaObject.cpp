#include "stdafx.h"

#include "LuaObject.h"

#include "actions/ActionResult.h"
#include "Direction.h"
#include "Entity.h"
#include "EntityId.h"
#include "Gender.h"
#include "Property.h"

Lua::Lua()
{
  // Initialize the logger.
  SET_UP_LOGGER("Lua", true);

  // Initialize the Lua interpreter.
  L_ = luaL_newstate();

  // Load the base libraries.
  luaL_openlibs(L_);

  // Register enums.
  addActionResultEnumToLua();
  addGenderEnumToLua();
  addPropertyTypeEnumToLua();

  // Register the trace function.
  register_function("print_trace", Lua::LUA_trace);

  // Run the initial Lua script.
  require("resources/default", true);
}

Lua::~Lua()
{
  /// Clean up Lua.
  lua_close(L_);
}

void Lua::register_function(std::string name, lua_CFunction func)
{
  lua_register(L_, name.c_str(), func);
}

void Lua::do_file(FileName filename)
{
  const char * file = filename.c_str();
  if (luaL_dofile(L_, file))
  {
    fprintf(stderr, "%s\n", lua_tostring(L_, -1));
  }
}

void Lua::require(FileName packagename, bool fatal)
{
  int err;

  // Call the "require" function.
  lua_getfield(L_, LUA_GLOBALSINDEX, "require"); // function
  lua_pushstring(L_, packagename.c_str());     // arg 0: module name
  err = lua_pcall(L_, 1, 1, 0);

  if (err)
  {
    if (fatal)
    {
      FATAL_ERROR("%s", lua_tostring(L_, -1));
    }
    else
    {
      MAJOR_ERROR("%s", lua_tostring(L_, -1));
    }
  }
  else
  {
    // Store module table in global var
    lua_setfield(L_, LUA_GLOBALSINDEX, packagename.c_str());
  }
}

void Lua::set_global(std::string name, lua_Integer value)
{
  lua_pushinteger(L_, value);
  lua_setglobal(L_, name.c_str());
}

void Lua::stackDump() const
{
  printf("Lua Stack Dump: ");
  int i;
  int top = lua_gettop(L_);
  for (i = 1; i <= top; i++)
  {  /* repeat for each level */
    int t = lua_type(L_, i);
    switch (t)
    {

      case LUA_TSTRING:  /* strings */
        printf("\"%s\"", lua_tostring(L_, i));
        break;

      case LUA_TBOOLEAN:  /* booleans */
        printf(lua_toboolean(L_, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:  /* numbers */
        printf("%g", lua_tonumber(L_, i));
        break;

      default:  /* other values */
        printf("[%s]", lua_typename(L_, t));
        break;

    }
    printf(" ");  /* put a separator */
  }
  printf("\n");  /* end the listing */
}

bool Lua::add_enum(const char* tname, ...)
{
  /// @note Here's the Lua code we're building and executing to define the
  ///       enum.
  /// ```
  /// <tname> = setmetatable( {}, {
  ///      __index = {
  ///          <name1> = {
  ///              value = <value1>,
  ///              type = \"<tname>\"
  ///          },
  ///          ...
  ///      },
  ///      __newindex = function(table, key, value)
  ///          error(\"Attempt to modify read-only table\")
  ///      end,
  ///      __metatable = false
  /// });
  /// ```

  va_list args;
  std::stringstream code;
  char* ename;

  code << tname << " = setmetatable({}, {";
  code << "__index = {";

  // Iterate over the variadic arguments adding the enum values.
  va_start(args, tname);
  while ((ename = va_arg(args, char*)) != 0)
  {
    int evalue = va_arg(args, int);
    code << ename << "={value=" << evalue << ",type=\"" << tname << "\"},";
  }
  va_end(args);

  code << "},";
  code << "__newindex = function(table, key, value) error(\"Attempt to modify read-only table\") end,";
  code << "__metatable = false});";

  // Execute lua code
  if (luaL_loadbuffer(L_, code.str().c_str(), code.str().length(), 0) || lua_pcall(L_, 0, 0, 0))
  {
    fprintf(stderr, "%s\n", lua_tostring(L_, -1));
    lua_pop(L_, 1);
    return false;
  }
  return true;
}

bool Lua::check_enum_type(const char* tname, int index) const
{
  if (!lua_istable(L_, index))
  {
    stackDump();
    CLOG(FATAL, "Lua") << "Requested stack level " << index << " doesn't contain a table";
  }

  lua_pushstring(L_, "type");
  lua_gettable(L_, index - 1);
  if (!lua_isnil(L_, -1))
  {
    const char* type = lua_tostring(L_, -1);
    if (strcmp(type, tname) == 0)
    {
      lua_pop(L_, 1);
      return true;
    }
  }
  lua_pop(L_, 1);
  return false;
}

int Lua::get_enum_value(int index)
{
  int value;
  lua_pushstring(L_, "value");
  lua_gettable(L_, index - 1);
  value = static_cast<int>(luaL_checkinteger(L_, -1));
  lua_pop(L_, 1);
  return value;
}

/// Pushes a property onto the Lua stack.
///
/// Currently supported types are:
///  * Boolean
///  * String
///  * Number
///  * IntVec2
///  * Direction
///  * Color
///
/// @param    value   Value to deduce the type of.
/// @return           The number of arguments pushed to the stack.
///                   If the type could not be deduced, it will push
///                   nil and return 1.

int Lua::push_value(Property property)
{
  switch (property.type())
  {
    case Property::Type::Null:         lua_pushnil(L_); return 1;
    case Property::Type::Boolean:      return push_value(property.as<bool>());
    case Property::Type::String:       return push_value(property.as<std::string>());
    case Property::Type::Integer:      return push_value(property.as<int64_t>());
    case Property::Type::Number:       return push_value(property.as<double>());
    case Property::Type::IntVec2:      return push_value(property.as<IntVec2>());
    case Property::Type::Direction:    return push_value(property.as<Direction>());
    case Property::Type::Color:        return push_value(property.as<Color>());
    default:
    {
      std::stringstream ss;
      ss << property.type();
      MAJOR_ERROR("Attempted to push Property of type %s to Lua", ss.str().c_str());
      lua_pushnil(L_);
      return 1;
    }
  }
}

int Lua::push_value(unsigned int value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value));
  return 1;
}

int Lua::push_value(int value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value));
  return 1;
}

int Lua::push_value(int64_t value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value));
  return 1;
}

int Lua::push_value(EntityId value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value));
  return 1;
}

int Lua::push_value(float value)
{
  lua_pushnumber(L_, static_cast<lua_Number>(value));
  return 1;
}

int Lua::push_value(double value)
{
  lua_pushnumber(L_, static_cast<lua_Number>(value));
  return 1;
}

int Lua::push_value(bool value)
{
  lua_pushboolean(L_, static_cast<int>(value));
  return 1;
}

int Lua::push_value(std::string value)
{
  lua_pushstring(L_, value.c_str());
  return 1;
}

int Lua::push_value(UintVec2 value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value.x));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.y));
  return 2;
}

int Lua::push_value(IntVec2 value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value.x));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.y));
  return 2;
}

int Lua::push_value(Direction value)
{
  Vec3i vec = static_cast<Vec3i>(value);
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.x));
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.y));
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.z));
  return 3;
}

int Lua::push_value(sf::Color value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value.r));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.g));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.b));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.a));
  return 4;
}

Property::Type Lua::pop_type()
{
  Property::Type return_value;
  if (!check_enum_type("PropertyType", -1))
  {
    stackDump();
    CLOG(FATAL, "Lua") << "Expected PropertyType on the Lua stack, not found";
  }
  else
  {
    return_value = static_cast<Property::Type>(get_enum_value(-1));
  }
  lua_pop(L_, 1);
  return return_value;
}

Property Lua::pop_value(Property::Type type)
{
  Property property;
  switch (type)
  {
    case Property::Type::Null: 
      break;

    case Property::Type::Boolean:
      property = Property::from(lua_toboolean(L_, -1) != 0);
      lua_pop(L_, 1);
      break;

    case Property::Type::String: 
      property = Property::from(lua_tostring(L_, -1));
      lua_pop(L_, 1);
      break;

    case Property::Type::Integer:
      property = Property::from(static_cast<int64_t>(lua_tointeger(L_, -1)));
      lua_pop(L_, 1);
      break;

    case Property::Type::Number:
      property = Property::from(static_cast<double>(lua_tonumber(L_, -1)));
      lua_pop(L_, 1);
      break;

    case Property::Type::ActionResult:
      if (!check_enum_type("ActionResult", -1))
      {
        stackDump();
        CLOG(ERROR, "Lua") << "Expected ActionResult on the Lua stack, not found";
      }
      else
      {
        property = Property::from(static_cast<ActionResult>(get_enum_value(-1)));
      }
      lua_pop(L_, 1);
      break;

    case Property::Type::IntVec2:
      property = Property::from(IntVec2(static_cast<int>(lua_tointeger(L_, -2)),
                                        static_cast<int>(lua_tointeger(L_, -1))));
      lua_pop(L_, 2);
      break;

    case Property::Type::Direction: 
      property = Property::from(Direction(static_cast<int>(lua_tointeger(L_, -3)),
                                          static_cast<int>(lua_tointeger(L_, -2)),
                                          static_cast<int>(lua_tointeger(L_, -1))));
      lua_pop(L_, 3);
      break;

    case Property::Type::Color: 
      property = Property::from(Color(static_cast<int>(lua_tointeger(L_, -4)),
                                      static_cast<int>(lua_tointeger(L_, -3)),
                                      static_cast<int>(lua_tointeger(L_, -2)),
                                      static_cast<int>(lua_tointeger(L_, -1))));
      lua_pop(L_, 4);
      break;

    case Property::Type::PropertyType:
      property = Property::from(pop_type());
      // pop_type() did the popping, none needed here
      break;

    default:
      CLOG(ERROR, "Lua") << "Unsupported Property::Type " << type;
      break;
  }

  return property;
}

unsigned int Lua::stack_slots(Property::Type type) const
{
  switch (type)
  {
    case Property::Type::Null: return 0;
    case Property::Type::Boolean: return 1;
    case Property::Type::String: return 1;
    case Property::Type::Integer: return 1;
    case Property::Type::Number: return 1;
    case Property::Type::ActionResult: return 1;
    case Property::Type::IntVec2: return 2;
    case Property::Type::Direction: return 3;
    case Property::Type::Color: return 4;
    case Property::Type::PropertyType: return 1;
    default: CLOG(ERROR, "Lua") << "Unsupported Property::Type " << type; return 0;
  }
}

Property Lua::call_thing_function(std::string function_name, 
                                  EntityId caller, 
                                  std::vector<Property> const & args, 
                                  Property::Type result_type,
                                  Property default_result)
{
  {
    Property return_value = default_result;
    std::string group = caller->get_type();

    int start_stack = lua_gettop(L_);

    // Push name of group onto the stack. (+1)
    lua_getglobal(L_, group.c_str());

    if (lua_isnoneornil(L_, -1))
    {
      // Category not found -- pop the group back off. (-1)
      lua_pop(L_, 1);
    }
    else
    {
      // Push name of function onto the stack. (+1)
      lua_getfield(L_, -1, function_name.c_str());

      if (lua_isnoneornil(L_, -1))
      {
        // Function not found -- pop the function and group names back off. (-2)
        lua_pop(L_, 2);

        CLOG(WARNING, "Lua") << "Could not find Lua function "
          << group << ":"
          << function_name;
      }
      else
      {
        // Remove the group name from the stack. (-1)
        lua_remove(L_, -2);

        // Push the caller's ID onto the stack. (+1)
        lua_pushinteger(L_, caller);

        // Push the arguments onto the stack. (+N)
        unsigned int lua_args = 0;
        for (auto& arg : args)
        {
          lua_args += stack_slots(arg.type());
          push_value(arg);
        }

        // Call the function with N+1 arguments and R results. (-(N+2), +R) = R-(N+2)
        //stackDump();
        int result = lua_pcall(L_, lua_args + 1, stack_slots(result_type), 0);
        //stackDump();
        if (result == 0)
        {
          // Pop the return value off of the stack. (-R)
          return_value = pop_value(result_type);
        }
        else
        {
          // Get the error message.
          char const* error_message = lua_tostring(L_, -1);
          MAJOR_ERROR("Error calling %s.%s: %s", group.c_str(), function_name.c_str(), error_message);

          // Pop the error message off the stack. (-1)
          lua_pop(L_, 1);
        }
      }
    }

    int end_stack = lua_gettop(L_);

    if (start_stack != end_stack)
    {
      FATAL_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", group.c_str(), function_name.c_str(), start_stack, end_stack);
    }

    return return_value;
  }
}

Property Lua::call_thing_function(std::string function_name, 
                                  EntityId caller, 
                                  std::vector<Property> const & args, 
                                  Property::Type result_type)
{
  return call_thing_function(function_name, caller, args, result_type, Property::empty(result_type));
}

Property Lua::get_type_intrinsic(std::string group,
                                 std::string name, 
                                 Property::Type type, 
                                 Property default_value)
{
  Property return_value = default_value;
  Property::Type return_type = default_value.type();

  int start_stack = lua_gettop(L_);

  // Push group onto the stack. (+1)
  lua_getglobal(L_, group.c_str());           // group <

  if (lua_isnoneornil(L_, -1))
  {
    // Category not found -- pop the name back off. (-1)
    lua_pop(L_, 1);
    MAJOR_ERROR("Lua class %s was not found", group.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(L_, -1, "get_intrinsic");             // group get <

    if (lua_isnoneornil(L_, -1))
    {
      // Function not found -- pop the function and group names back off. (-2)
      lua_pop(L_, 2);
      MAJOR_ERROR("Lua function %s:get_intrinsic() was not found", group.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(L_, -2);                 // group get group <
      lua_remove(L_, -3);                    // get group <
      lua_pushstring(L_, name.c_str());      // get group name <

      int num_results = stack_slots(type) + 1;
      //stackDump();
      int result = lua_pcall(L_, 2, num_results, 0);   // (result|nil|err) <
      //stackDump();
      if (result == 0)
      {
        return_type = pop_type();
        return_value = pop_value(type);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(L_, -1);
        MAJOR_ERROR("Error calling %s:get_intrinsic(%s): %s", group.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(L_, 1);
      }
    }
  }

  int end_stack = lua_gettop(L_);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_intrinsic): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  bool breakpoint = false;
  if (name == "inventory_size")
  {
    breakpoint = true;
  }

  return return_value;
}

Property Lua::get_type_intrinsic(std::string group,
                                 std::string name, 
                                 Property::Type type)
{
  return get_type_intrinsic(group, name, type, Property::empty(type));
}

void Lua::set_type_intrinsic(std::string group, std::string name, Property value)
{
  int start_stack = lua_gettop(L_);

  // Push type of group onto the stack. (+1)
  lua_getglobal(L_, group.c_str());           // group <

  if (lua_isnoneornil(L_, -1))
  {
    // Category not found -- pop the name back off. (-1)
    lua_pop(L_, 1);
    MAJOR_ERROR("Lua class %s was not found", group.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(L_, -1, "set_intrinsic");             // group get <

    if (lua_isnoneornil(L_, -1))
    {
      // Function not found -- pop the function and group names back off. (-2)
      lua_pop(L_, 2);
      MAJOR_ERROR("Lua function %s:set_intrinsic() was not found", group.c_str());
    }
    else
    {
      // Push the group name up to the front of the stack.
      lua_pushvalue(L_, -2);                         // group set group <
      lua_remove(L_, -3);                            // set group <
      lua_pushstring(L_, name.c_str());              // set group name <
      push_value(value);                             // set group name value <
      int num_args = 2 + stack_slots(value.type());
      //stackDump();
      int result = lua_pcall(L_, num_args, 0, 0);    // (nil|err) <
      //stackDump();
      if (result != 0)
      {
        // Get the error message.
        char const* error_message = lua_tostring(L_, -1);
        MAJOR_ERROR("Error calling %s:set_intrinsic(%s): %s", group.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(L_, 1);
      }
    }
  }

  int end_stack = lua_gettop(L_);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:set_intrinsic): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }
}

lua_State* Lua::state()
{
  return L_;
}

int Lua::LUA_trace(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "Expected 1 argument, got " << num_args;
  }
  else
  {
    std::string str = lua_tostring(L, 1);
    CLOG(TRACE, "Lua") << str;
  }

  return 0;
}

void Lua::addActionResultEnumToLua()
{
  add_enum("ActionResult",
           "FailureActorCantPerform", ActionResult::FailureActorCantPerform,
           "FailureContainerIsEmpty", ActionResult::FailureContainerIsEmpty,
           "FailureContainerCantBeSelf", ActionResult::FailureContainerCantBeSelf,
           "FailureCircularReference", ActionResult::FailureCircularReference,
           "FailureItemNotEquippable", ActionResult::FailureItemNotEquippable,
           "FailureItemNotEquipped", ActionResult::FailureItemNotEquipped,
           "FailureItemWielded", ActionResult::FailureItemWielded,
           "FailureItemEquipped", ActionResult::FailureItemEquipped,
           "FailureNotEnoughHands", ActionResult::FailureNotEnoughHands,
           "FailureTooStupid", ActionResult::FailureTooStupid,
           "FailureNotLiquidCarrier", ActionResult::FailureNotLiquidCarrier,
           "FailureSelfReference", ActionResult::FailureSelfReference,
           "FailureTargetNotAContainer", ActionResult::FailureTargetNotAContainer,
           "FailureNotInsideContainer", ActionResult::FailureNotInsideContainer,
           "FailureContainerOutOfReach", ActionResult::FailureContainerOutOfReach,
           "FailureEntityOutOfReach", ActionResult::FailureEntityOutOfReach,
           "FailureAlreadyPresent", ActionResult::FailureAlreadyPresent,
           "FailureInventoryCantContain", ActionResult::FailureInventoryCantContain,
           "FailureInventoryFull", ActionResult::FailureInventoryFull,
           "FailureNotPresent", ActionResult::FailureNotPresent,
           "Failure", ActionResult::Failure,
           "Pending", ActionResult::Pending,
           "Success", ActionResult::Success,
           "SuccessDestroyed", ActionResult::SuccessDestroyed,
           "SuccessSelfReference", ActionResult::SuccessSelfReference,
           "SuccessSwapHands", ActionResult::SuccessSwapHands,
           0
  );
}

void Lua::addGenderEnumToLua()
{
  add_enum("Gender",
           "None", Gender::None,
           "Male", Gender::Male,
           "Female", Gender::Female,
           "Neuter", Gender::Neuter,
           "Spivak", Gender::Spivak,
           "FirstPerson", Gender::FirstPerson,
           "SecondPerson", Gender::SecondPerson,
           "Plural", Gender::Plural,
           "Hanar", Gender::Hanar,
           "UnknownEntity", Gender::UnknownEntity,
           "UnknownPerson", Gender::UnknownPerson,
           "Count", Gender::Count,
           0
  );
}

void Lua::addPropertyTypeEnumToLua()
{
  add_enum("PropertyType",
           "Null", Property::Type::Null,
           "Boolean", Property::Type::Boolean,
           "String", Property::Type::String,
           "Integer", Property::Type::Integer,
           "Number", Property::Type::Number,
           "IntVec2", Property::Type::IntVec2,
           "ActionResult", Property::Type::ActionResult,
           "Direction", Property::Type::Direction,
           "Color", Property::Type::Color,
           "PropertyType", Property::Type::PropertyType,
           "Unknown", Property::Type::Unknown,
           0);
}
