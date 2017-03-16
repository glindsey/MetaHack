#pragma once

#include <string>
#include <boost/any.hpp>
#include <SFML/Graphics.hpp>

#include "LuaObject.h"

// Forward declarations and using statements
class Direction;
template < typename T > class Vec2;
using Color = sf::Color;
using Vec2i = Vec2<int>;

/// Exception thrown if an unexpected cast is made.
class InvalidPropertyCastException : std::exception
{
  virtual const char* what() const override
  {
    return "Attempted to cast property to invalid type";
  }
};

/// Second attempt to create a Property class to represent, well, a property.
/// This one will be not quite as ambitious as my last refactor attempt.
/// This class duplicates some of the functionality already present in
/// the boost::any member inside it, with regards to type checking... but I
/// wanted to expose a generic API that isn't bound to any specific
/// implementation here. This way I can later swap out the boost::any with
/// a boost::variant, or a union, or whatever, if I see fit.
/// Plus, the explicit type enum makes exposing information to the Lua
/// interpreter that much easier.
class Property final // final because it doesn't have a virtual dtor, don't want inheritance
{
public:
  /// Enum of the valid property types.
  enum class Type
  {
    Null,
    Boolean,
    String,
    Integer,
    Float,
    Vec2Integer,
    Direction,
    Color
  };

  Property();
  explicit Property(bool value);
  explicit Property(std::string value);
  explicit Property(int32_t value);
  explicit Property(double value);
  explicit Property(Vec2i value);
  explicit Property(Direction value);
  explicit Property(Color value);
  ~Property();

  template <typename T> T as()
  {
    throw InvalidPropertyCastException();
  }

  template <> bool as();
  template <> std::string as();
  template <> int32_t as();
  template <> double as();
  template <> Vec2i as();
  template <> Direction as();
  template <> Color as();
  
  Type type();

  static void initializeIfNeeded();

  static void addTypeEnumToLua(Lua* lua_instance)
  {
    lua_instance->add_enum("PropertyType",
                           "Null", Type::Null,
                           "Boolean", Type::Boolean,
                           "String", Type::String,
                           "Integer", Type::Integer,
                           "Float", Type::Float,
                           "Vec2Integer", Type::Vec2Integer,
                           "Direction", Type::Direction,
                           "Color", Type::Color,
                           0
    );
  }

private:
  static bool s_initialized;
  Type m_type;
  boost::any m_value;
};