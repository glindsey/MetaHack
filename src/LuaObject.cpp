#include "stdafx.h"

#include "LuaObject.h"

#include "actions/ActionResult.h"
#include "Direction.h"
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

bool Lua::check_enum_type(const char* tname, int index)
{
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
///  * IntegerVec2
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
    case Property::Type::Null: lua_pushnil(L_); return 1;
    case Property::Type::Boolean: return push_value(property.as<Boolean>());
    case Property::Type::String: return push_value(property.as<String>());
    case Property::Type::EightBits: return push_value(property.as<EightBits>());
    case Property::Type::Number: return push_value(property.as<Number>());
    case Property::Type::BigInteger: return push_value(property.as<BigInteger>());
    case Property::Type::IntegerVec2: return push_value(property.as<IntegerVec2>());
    case Property::Type::Direction: return push_value(property.as<Direction>());
    case Property::Type::Color: return push_value(property.as<Color>());
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

int Lua::push_value(Vec2u value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value.x));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.y));
  return 2;
}

int Lua::push_value(IntegerVec2 value)
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

Property Lua::pop_value(Property::Type type)
{
  std::unique_ptr<Property> property(new Property());
  switch (type)
  {
    case Property::Type::Null: 
      break;

    case Property::Type::Boolean:
      property.reset(new Property(static_cast<Boolean>(lua_tointeger(L_, -1))));
      lua_pop(L_, 1);
      break;

    case Property::Type::String: 
      property.reset(new Property(lua_tostring(L_, -1)));
      lua_pop(L_, 1);
      break;

    case Property::Type::EightBits: 
      property.reset(new Property(static_cast<EightBits>(lua_tointeger(L_, -1))));
      lua_pop(L_, 1);
      break;

    case Property::Type::Number:
      property.reset(new Property(static_cast<Real>(lua_tonumber(L_, -1))));
      lua_pop(L_, 1);
      break;

    case Property::Type::BigInteger: 
      property.reset(new Property(static_cast<BigInteger>(lua_tointeger(L_, -1))));
      lua_pop(L_, 1);
      break;

    case Property::Type::IntegerVec2:
      property.reset(new Property(IntegerVec2(static_cast<int>(lua_tointeger(L_, -2)),
                                              static_cast<int>(lua_tointeger(L_, -1)))));
      lua_pop(L_, 2);
      break;

    case Property::Type::Direction: 
      property.reset(new Property(Direction(static_cast<int>(lua_tointeger(L_, -3)),
                                            static_cast<int>(lua_tointeger(L_, -2)),
                                            static_cast<int>(lua_tointeger(L_, -1)))));
      lua_pop(L_, 3);
      break;

    case Property::Type::Color: 
      property.reset(new Property(Color(static_cast<int>(lua_tointeger(L_, -4)),
                                        static_cast<int>(lua_tointeger(L_, -3)),
                                        static_cast<int>(lua_tointeger(L_, -2)),
                                        static_cast<int>(lua_tointeger(L_, -1)))));
      lua_pop(L_, 4);
      break;

    default:
      break;
  }

  return *property;
}

unsigned int Lua::stack_slots(Property::Type type)
{
  switch (type)
  {
    case Property::Type::Null: return 0;
    case Property::Type::Boolean: return 1;
    case Property::Type::String: return 1;
    case Property::Type::EightBits: return 1;
    case Property::Type::Number: return 1;
    case Property::Type::BigInteger: return 1;
    case Property::Type::IntegerVec2: return 2;
    case Property::Type::Direction: return 3;
    case Property::Type::Color: return 4;
    default: return 0;
  }
}

Property Lua::get_type_intrinsic(std::string category, std::string name, Property::Type type, Property default_value)
{
  Property return_value = default_value;

  int start_stack = lua_gettop(L_);

  // Push category onto the stack. (+1)
  lua_getglobal(L_, category.c_str());           // category <

  if (lua_isnoneornil(L_, -1))
  {
    // Category not found -- pop the name back off. (-1)
    lua_pop(L_, 1);
    MAJOR_ERROR("Lua class %s was not found", category.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(L_, -1, "get_intrinsic");             // category get <

    if (lua_isnoneornil(L_, -1))
    {
      // Function not found -- pop the function and category names back off. (-2)
      lua_pop(L_, 2);
      MAJOR_ERROR("Lua function %s:get_intrinsic() was not found", category.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(L_, -2);                 // category get category <
      lua_remove(L_, -3);                    // get category <
      lua_pushstring(L_, name.c_str());      // get category name <

      int num_results = stack_slots(type);
      int result = lua_pcall(L_, 2, num_results, 0);   // (result|nil|err) <
      if (result == 0)
      {
        return_value = pop_value(type);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(L_, -1);
        MAJOR_ERROR("Error calling %s:get_intrinsic(%s): %s", category.c_str(), name.c_str(), error_message);

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

  return return_value;
}

Property Lua::get_type_intrinsic(std::string category, std::string name, Property::Type type)
{
  return get_type_intrinsic(category, name, type, Property(type));
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
           "EightBits", Property::Type::EightBits,
           "Number", Property::Type::Number,
           "BigInteger", Property::Type::BigInteger,
           "IntegerVec2", Property::Type::IntegerVec2,
           "EntityId", Property::Type::EntityId,
           "ActionResult", Property::Type::ActionResult,
           "Direction", Property::Type::Direction,
           "Color", Property::Type::Color,
           0);
}
