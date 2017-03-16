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
using Real = double;
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
    Index,
    Integer,
    BigInteger,
    Real,
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
      case Type::Index: os << "Index"; break;
      case Type::Integer: os << "Integer"; break;
      case Type::BigInteger: os << "BigInteger"; break;
      case Type::Real: os << "Real"; break;
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
  explicit Property(EightBits value) : m_type{ Type::EightBits }, m_value{ value } {}
  explicit Property(Index value) : m_type{ Type::Index }, m_value{ value } {}
  explicit Property(Integer value) : m_type{ Type::Integer }, m_value{ value } {}
  explicit Property(BigInteger value) : m_type{ Type::BigInteger }, m_value{ value } {}
  explicit Property(Real value) : m_type{ Type::Real }, m_value{ value } {}
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
    throw InvalidPropertyCastException();
  }

  template <> Boolean as() const
  {
    if (type() == Type::Boolean)
    {
      return boost::any_cast<Boolean>(m_value);
    }
    throw InvalidPropertyCastException("Boolean", type());
  }

  template <> String as() const
  {
    if (type() == Type::String)
    {
      return boost::any_cast<String>(m_value);
    }
    throw InvalidPropertyCastException("String", type());
  }

  template <> EightBits as() const
  {
    if (type() == Type::EightBits)
    {
      return boost::any_cast<EightBits>(m_value);
    }
    throw InvalidPropertyCastException("EightBits", type());
  }

  template <> Index as() const
  {
    if (type() == Type::Index)
    {
      return boost::any_cast<Index>(m_value);
    }
    throw InvalidPropertyCastException("Index", type());
  }


  template <> Integer as() const
  {
    if (type() == Type::Integer)
    {
      return boost::any_cast<Integer>(m_value);
    }
    throw InvalidPropertyCastException("Integer", type());
  }

  template <> BigInteger as() const
  {
    if (type() == Type::BigInteger)
    {
      return boost::any_cast<BigInteger>(m_value);
    }
    throw InvalidPropertyCastException("BigInteger", type());
  }

  template <> Real as() const
  {
    if (type() == Type::Real)
    {
      return boost::any_cast<Real>(m_value);
    }
    throw InvalidPropertyCastException("Real", type());
  }

  template <> IntegerVec2 as() const
  {
    if (type() == Type::IntegerVec2)
    {
      return boost::any_cast<IntegerVec2>(m_value);
    }
    throw InvalidPropertyCastException("IntegerVec2", type());
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
                           "EightBits", Type::EightBits,
                           "Index", Type::Index,
                           "Integer", Type::Integer,
                           "BigInteger", Type::BigInteger,
                           "Real", Type::Real,
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
      case Type::Index: m_value = boost::any_cast<Index>(m_value) + 1U; break;
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) + 1; break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) + 1L; break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) + 1.0; break;
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
      case Type::Index: m_value = boost::any_cast<Index>(m_value) - 1U; break;
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) - 1; break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) - 1L; break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) - 1.0; break;
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
      case Type::Index: m_value = boost::any_cast<Index>(m_value) + boost::any_cast<Index>(rhs.m_value); break;
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) + boost::any_cast<Integer>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) + boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) + boost::any_cast<Real>(rhs.m_value); break;
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
      case Type::Index: m_value = boost::any_cast<Index>(m_value) - boost::any_cast<Index>(rhs.m_value); break;
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) - boost::any_cast<Integer>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) - boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) - boost::any_cast<Real>(rhs.m_value); break;
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
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) * boost::any_cast<Integer>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) * boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) * boost::any_cast<Real>(rhs.m_value); break;
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
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) / boost::any_cast<Integer>(rhs.m_value); break;
      case Type::BigInteger: m_value = boost::any_cast<BigInteger>(m_value) / boost::any_cast<BigInteger>(rhs.m_value); break;
      case Type::Real: m_value = boost::any_cast<Real>(m_value) / boost::any_cast<Real>(rhs.m_value); break;
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
      case Type::Index: m_value = boost::any_cast<Index>(m_value) % boost::any_cast<Index>(rhs.m_value); break;
      case Type::Integer: m_value = boost::any_cast<Integer>(m_value) % boost::any_cast<Integer>(rhs.m_value); break;
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
      case Property::Type::Index: os << boost::any_cast<Index>(obj.m_value); break;
      case Property::Type::Integer: os << boost::any_cast<Integer>(obj.m_value); break;
      case Property::Type::BigInteger: os << boost::any_cast<BigInteger>(obj.m_value); break;
      case Property::Type::Real: os << boost::any_cast<Real>(obj.m_value); break;
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
      case Property::Type::Index: return std::tie(lhs.m_type, *boost::any_cast<Index*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Index*>(&rhs.m_value));
      case Property::Type::Integer: return std::tie(lhs.m_type, *boost::any_cast<Integer*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Integer*>(&rhs.m_value));
      case Property::Type::BigInteger: return std::tie(lhs.m_type, *boost::any_cast<BigInteger*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<BigInteger*>(&rhs.m_value));
      case Property::Type::Real: return std::tie(lhs.m_type, *boost::any_cast<Real*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Real*>(&rhs.m_value));
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
      case Property::Type::Index: return boost::any_cast<Index>(lhs.m_value) == boost::any_cast<Index>(rhs.m_value);
      case Property::Type::Integer: return boost::any_cast<Integer>(lhs.m_value) == boost::any_cast<Integer>(rhs.m_value);
      case Property::Type::BigInteger: return boost::any_cast<BigInteger>(lhs.m_value) == boost::any_cast<BigInteger>(rhs.m_value);
      case Property::Type::Real: return boost::any_cast<Real>(lhs.m_value) == boost::any_cast<Real>(rhs.m_value);
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