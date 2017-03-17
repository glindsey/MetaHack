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

// Forward declarations and using statement
using Boolean = bool;
using String = std::string;
using EightBits = uint8_t;
using Index = uint32_t;
using Integer = int32_t;
using BigInteger = int64_t;
using Real = float;
using Number = double;
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
    EightBits,
    Number,
    BigInteger,
    IntegerVec2,
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
      case Type::EightBits: os << "EightBits"; break;
      case Type::Number: os << "Number"; break;
      case Type::BigInteger: os << "BigInteger"; break;
      case Type::IntegerVec2: os << "IntegerVec2"; break;
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
  explicit Property(Boolean value) : m_type{ Type::Boolean }, m_value{ value } {}
  explicit Property(String value) : m_type{ Type::String }, m_value{ value } {}
  explicit Property(char const* value) : m_type{ Type::String }, m_value{ std::string(value) } {}
  explicit Property(EightBits value) : m_type{ Type::EightBits }, m_value{ value } {}
  explicit Property(Index value) : m_type{ Type::Number }, m_value{ static_cast<Number>(value) } {}
  explicit Property(Integer value) : m_type{ Type::Number }, m_value{ static_cast<Number>(value) } {}
  explicit Property(BigInteger value) : m_type{ Type::BigInteger }, m_value{ value } {}
  explicit Property(Real value) : m_type{ Type::Number }, m_value{ static_cast<Number>(value) } {}
  explicit Property(IntegerVec2 value) : m_type{ Type::IntegerVec2 }, m_value{ value } {}
  explicit Property(Direction value) : m_type{ Type::Direction }, m_value{ value } {}
  explicit Property(Color value) : m_type{ Type::Color }, m_value{ value } {}

  Property(Property const& other) = default;
  Property(Property&& other) = default;
  Property& operator=(Property const& other) = default;
  Property& operator=(Property&& other) noexcept = default;

  ~Property() {}

  template <typename T> T as() const
  {
    return as(T());
  }

  template <typename T> T as(T default_value) const
  {
    throw InvalidPropertyCastException("(unknown)", type());
  }

  template <> Boolean as(Boolean default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Boolean: return boost::any_cast<Boolean>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("Boolean", type());
  }

  template <> String as(String default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::String: return boost::any_cast<String>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("String", type());
  }

  template <> EightBits as(EightBits default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::EightBits: return boost::any_cast<EightBits>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("EightBits", type());
  }

  template <> Index as(Index default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Number: return static_cast<Index>(boost::any_cast<Number>(m_value));
      default: break;
    }
    throw InvalidPropertyCastException("Index", type());
  }

  template <> Integer as(Integer default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Number: return static_cast<Integer>(boost::any_cast<Number>(m_value));
      default: break;
    }
    throw InvalidPropertyCastException("Integer", type());
  }

  template <> BigInteger as(BigInteger default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::BigInteger: return boost::any_cast<BigInteger>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("BigInteger", type());
  }

  template <> Real as(Real default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Number: return static_cast<Real>(boost::any_cast<Number>(m_value));
      default: break;
    }
    throw InvalidPropertyCastException("Real", type());
  }

  template <> IntegerVec2 as(IntegerVec2 default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::IntegerVec2: return boost::any_cast<IntegerVec2>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("IntegerVec2", type());
  }

  template <> Direction as(Direction default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Direction: return boost::any_cast<Direction>(m_value);
      default: break;
    }
    throw InvalidPropertyCastException("Direction", type());
  }
  
  template <> Color as(Color default_value) const
  {
    switch (type())
    {
      case Type::Null: return default_value;
      case Type::Color: return boost::any_cast<Color>(m_value);
      default: break;
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

  static Property const& null()
  {
    static Property null_property{};
    return null_property;
  }

  static void addTypeEnumToLua(Lua* lua_instance)
  {
    lua_instance->add_enum("PropertyType",
                           "Null", Type::Null,
                           "Boolean", Type::Boolean,
                           "String", Type::String,
                           "EightBits", Type::EightBits,
                           "Number", Type::Number,
                           "BigInteger", Type::BigInteger,
                           "IntegerVec2", Type::IntegerVec2,
                           "Direction", Type::Direction,
                           "Color", Type::Color,
                           0
    );
  }

  /// @warning Returns whether the property is non-null, NOT if the property 
  /// is not equal to false!
  explicit operator Boolean() const
  {
    return (type() != Type::Null);
  }

  /// @warning Returns whether the property is null, NOT if the property 
  /// is equal to false!
  bool operator!() const
  {
    return !(operator Boolean());
  }

  Property& operator++()
  {
    switch (type())
    { 
      case Type::EightBits: m_value = boost::any_cast<EightBits>(m_value) + static_cast<EightBits>(1); break;
      case Type::Number: m_value = boost::any_cast<Number>(m_value) + 1.0; break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) + 1L; break;
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
      case Type::EightBits: m_value = boost::any_cast<EightBits>(m_value) - static_cast<EightBits>(1); break;
      case Type::Number: m_value = boost::any_cast<Index>(m_value) - 1.0; break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) - 1L; break;
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
      case Type::String: m_value = boost::any_cast<String>(m_value) + boost::any_cast<String>(rhs.m_value); break;
      case Type::EightBits: m_value = boost::any_cast<EightBits>(m_value) + boost::any_cast<EightBits>(rhs.m_value); break;
      case Type::Number: m_value = boost::any_cast<Number>(m_value) + boost::any_cast<Number>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) + boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::IntegerVec2: m_value = boost::any_cast<IntegerVec2>(m_value) + boost::any_cast<IntegerVec2>(rhs.m_value); break;
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
      case Type::EightBits: m_value = boost::any_cast<EightBits>(m_value) - boost::any_cast<EightBits>(rhs.m_value); break;
      case Type::Number: m_value = boost::any_cast<Index>(m_value) - boost::any_cast<Number>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) - boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::IntegerVec2: m_value = boost::any_cast<IntegerVec2>(m_value) - boost::any_cast<IntegerVec2>(rhs.m_value); break;
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
      case Type::Number: m_value = boost::any_cast<Number>(m_value) * boost::any_cast<Number>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) * boost::any_cast<BigInteger>(rhs.m_value); break;
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
      case Type::Number: m_value = boost::any_cast<Number>(m_value) / boost::any_cast<Number>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) / boost::any_cast<BigInteger>(rhs.m_value); break;
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
      case Type::EightBits: m_value = boost::any_cast<EightBits>(m_value) % boost::any_cast<EightBits>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) % boost::any_cast<BigInteger>(rhs.m_value); break;
      default: throw InvalidPropertyOperationException("operator%=", type());
    }
    return *this;
  }

  friend std::ostream& operator<<(std::ostream os, Property const& obj)
  {
    os << obj.m_type << "(";
    switch (obj.m_type)
    {
      case Property::Type::Null: break;
      case Property::Type::Boolean: os << boost::any_cast<Boolean>(obj.m_value); break;
      case Property::Type::String: os << boost::any_cast<String>(obj.m_value); break;
      case Property::Type::EightBits: os << boost::any_cast<EightBits>(obj.m_value); break;
      case Property::Type::Number: os << boost::any_cast<Number>(obj.m_value); break;
      case Property::Type::BigInteger: os << boost::any_cast<BigInteger>(obj.m_value); break;
      case Property::Type::IntegerVec2: os << boost::any_cast<IntegerVec2>(obj.m_value); break;
      case Property::Type::Direction: os << boost::any_cast<Direction>(obj.m_value); break;
      case Property::Type::Color: os << boost::any_cast<Color>(obj.m_value); break;
      default: os << "???"; break;
    }
    os << ")";
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
      case Property::Type::Boolean: return std::tie(lhs.m_type, *boost::any_cast<Boolean*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Boolean*>(&rhs.m_value));
      case Property::Type::String: return std::tie(lhs.m_type, *boost::any_cast<String*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<String*>(&rhs.m_value));
      case Property::Type::EightBits: return std::tie(lhs.m_type, *boost::any_cast<EightBits*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<EightBits*>(&rhs.m_value));
      case Property::Type::Number: return std::tie(lhs.m_type, *boost::any_cast<Number*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Number*>(&rhs.m_value));
      case Property::Type::BigInteger: return std::tie(lhs.m_type, *boost::any_cast<BigInteger*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<BigInteger*>(&rhs.m_value));
      case Property::Type::IntegerVec2: return std::tie(lhs.m_type, *boost::any_cast<IntegerVec2*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<IntegerVec2*>(&rhs.m_value));
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
      case Property::Type::Boolean: return boost::any_cast<Boolean>(lhs.m_value) == boost::any_cast<Boolean>(rhs.m_value);
      case Property::Type::String: return boost::any_cast<String>(lhs.m_value) == boost::any_cast<String>(rhs.m_value);
      case Property::Type::EightBits: return boost::any_cast<EightBits>(lhs.m_value) == boost::any_cast<EightBits>(rhs.m_value);
      case Property::Type::Number: return boost::any_cast<Number>(lhs.m_value) == boost::any_cast<Number>(rhs.m_value);
      case Property::Type::BigInteger: return boost::any_cast<BigInteger>(lhs.m_value) == boost::any_cast<BigInteger>(rhs.m_value);
      case Property::Type::IntegerVec2: return boost::any_cast<IntegerVec2>(lhs.m_value) == boost::any_cast<IntegerVec2>(rhs.m_value);
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
using PropertyPair = std::pair<std::string, Property>;