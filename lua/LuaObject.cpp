#include "stdafx.h"

#include "lua/LuaObject.h"

#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "types/Direction.h"
#include "types/Color.h"
#include "types/Gender.h"
#include "Service.h"
#include "services/IGameRules.h"

Lua::Lua()
{
  // Initialize the Lua interpreter.
  L_ = luaL_newstate();

  // Load the base libraries.
  luaL_openlibs(L_);

  // Register enums.
  addGenderEnumToLua();
  addLuaTypeEnumToLua();

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
      CLOG(FATAL, "Lua") << "Could not load Lua script \"" << packagename << "\": " << lua_tostring(L_, -1);
    }
    else
    {
      CLOG(ERROR, "Lua") << "Could not load Lua script \"" << packagename << "\": " << lua_tostring(L_, -1);
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

void Lua::set_global(std::string name, std::string value)
{
  lua_pushstring(L_, value.c_str());
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
    CLOG(ERROR, "Lua") << "Requested stack level " << index << " doesn't contain a table";
    return false;
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

int Lua::push_value(json value)
{
  if (value.is_null())
  {
    lua_pushnil(L_); 
    return 1;
  }
  else if (value.is_boolean())
  {
    return push_value(value.get<bool>());
  }
  else if (value.is_string())
  {
    return push_value(value.get<std::string>());
  }
  else if (value.is_number())
  {
    if (value.is_number_integer())
    {
      if (value.is_number_unsigned())
      {
        return push_value(value.get<unsigned int>());
      }
      else
      {
        return push_value(value.get<int>());
      }
    }
    else
    {
      return push_value(value.get<float>());
    }
  }
  else if (value.is_array())
  {
    return push_array(value);
  }
  else if (value.is_object())
  {
    return push_object(value);
  }
  else
  {
    CLOG(ERROR, "Lua") << "Attempted to push data of type " << value.type_name() << " to Lua";
    lua_pushnil(L_);
    return 1;
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

int Lua::push_value(RealVec2 value)
{
  lua_pushnumber(L_, static_cast<lua_Number>(value.x));
  lua_pushnumber(L_, static_cast<lua_Number>(value.y));
  return 2;
}

int Lua::push_value(Direction value)
{
  IntVec3 vec = static_cast<IntVec3>(value);
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.x));
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.y));
  lua_pushinteger(L_, static_cast<lua_Integer>(vec.z));
  return 3;
}

int Lua::push_value(Color value)
{
  lua_pushinteger(L_, static_cast<lua_Integer>(value.r()));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.g()));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.b()));
  lua_pushinteger(L_, static_cast<lua_Integer>(value.a()));
  return 4;
}

int Lua::push_array(json value)
{
  int stack_slots = 0;
  size_t array_size = value.size();

  std::string type = value[0];

  for (int index = 1; index < array_size; ++index)
  {
    stack_slots += push_value(value[index]);
  }

  return stack_slots;
}

int Lua::push_object(json value)
{
  CLOG(ERROR, "Lua") << "No support for pushing JSON objects to Lua";
  lua_pushnil(L_);
  return 1;
}

Lua::Type Lua::pop_type()
{
  Lua::Type return_value;
  if (!check_enum_type("LuaType", -1))
  {
    stackDump();
    CLOG(FATAL, "Lua") << "Expected LuaType on the Lua stack, not found";
  }
  else
  {
    return_value = static_cast<Lua::Type>(get_enum_value(-1));
  }
  lua_pop(L_, 1);
  return return_value;
}

json Lua::pop_value(Lua::Type type)
{
  json value;
  switch (type)
  {
    case Type::Null:
      lua_pop(L_, 1);
      break;

    case Type::Boolean:
      value = (lua_toboolean(L_, -1) != 0);
      lua_pop(L_, 1);
      break;

    case Type::String: 
      value = std::string(lua_tostring(L_, -1));
      lua_pop(L_, 1);
      break;

    case Type::Integer:
      value = static_cast<int>(lua_tointeger(L_, -1));
      lua_pop(L_, 1);
      break;

    case Type::Unsigned:
      value = static_cast<unsigned int>(lua_tointeger(L_, -1));
      lua_pop(L_, 1);
      break;

    case Type::Number:
      value = static_cast<double>(lua_tonumber(L_, -1));
      lua_pop(L_, 1);
      break;

    case Type::IntVec2:
      value = IntVec2(static_cast<int32_t>(lua_tointeger(L_, -2)), 
                      static_cast<int32_t>(lua_tointeger(L_, -1)));
      lua_pop(L_, 2);
      break;

    case Type::UintVec2:
      value = UintVec2(static_cast<uint32_t>(lua_tointeger(L_, -2)), 
                       static_cast<uint32_t>(lua_tointeger(L_, -1)));
      lua_pop(L_, 2);
      break;

    case Type::RealVec2:
      value = RealVec2(static_cast<float>(lua_tonumber(L_, -2)), 
                       static_cast<float>(lua_tonumber(L_, -1)));
      lua_pop(L_, 2);
      break;

    case Type::Direction:
      value = Direction(static_cast<int>(lua_tointeger(L_, -3)), 
                        static_cast<int>(lua_tointeger(L_, -2)),
                        static_cast<int>(lua_tointeger(L_, -1)));
      lua_pop(L_, 3);
      break;

    case Type::Color: 
      value = Color(static_cast<uint8_t>(lua_tointeger(L_, -4)), 
                    static_cast<uint8_t>(lua_tointeger(L_, -3)),
                    static_cast<uint8_t>(lua_tointeger(L_, -2)),
                    static_cast<uint8_t>(lua_tointeger(L_, -1)));
      lua_pop(L_, 4);
      break;

    case Type::Type:
      value = static_cast<int>(pop_type());
      // pop_type() did the popping, none needed here
      break;

    default:
      CLOG(ERROR, "Lua") << "Unsupported Lua Type " << type;
      break;
  }

  return value;
}

unsigned int Lua::stack_slots(Lua::Type type) const
{
  switch (type)
  {
    case Type::Null: return 1;     // 1, because nil is still a Lua value
    case Type::Boolean: return 1;
    case Type::String: return 1;
    case Type::Integer: return 1;
    case Type::Unsigned: return 1;
    case Type::Number: return 1;
    case Type::IntVec2: return 2;
    case Type::UintVec2: return 2;
    case Type::RealVec2: return 2;
    case Type::Direction: return 3;
    case Type::Color: return 4;
    case Type::Type: return 1;
    default: CLOG(ERROR, "Lua") << "Unsupported Lua Type " << type; return 0;
  }
}

std::string Lua::find_lua_function(std::string category, std::string suffix)
{
  std::string result = find_lua_function_(category, suffix);
  if (result.empty())
  {
    // Get this entity category's templates.
    json const& templates = Service<IGameRules>::get().category(category).value("templates", json::array());
    for (auto index = 0; index < templates.size(); ++index)
    {
      result = find_lua_function_(templates[index].get<std::string>(), suffix);
      if (result != "") return result;
    }
  }

  // Nothing found, return empty string.
  return result;
}

std::string Lua::find_lua_function_(std::string category, std::string suffix)
{
  std::string function_name = category + "_" + suffix;

  // Push function onto the stack. (+1)
  lua_getglobal(L_, function_name.c_str());

  if (lua_isnoneornil(L_, -1))
  {
    // Function not found; pop the result off and return an empty string. (-1)
    lua_pop(L_, 1);
    return std::string();
  }
  else
  {
    // Function found; pop the result off. (-1)
    lua_pop(L_, 1);
    return function_name;
  }
}

json Lua::call_thing_function(std::string function_name, 
                              EntityId caller,
                              json const& args,
                              json default_result)
{
  json return_value = default_result;
  Lua::Type return_type;
  std::string caller_type = COMPONENTS.category[caller];

  int start_stack = lua_gettop(L_);

  auto original_function_name = caller_type + "_" + function_name;

  // Get full function name (with type prefixed).
  auto full_function_name = find_lua_function(caller_type, function_name);

  // If it doesn't exist after going all the way up the family tree, return 
  // the default result.
  if (full_function_name.empty())
  {
    /// @todo Probably don't need a warning here, that's what the default result is for
    CLOG(WARNING, "Lua") << "Could not find Lua function "
      << original_function_name << " after traversing family tree";

    return default_result;
  }

  // Push name of function onto the stack. (+1)
  // (Shouldn't fail here; if it does, find_lua_function() is borked.)
  lua_getglobal(L_, full_function_name.c_str());

  // Push the caller's ID onto the stack. (+1)
  lua_pushinteger(L_, caller);

  // Push the arguments onto the stack. (+N)
  unsigned int lua_args = 0;
  for (auto& arg : args)
  {
    lua_args += push_value(arg);
  }

  // Call the function with N+1 arguments and R+1 results. (-(N+2), +(R+1)) = R-(N+1)
  //stackDump();
  // auto number_of_results = stack_slots(result_type) + 1
  int result = lua_pcall(L_, lua_args + 1, LUA_MULTRET, 0);
  //stackDump();
  if (result == 0)
  {
    // Pop the return type off of the stack. (-1)
    return_type = pop_type();
    // Pop the return value off of the stack. (-R)
    return_value = pop_value(return_type);
    /// @todo Handle multiple return values.
  }
  else
  {
    // Get the error message.
    char const* error_message = lua_tostring(L_, -1);
    CLOG(ERROR, "Lua") << "Error calling " << full_function_name << 
      " (" << original_function_name << "): " << error_message;

    // Pop the error message off the stack. (-1)
    lua_pop(L_, 1);
  }

  int end_stack = lua_gettop(L_);

  if (start_stack != end_stack)
  {
    CLOG(FATAL, "Lua") << "*** LUA STACK MISMATCH (" << full_function_name <<
      " (" << original_function_name << "): Started at " << start_stack << 
      ", ended at " << end_stack;
  }

  return return_value;
}

json Lua::call_modifier_function(std::string property_name,
                                 json unmodified_value,
                                 EntityId affected_id,
                                 std::string responsible_group,
                                 EntityId responsible_id,
                                 std::string suffix)
{
  json return_value = unmodified_value;
  Lua::Type return_type;
  std::string function_name = "modify_property_" + property_name;
  if (!suffix.empty())
  {
    function_name += "_" + suffix;
  }

  int start_stack = lua_gettop(L_);

  auto original_function_name = responsible_group + "_" + function_name;

  // Get full function name (with type prefixed).
  auto full_function_name = find_lua_function(responsible_group, function_name);

  // If it doesn't exist after going all the way up the family tree, return 
  // the default result.
  if (full_function_name.empty())
  {
    /// @todo Probably don't need a warning here, that's what the default result is for
    CLOG(WARNING, "Lua") << "Could not find Lua function "
      << original_function_name << " after traversing family tree";

    return return_value;
  }

  // Push name of function onto the stack. (+1)
  // (Shouldn't fail here; if it does, find_lua_function() is borked.)
  lua_getglobal(L_, full_function_name.c_str());

  // Push the affected entity's ID onto the stack. (+1)
  lua_pushinteger(L_, affected_id);

  // Push the responsible entity's ID onto the stack. (+1)
  lua_pushinteger(L_, affected_id);

  // Push the "before" value onto the stack. (+N)
  unsigned int lua_args = push_value(unmodified_value);

  // Call the function with N+2 arguments and R+1 results. (-(N+2), +(R+1)) = R-(N+1)
  //stackDump();
  int result = lua_pcall(L_, lua_args + 1, LUA_MULTRET, 0);
  //stackDump();
  if (result == 0)
  {
    // Pop the return type off of the stack. (-1)
    return_type = pop_type();
    // Pop the return value off of the stack. (-R)
    return_value = pop_value(return_type);
  }
  else
  {
    // Get the error message.
    char const* error_message = lua_tostring(L_, -1);
    CLOG(ERROR, "Lua") << "Error calling " << full_function_name <<
      " (" << original_function_name << "): " << error_message;

    // Pop the error message off the stack. (-1)
    lua_pop(L_, 1);
  }

  return return_value;
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

void Lua::addLuaTypeEnumToLua()
{
  add_enum("LuaType",
           "Null", Lua::Type::Null,
           "Boolean", Lua::Type::Boolean,
           "String", Lua::Type::String,
           "Integer", Lua::Type::Integer,
           "Unsigned", Lua::Type::Unsigned,
           "Number", Lua::Type::Number,
           "IntVec2", Lua::Type::IntVec2,
           "UintVec2", Lua::Type::UintVec2,
           "RealVec2", Lua::Type::RealVec2,
           "Direction", Lua::Type::Direction,
           "Color", Lua::Type::Color,
           "Type", Lua::Type::Type,
           "Unknown", Lua::Type::Unknown,
           0);
}
