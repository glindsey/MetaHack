#pragma once

#include <unordered_map>
#include <string>
#include <boost/any.hpp>
#include <SFML/Graphics.hpp>

#include "App.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "Vec2.h"
#include "LuaObject.h"

// Forward declarations and using statements
using Color = sf::Color;

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

  friend std::ostream& operator<<(std::ostream& os, Type const& type)
  {
    switch (type)
    {
      case Type::Null: os << "Null"; break;
      case Type::Boolean: os << "Boolean"; break;
      case Type::String: os << "String"; break;
      case Type::Integer: os << "Integer"; break;
      case Type::Float: os << "Float"; break;
      case Type::Vec2Integer: os << "Vec2Integer"; break;
      case Type::Direction: os << "Direction"; break;
      case Type::Color: os << "Color"; break;
      default: os << "??? (" << static_cast<int>(type) << ")"; break;
    }
    return os;
  }

  /// Exception thrown if an unexpected cast is made.
  class InvalidPropertyCastException : std::exception
  {
  public:
    InvalidPropertyCastException(char const* cast, Property::Type type)
    {
      std::stringstream ss;
      ss << "Attempted to cast property of type " << type << " to a " << cast;
      description = ss.str();
    }

    virtual char const* what() const override
    {
      return description.c_str();
    }

  private:
    std::string description;
  };

  /// Exception thrown if invalid operator is used.
  class InvalidPropertyOperationException : std::exception
  {
  public:
    InvalidPropertyOperationException(char const* operation, Property::Type type)
    {
      std::stringstream ss;
      ss << "Attempted to call " << operation << " on invalid type " << type;
      description = ss.str();

    }
    virtual char const* what() const override
    {
      return description.c_str();
    }

  private:
    std::string description;
  };

  /// Exception thrown if operator is used on mismatched properties.
  class MismatchedPropertyTypesException : std::exception
  {
  public:
    MismatchedPropertyTypesException(char const* operation, Property::Type left, Property::Type right)
    {
      std::stringstream ss;
      ss << "Attempted to call " << operation << " on mismatched types " << left << " and " << right;
      description = ss.str();
    }
    virtual char const* what() const override
    {
      return description.c_str();
    }

  private:
    std::string description;

  };

  Property() : m_type{ Type::Null } {}
  explicit Property(bool value) : m_type{ Type::Boolean }, m_value{ value } {}
  explicit Property(std::string value) : m_type{ Type::String }, m_value{ value } {}
  explicit Property(int32_t value) : m_type{ Type::Integer }, m_value{ value } {}
  explicit Property(double value) : m_type{ Type::Float }, m_value{ value } {}
  explicit Property(Vec2i value) : m_type{ Type::Vec2Integer }, m_value{ value } {}
  explicit Property(Direction value) : m_type{ Type::Direction }, m_value{ value } {}
  explicit Property(Color value) : m_type{ Type::Color }, m_value{ value } {}

  Property(Property const& other) = default;
  Property(Property&& other) = default;
  Property& operator=(Property const& other) = default;
  Property& operator=(Property&& other) noexcept = default;

  ~Property() {}

  template <typename T> T as() const
  {
    throw InvalidPropertyCastException();
  }

  template <> bool as() const
  {
    if (type() == Type::Boolean)
    {
      return boost::any_cast<bool>(m_value);
    }
    throw InvalidPropertyCastException("bool", type());
  }

  template <> std::string as() const
  {
    if (type() == Type::String)
    {
      return boost::any_cast<std::string>(m_value);
    }
    throw InvalidPropertyCastException("string", type());
  }

  template <> int32_t as() const
  {
    if (type() == Type::Integer)
    {
      return boost::any_cast<int32_t>(m_value);
    }
    throw InvalidPropertyCastException("int32_t", type());
  }

  template <> double as() const
  {
    if (type() == Type::Float)
    {
      return boost::any_cast<double>(m_value);
    }
    throw InvalidPropertyCastException("double", type());
  }

  template <> Vec2i as() const
  {
    if (type() == Type::Vec2Integer)
    {
      return boost::any_cast<Vec2i>(m_value);
    }
    throw InvalidPropertyCastException("Vec2i", type());
  }

  template <> Direction as() const
  {
    if (type() == Type::Direction)
    {
      return boost::any_cast<Direction>(m_value);
    }
    throw InvalidPropertyCastException("Direction", type());
  }
  
  template <> Color as() const
  {
    if (type() == Type::Color)
    {
      return boost::any_cast<Color>(m_value);
    }
    throw InvalidPropertyCastException("Color", type());
  }
  
  Type type() const
  {
    return m_type;
  }

  static void initializeIfNeeded()
  {
    if (!s_initialized)
    {
      SET_UP_LOGGER("Property", true);
    }
  }

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

  /// @warning Returns whether the property is non-null, NOT if the property 
  /// is not equal to false!
  explicit operator bool() const
  {
    return (type() != Type::Null);
  }

  /// @warning Returns whether the property is null, NOT if the property 
  /// is equal to false!
  bool operator!() const
  {
    return !(operator bool());
  }

  Property& operator++()
  {
    switch (type())
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) + 1; break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) + 1.0; break;
      default: throw InvalidPropertyOperationException("operator++", type());
    }
    return *this;
  }

  Property operator++(int)
  {
    Property tmp(*this);
    operator++();
    return tmp;
  }

  Property& operator--()
  {
    switch (m_type)
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) - 1; break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) - 1.0; break;
      default: throw InvalidPropertyOperationException("operator--", type());
    }
    return *this;
  }

  Property operator--(int)
  {
    Property tmp(*this);
    operator--();
    return tmp;
  }

  Property& operator+=(Property const& rhs)
  {
    if (type() != rhs.type())
    {
      throw MismatchedPropertyTypesException("operator+=", type(), rhs.type());
    }
    switch (type())
    {
      case Type::String: m_value = boost::any_cast<std::string>(m_value) + boost::any_cast<std::string>(rhs.m_value); break;
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) + boost::any_cast<uint32_t>(rhs.m_value); break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) + boost::any_cast<double>(rhs.m_value); break;
      case Type::Vec2Integer: m_value = boost::any_cast<Vec2i>(m_value) + boost::any_cast<Vec2i>(rhs.m_value); break;
      case Type::Color: m_value = boost::any_cast<Color>(m_value) + boost::any_cast<Color>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator+=", type());
    }
    return *this;
  }

  Property& operator-=(Property const& rhs)
  {
    if (type() != rhs.type())
    {
      throw MismatchedPropertyTypesException("operator-=", type(), rhs.type());
    }
    switch (type())
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) - boost::any_cast<uint32_t>(rhs.m_value); break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) - boost::any_cast<double>(rhs.m_value); break;
      case Type::Vec2Integer: m_value = boost::any_cast<Vec2i>(m_value) - boost::any_cast<Vec2i>(rhs.m_value); break;
      case Type::Color: m_value = boost::any_cast<Color>(m_value) - boost::any_cast<Color>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator-=", type());
    }
    return *this;
  }

  Property& operator*=(Property const& rhs)
  {
    if (type() != rhs.type())
    {
      throw MismatchedPropertyTypesException("operator*=", type(), rhs.type());
    }
    switch (type())
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) * boost::any_cast<uint32_t>(rhs.m_value); break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) * boost::any_cast<double>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator*=", type());
    }
    return *this;
  }

  Property& operator/=(Property const& rhs)
  {
    if (type() != rhs.type())
    {
      throw MismatchedPropertyTypesException("operator/=", type(), rhs.type());
    }
    switch (type())
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) / boost::any_cast<uint32_t>(rhs.m_value); break;
      case Type::Float: m_value = boost::any_cast<double>(m_value) / boost::any_cast<double>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator/=", type());
    }
    return *this;
  }

  Property& operator%=(Property const& rhs)
  {
    if (type() != rhs.type())
    {
      throw MismatchedPropertyTypesException("operator%=", type(), rhs.type());
    }
    switch (type())
    {
      case Type::Integer: m_value = boost::any_cast<uint32_t>(m_value) % boost::any_cast<uint32_t>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator%=", type());
    }
    return *this;
  }

  friend std::ostream& operator<<(std::ostream os, Property const& obj)
  {
    // TODO: write me
    os << "Property";
    return os;
  }

  friend Property operator+(Property lhs, Property const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend Property operator-(Property lhs, Property const& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend Property operator*(Property lhs, Property const& rhs)
  {
    lhs *= rhs;
    return lhs;
  }

  friend Property operator/(Property lhs, Property const& rhs)
  {
    lhs /= rhs;
    return lhs;
  }

  friend bool operator<(Property const& lhs, Property const& rhs)
  {
    switch (lhs.m_type)
    {
      case Property::Type::Null: return true;
      case Property::Type::Boolean: return std::tie(lhs.m_type, *boost::any_cast<bool*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<bool*>(&rhs.m_value));
      case Property::Type::String: return std::tie(lhs.m_type, *boost::any_cast<std::string*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<std::string*>(&rhs.m_value));
      case Property::Type::Integer: return std::tie(lhs.m_type, *boost::any_cast<uint32_t*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<uint32_t*>(&rhs.m_value));
      case Property::Type::Float: return std::tie(lhs.m_type, *boost::any_cast<double*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<double*>(&rhs.m_value));
      case Property::Type::Vec2Integer: return std::tie(lhs.m_type, *boost::any_cast<Vec2i*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Vec2i*>(&rhs.m_value));
      case Property::Type::Direction: return std::tie(lhs.m_type, *boost::any_cast<Direction*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Direction*>(&rhs.m_value));
      case Property::Type::Color: return std::tie(lhs.m_type, *boost::any_cast<Color*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Color*>(&rhs.m_value));
      default: throw Property::InvalidPropertyOperationException("operator<", lhs.m_type);
    }
  }

  friend bool operator>(Property const& lhs, Property const& rhs)
  {
    return rhs < lhs;
  }

  friend bool operator<=(Property const& lhs, Property const& rhs)
  {
    return !(lhs > rhs);
  }

  friend bool operator>=(Property const& lhs, Property const& rhs)
  {
    return !(lhs < rhs);
  }

  friend bool operator==(Property const& lhs, Property const& rhs)
  {
    if (lhs.m_type != rhs.m_type) return false;
    switch (lhs.m_type)
    {
      case Property::Type::Null: return true;
      case Property::Type::Boolean: return boost::any_cast<bool>(lhs.m_value) == boost::any_cast<bool>(rhs.m_value);
      case Property::Type::String: return boost::any_cast<std::string>(lhs.m_value) == boost::any_cast<std::string>(rhs.m_value);
      case Property::Type::Integer: return boost::any_cast<uint32_t>(lhs.m_value) == boost::any_cast<uint32_t>(rhs.m_value);
      case Property::Type::Float: return boost::any_cast<double>(lhs.m_value) == boost::any_cast<double>(rhs.m_value);
      case Property::Type::Vec2Integer: return boost::any_cast<Vec2i>(lhs.m_value) == boost::any_cast<Vec2i>(rhs.m_value);
      case Property::Type::Direction: return boost::any_cast<Direction>(lhs.m_value) == boost::any_cast<Direction>(rhs.m_value);
      case Property::Type::Color: return boost::any_cast<Color>(lhs.m_value) == boost::any_cast<Color>(rhs.m_value);
      default: throw Property::InvalidPropertyOperationException("operator==", lhs.m_type);
    }
  }

  friend bool operator!=(Property const& lhs, Property const& rhs)
  {
    return !(lhs == rhs);
  }

private:
  static bool s_initialized;
  Type m_type;
  boost::any m_value;
};

using PropertyMap = std::unordered_map<std::string, Property>;