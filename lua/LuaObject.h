#pragma once
// Lua enum binding modified from code Copyright (c) 2010 Tom Distler.

// Forward declarations
namespace Actions
{
  class Action;
}
class Color;
class Direction;
class EntityId;
class Property;

/// This class encapsulates the Lua state and interface as an object.
class Lua : public boost::noncopyable
{
public:
  /// The type of objects passed back and forth to Lua.
  enum class Type
  {
    Null = 0,
    Boolean,
    String,
    Integer,        
    Unsigned,
    Number,         
    IntVec2,
    UintVec2,
    RealVec2,
    Direction,
    Color,
    Type,           ///< Type representing a Type. How meta.
    Unknown,        ///< Internally represented by a string, but only to contain the unknown Lua type
    Count           ///< Just for bookkeeping, doesn't need to be in LuaObject
  };
  
  /// Struct used when defining enums.
  template<typename T>
  struct EnumPair
  {
    std::string name;
    T value;
  };

  friend std::ostream& operator<<(std::ostream& os, Type const& type)
  {
    switch (type)
    {
      case Type::Null:         os << "Null"; break;
      case Type::Boolean:      os << "Boolean"; break;
      case Type::String:       os << "String"; break;
      case Type::Integer:      os << "Integer"; break;
      case Type::Unsigned:     os << "Unsigned"; break;
      case Type::Number:       os << "Number"; break;
      case Type::IntVec2:      os << "IntVec2"; break;
      case Type::UintVec2:     os << "UintVec2"; break;
      case Type::RealVec2:     os << "RealVec2"; break;
      case Type::Direction:    os << "Direction"; break;
      case Type::Color:        os << "Color"; break;
      case Type::Type:         os << "Type"; break;
      case Type::Unknown:      os << "Unknown"; break;
      default:                 os << "??? (" << static_cast<int>(type) << ")"; break;
    }
    return os;
  }

  Lua();

  virtual ~Lua();

  /// Register a function with Lua.
  void register_function(std::string name, lua_CFunction func);

  /// Execute a particular file.
  void do_file(FileName filename);

  /// Load a particular package, making sure it is not already loaded.
  void require(FileName filename, bool fatal = false);

  /// Sets a global to a particular integer value.
  /// If the global currently exists it will be overwritten.
  void set_global(std::string name, lua_Integer value);

  /// Sets a global to a particular string value.
  /// If the global currently exists it will be overwritten.
  void set_global(std::string name, std::string value);

  void stackDump() const;


  /// Adds a C++ enumerated type into Lua.
  /// L - Lua state.
  /// tname - The name of the enum type.
  /// <name:string><value:int> pairs, terminated by a null (0).
  ///
  /// EX: Assume the following enum in C-code:
  ///
  ///  typedef enum _TYPE { TYPE_FOO=0, TYPE_BAR, TYPE_BAZ, TYPE_MAX } TYPE;
  ///
  /// To map this to Lua, do the following:
  ///
  ///  add_enum( "type",
  ///    { { "foo", TYPE_FOO },
  ///      { "bar", TYPE_BAR },
  ///      { "baz", TYPE_BAZ } });
  ///
  /// In Lua, you can access the enum as:
  ///  type.foo
  ///  type.bar
  ///  type.baz
  ///
  /// You can print the actual value in Lua by:
  ///  > print(type.foo.value)
  ///
  template <typename T>
  bool add_enum(std::string tname, std::vector<EnumPair<T>> pairs)
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
    
    std::stringstream code;
    
    code << tname << " = setmetatable({}, {";
    code << "__index = {";
    
    // Iterate over the pairs adding the enum values.
    for (auto& pair : pairs)
    {
      code << pair.name << "={value=" << static_cast<int>(pair.value) << ",type=\"" << tname << "\"},";
    }
    
    code << "},";
    code << "__newindex = function(table, key, value) error(\"Attempt to modify read-only table\") end,";
    code << "__metatable = false});";
    
    // Execute lua code
    std::string codeString = code.str();
    
    if (luaL_loadbuffer(L_, codeString.c_str(), codeString.length(), 0) || lua_pcall(L_, 0, 0, 0))
    {
      std::string errorString{ lua_tostring(L_, -1) };
      
      CLOG(FATAL, "Lua") << "Could not add enum to Lua: " << errorString << "\nString was:\n" << codeString;
      
      // Should not actually get here due to fatal error log
      lua_pop(L_, 1);
      return false;
    }
    return true;
  }

  /// Validates that the specified value is the correct enumerated type.
  ///
  /// L - Lua state
  /// tname - The name of the expected type
  /// index - The stack index of the value to test
  ///
  /// EX: Assume you have a function in lua 'dosomething' which is called:
  ///
  ///  > dosomething(type.foo)
  ///
  /// Your C function gets called, and you use check_enum_type to verify the
  /// argument is the correct type.
  ///
  /// int lua_dosomething(lua_State* L)
  /// {
  ///   if (!check_enum_type(L, "type", -1))
  ///     /* error */
  ///   ...
  /// }
  ///
  bool check_enum_type(const char* tname, int index) const;

  /// Gets the enum value as an integer.
  ///
  /// L - Lua state
  /// index - The index on the Lua stack where the enum resides.
  ///
  /// EX:
  ///
  /// int lua_dosomething(lua_State* L)
  /// {
  ///   if (!check_enum_type(L, "type", -1))
  ///     /* error */
  ///
  ///   int value = get_enum_value(L, -1);
  ///   ...
  /// }
  ///
  int get_enum_value(int index);

  /// Pushes a JSON value onto the Lua stack.
  ///
  /// @param    value   Value to push.
  /// @return           The number of arguments pushed to the stack.
  ///                   If the type could not be deduced, it will push
  ///                   nil and return 1.
  int push_value(json value);

  int push_value(unsigned int value);
  int push_value(int value);
  int push_value(int64_t value);
  int push_value(EntityId value);
  int push_value(float value);
  int push_value(double value);
  int push_value(bool value);
  int push_value(std::string value);
  int push_value(UintVec2 value);
  int push_value(IntVec2 value);
  int push_value(RealVec2 value);
  int push_value(Direction value);
  int push_value(Color value);

  int push_array(json value);
  int push_object(json value);

  /// Pop a type from a Lua function.
  /// @return The popped type.
  Type pop_type();

  /// Pop a value from a Lua function, if possible.
  /// @return The popped value.
  json pop_value(Type type);

  /// Return the number of Lua stack slots associated with a particular value.
  unsigned int stack_slots(Type type) const;

  /// Given an entity type and a suffix, look for a Lua function name equal to
  /// "EntityType_suffix". If it doesn't exist, search the entity's templates
  /// and repeat. Do so until a matching name is found.
  /// @param type     Entity type to look for
  /// @param suffix   Suffix of the function to call
  /// @return A matching function name, or a blank string if none was found.
  std::string find_lua_function(std::string type, std::string suffix) const;

  /// Try to call a Lua function that takes the caller and a vector of
  /// arguments and returns a result.
  ///
  /// If the function does not exist, or it returns nil, the code attempts
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         EntityId of the entity calling the function
  /// @param args           Arguments to pass to the function
  /// @param default_result The default result if function is not found
  ///                       after traversing the entire parent tree.
  ///
  /// @return The result of the call.
  json callEntityFunction(std::string function_name,
                           EntityId caller,
                           json const& args,
                           json default_result);

  /// Call Lua function associated with a reflexive action.
  /// 
  /// @param subject  The subject of the action.
  /// @param action   The action to be performed.
  /// @return Bool indicating whether the action succeeded.
  bool doReflexiveAction(EntityId subject, Actions::Action& action);

  /// Call Lua action function given a subject and an object.
  /// @param subject  The subject performing the action.
  /// @param action   The action to be the target of.
  /// @param object   The object of the action.
  /// @return Bool indicating whether the action succeeded.
  bool doSubjectActionObject(EntityId subject, Actions::Action& action, EntityId object);

  /// Call Lua action function given a subject, object, and target.
  /// @param subject  The subject performing the action.
  /// @param action   The action to be the target of.
  /// @param object   The object of the action.
  /// @param target   The target of the action.
  /// @return Bool indicating whether the action succeeded.
  bool doSubjectActionObjectTarget(EntityId subject, Actions::Action& action, EntityId object, EntityId target);

  /// Call Lua action function given a subject, object, and direction.
  /// @param subject    The subject performing the action.
  /// @param action     The action to be the target of.
  /// @param object   The object of the action.
  /// @param direction  The direction of the action.
  /// @return Bool indicating whether the action succeeded.
  bool doSubjectActionObjectDirection(EntityId subject, Actions::Action& action, EntityId object, Direction direction);

  /// Call a Lua property modifier function.
  /// Calls the Lua function `GROUP:modify_property_PROP[_SFX]()` where:
  ///   * GROUP is the Lua class responsible for the modifier
  ///   * PROP is the name of the property to be affected
  ///   * SFX is an optional suffix
  ///
  /// The function takes three arguments:
  ///   * The ID of the Entity the property belongs to
  ///   * The ID of the Entity affecting the property
  ///   * The value of the property before being modified.
  ///
  /// It returns the modified value of the property.
  json callModifierFunction(std::string property_name,
                              json unmodified_value,
                              EntityId affected_id,
                              std::string responsible_group,
                              EntityId responsible_id,
                              std::string suffix = "");

  /// Return the Lua state.
  /// @todo For cleanliness, this should not be exposed; all Lua interaction
  ///       should go through the Lua class.
  lua_State* state();

  static int LUA_trace(lua_State* L);

  /// Add the "Gender" enum to the Lua instance.
  /// @see Gender
  void addGenderEnumToLua();

  /// Add the "Type" enum to the Lua instance.
  /// @see Type
  void addLuaTypeEnumToLua();

protected:
  /// Helper method for `find_lua_function`.
  std::string find_lua_function_(std::string category, std::string suffix) const;

private:
  /// Private Lua state.
  lua_State mutable* L_;
};
