#pragma once

#include <unordered_map>
#include <string>
#include <boost/any.hpp>
#include <SFML/Graphics.hpp>

#include "actions/ActionResult.h"
#include "Direction.h"
#include "EntityId.h"

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
  /// @note If this class is changed, remember to also change the method that
  ///       adds it to Lua in LuaObject.h!
  enum class Type
  {
    Null,
    Boolean,
    String,
    EightBits,      ///< Internally represented by double
    Integer,        ///< Internally represented by double
    BigInteger,     ///< Internally represented by double
    Number,         ///< Internally represented by double
    IntegerVec2,
    EntityId,
    ActionResult,
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
      case Type::Integer: os << "Integer"; break;
      case Type::BigInteger: os << "BigInteger"; break;
      case Type::Number: os << "Number"; break;
      case Type::IntegerVec2: os << "IntegerVec2"; break;
      case Type::EntityId: os << "EntityId"; break;
      case Type::ActionResult: os << "ActionResult"; break;
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

  Property();
  explicit Property(Type type);
  explicit Property(bool value);
  explicit Property(char value);
  explicit Property(std::string value);
  explicit Property(char const* value);
  explicit Property(uint8_t value);
  explicit Property(uint32_t value);
  explicit Property(int32_t value);
  explicit Property(uint64_t value);
  explicit Property(int64_t value);
  explicit Property(float value);
  explicit Property(double value);
  explicit Property(EntityId value);
  explicit Property(ActionResult value);
  explicit Property(IntegerVec2 value);
  explicit Property(Direction value);
  explicit Property(Color value);

  Property(Property const& other) = default;
  Property(Property&& other) = default;
  Property& operator=(Property const& other) = default;
  Property& operator=(Property&& other) noexcept = default;

  ~Property();

  template <typename T> T as() const
  {
    return as(T());
  }

  bool as(bool default_value) const;
  std::string as(std::string default_value) const;
  uint8_t as(uint8_t default_value) const;
  uint32_t as(uint32_t default_value) const;
  int32_t as(int32_t default_value) const;
  uint64_t as(uint64_t default_value) const;
  int64_t as(int64_t default_value) const;
  float as(float default_value) const;
  double as(double default_value) const;
  EntityId as(EntityId default_value) const;
  ActionResult as(ActionResult default_value) const;
  IntegerVec2 as(IntegerVec2 default_value) const;
  Direction as(Direction default_value) const;
  Color as(Color default_value) const;
  
  Type type() const;

  static Property const& null();
  
  /// @warning Returns whether the property is non-null, NOT if the property 
  /// is not equal to false!
  explicit operator bool() const;

  /// @warning Returns whether the property is null, NOT if the property 
  /// is equal to false!
  bool operator!() const;

  Property& operator++();
  Property operator++(int);
  Property& operator--();
  Property operator--(int);
  Property& operator+=(Property const& rhs);
  Property& operator-=(Property const& rhs);
  Property& operator*=(Property const& rhs);
  Property& operator/=(Property const& rhs);
  Property& operator%=(Property const& rhs);

  friend std::ostream& operator<<(std::ostream os, Property const& obj)
  {
    os << obj.m_type << "(";
    switch (obj.m_type)
    {
      case Property::Type::Null: break;
      case Property::Type::Boolean: os << boost::any_cast<bool>(obj.m_value); break;
      case Property::Type::String: os << boost::any_cast<std::string>(obj.m_value); break;
      case Property::Type::EightBits: os << boost::any_cast<uint8_t>(obj.m_value); break;
      case Property::Type::Number: os << boost::any_cast<double>(obj.m_value); break;
      case Property::Type::BigInteger: os << boost::any_cast<uint64_t>(obj.m_value); break;
      case Property::Type::EntityId: os << boost::any_cast<EntityId>(obj.m_value); break;
      case Property::Type::ActionResult: os << boost::any_cast<ActionResult>(obj.m_value); break;
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
      case Property::Type::Boolean: return std::tie(lhs.m_type, *boost::any_cast<bool*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<bool*>(&rhs.m_value));
      case Property::Type::String: return std::tie(lhs.m_type, *boost::any_cast<std::string*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<std::string*>(&rhs.m_value));
      case Property::Type::EightBits: return std::tie(lhs.m_type, *boost::any_cast<uint8_t*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<uint8_t*>(&rhs.m_value));
      case Property::Type::Number: return std::tie(lhs.m_type, *boost::any_cast<double*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<double*>(&rhs.m_value));
      case Property::Type::BigInteger: return std::tie(lhs.m_type, *boost::any_cast<uint64_t*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<uint64_t*>(&rhs.m_value));
      case Property::Type::EntityId: return std::tie(lhs.m_type, *boost::any_cast<EntityId*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<EntityId*>(&rhs.m_value));
      case Property::Type::ActionResult: return std::tie(lhs.m_type, *boost::any_cast<ActionResult*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<ActionResult*>(&rhs.m_value));
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
      case Property::Type::Boolean: return boost::any_cast<bool>(lhs.m_value) == boost::any_cast<bool>(rhs.m_value);
      case Property::Type::String: return boost::any_cast<std::string>(lhs.m_value) == boost::any_cast<std::string>(rhs.m_value);
      case Property::Type::EightBits: return boost::any_cast<uint8_t>(lhs.m_value) == boost::any_cast<uint8_t>(rhs.m_value);
      case Property::Type::Number: return boost::any_cast<double>(lhs.m_value) == boost::any_cast<double>(rhs.m_value);
      case Property::Type::BigInteger: return boost::any_cast<uint64_t>(lhs.m_value) == boost::any_cast<uint64_t>(rhs.m_value);
      case Property::Type::EntityId: return boost::any_cast<EntityId>(lhs.m_value) == boost::any_cast<EntityId>(rhs.m_value);
      case Property::Type::ActionResult: return boost::any_cast<ActionResult>(lhs.m_value) == boost::any_cast<ActionResult>(rhs.m_value);
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
  Type m_type;
  boost::any m_value;
};

using PropertyMap = std::unordered_map<std::string, Property>;
using PropertyPair = std::pair<std::string, Property>;