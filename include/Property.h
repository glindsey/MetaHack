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
  ///       adds it to Lua in LuaObject.cpp!
  enum class Type
  {
    Null = 0,
    Boolean,
    String,
    Integer,        ///< Internally represented by int64_t
    Number,         ///< Internally represented by double
    IntVec2,
    ActionResult,
    Direction,
    Color,
    PropertyType,   ///< Property::Type representing a Property::Type. How meta.
    Unknown,        ///< Internally represented by a string, but only to contain the unknown Lua type
    Count           ///< Just for bookkeeping, doesn't need to be in LuaObject
  };

  friend std::ostream& operator<<(std::ostream& os, Type const& type)
  {
    switch (type)
    {
      case Type::Null:         os << "Null"; break;
      case Type::Boolean:      os << "Boolean"; break;
      case Type::String:       os << "String"; break;
      case Type::Integer:      os << "Integer"; break;
      case Type::Number:       os << "Number"; break;
      case Type::IntVec2:      os << "IntVec2"; break;
      case Type::ActionResult: os << "ActionResult"; break;
      case Type::Direction:    os << "Direction"; break;
      case Type::Color:        os << "Color"; break;
      case Type::PropertyType: os << "PropertyType"; break;
      case Type::Unknown:      os << "Unknown"; break;
      default:                 os << "??? (" << static_cast<int>(type) << ")"; break;
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
  Property(Property const& other) = default;
  Property(Property&& other) = default;
  Property& operator=(Property const& other) = default;
  Property& operator=(Property&& other) noexcept = default;

  ~Property();

  static Property empty(Type type);

  static Property from(bool value);
  static Property from(char value);
  static Property from(std::string value);
  static Property from(char const* value);
  static Property from(uint8_t value);
  static Property from(uint32_t value);
  static Property from(int32_t value);
  static Property from(uint64_t value);
  static Property from(int64_t value);
  static Property from(EntityId value);
  static Property from(float value);
  static Property from(double value);
  static Property from(ActionResult value);
  static Property from(UintVec2 value);
  static Property from(IntVec2 value);
  static Property from(Direction value);
  static Property from(Color value);
  static Property from(Property::Type value);

  template <typename T> 
  T as() const 
  { 
    return as(T()); 
  }

  template <typename T> 
  T as(T default_value) const
  {
    throw InvalidPropertyCastException("(unknown)", type());
  }

  template <> bool           as(bool           default_value) const { return as_bool(default_value); }
  template <> std::string    as(std::string    default_value) const { return as_string(default_value); }
  template <> uint8_t        as(uint8_t        default_value) const { return as_uint8_t(default_value); }
  template <> uint32_t       as(uint32_t       default_value) const { return as_uint32_t(default_value); }
  template <> int32_t        as(int32_t        default_value) const { return as_int32_t(default_value); }
  template <> uint64_t       as(uint64_t       default_value) const { return as_uint64_t(default_value); }
  template <> int64_t        as(int64_t        default_value) const { return as_int64_t(default_value); }
  template <> float          as(float          default_value) const { return as_float(default_value); }
  template <> double         as(double         default_value) const { return as_double(default_value); }
  template <> EntityId       as(EntityId       default_value) const { return as_EntityId(default_value); }
  template <> ActionResult   as(ActionResult   default_value) const { return as_ActionResult(default_value); }
  template <> UintVec2       as(UintVec2       default_value) const { return as_UintVec2(default_value); }
  template <> IntVec2        as(IntVec2        default_value) const { return as_IntVec2(default_value); }
  template <> Direction      as(Direction      default_value) const { return as_Direction(default_value); }
  template <> Color          as(Color          default_value) const { return as_Color(default_value); }
  template <> Property::Type as(Property::Type default_value) const { return as_PropertyType(default_value); }
  
  bool           as_bool        (bool           default_value) const;
  std::string    as_string      (std::string    default_value) const;
  uint8_t        as_uint8_t     (uint8_t        default_value) const;
  uint32_t       as_uint32_t    (uint32_t       default_value) const;
  int32_t        as_int32_t     (int32_t        default_value) const;
  uint64_t       as_uint64_t    (uint64_t       default_value) const;
  int64_t        as_int64_t     (int64_t        default_value) const;
  float          as_float       (float          default_value) const;
  double         as_double      (double         default_value) const;
  EntityId       as_EntityId    (EntityId       default_value) const;
  ActionResult   as_ActionResult(ActionResult   default_value) const;
  UintVec2       as_UintVec2    (UintVec2       default_value) const;
  IntVec2        as_IntVec2     (IntVec2        default_value) const;
  Direction      as_Direction   (Direction      default_value) const;
  Color          as_Color       (Color          default_value) const;
  Property::Type as_PropertyType(Property::Type default_value) const;

  Type type() const;

  static Property const& null();
  
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
      case Property::Type::Null:         break;
      case Property::Type::Boolean:      os << boost::any_cast<bool>(obj.m_value); break;
      case Property::Type::String:       os << boost::any_cast<std::string>(obj.m_value); break;
      case Property::Type::Integer:      os << boost::any_cast<int64_t>(obj.m_value); break;
      case Property::Type::Number:       os << boost::any_cast<double>(obj.m_value); break;
      case Property::Type::ActionResult: os << boost::any_cast<ActionResult>(obj.m_value); break;
      case Property::Type::IntVec2:      os << boost::any_cast<IntVec2>(obj.m_value); break;
      case Property::Type::Direction:    os << boost::any_cast<Direction>(obj.m_value); break;
      case Property::Type::Color:        os << boost::any_cast<Color>(obj.m_value); break;
      case Property::Type::PropertyType: os << boost::any_cast<Property::Type>(obj.m_value); break;
      case Property::Type::Unknown:      os << boost::any_cast<std::string>(obj.m_value); break;
      default:                           os << "???"; break;
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
      case Property::Type::Null:         return true;
      case Property::Type::Boolean:      return std::tie(lhs.m_type, *boost::any_cast<bool*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<bool*>(&rhs.m_value));
      case Property::Type::String:       return std::tie(lhs.m_type, *boost::any_cast<std::string*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<std::string*>(&rhs.m_value));
      case Property::Type::Integer:      return std::tie(lhs.m_type, *boost::any_cast<int64_t*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<int64_t*>(&rhs.m_value));
      case Property::Type::Number:       return std::tie(lhs.m_type, *boost::any_cast<double*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<double*>(&rhs.m_value));
      case Property::Type::ActionResult: return std::tie(lhs.m_type, *boost::any_cast<ActionResult*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<ActionResult*>(&rhs.m_value));
      case Property::Type::IntVec2:      return std::tie(lhs.m_type, *boost::any_cast<IntVec2*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<IntVec2*>(&rhs.m_value));
      case Property::Type::Direction:    return std::tie(lhs.m_type, *boost::any_cast<Direction*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Direction*>(&rhs.m_value));
      case Property::Type::Color:        return std::tie(lhs.m_type, *boost::any_cast<Color*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Color*>(&rhs.m_value));
      case Property::Type::PropertyType: return std::tie(lhs.m_type, *boost::any_cast<Property::Type*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<Property::Type*>(&rhs.m_value));
      case Property::Type::Unknown:      return std::tie(lhs.m_type, *boost::any_cast<std::string*>(&lhs.m_value)) < std::tie(rhs.m_type, *boost::any_cast<std::string*>(&rhs.m_value));
      default: 
        throw Property::InvalidPropertyOperationException("operator<", lhs.m_type);
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
      case Property::Type::Null:         return true;
      case Property::Type::Boolean:      return boost::any_cast<bool>(lhs.m_value) == boost::any_cast<bool>(rhs.m_value);
      case Property::Type::String:       return boost::any_cast<std::string>(lhs.m_value) == boost::any_cast<std::string>(rhs.m_value);
      case Property::Type::Integer:      return boost::any_cast<int64_t>(lhs.m_value) == boost::any_cast<int64_t>(rhs.m_value);
      case Property::Type::Number:       return boost::any_cast<double>(lhs.m_value) == boost::any_cast<double>(rhs.m_value);
      case Property::Type::ActionResult: return boost::any_cast<ActionResult>(lhs.m_value) == boost::any_cast<ActionResult>(rhs.m_value);
      case Property::Type::IntVec2:      return boost::any_cast<IntVec2>(lhs.m_value) == boost::any_cast<IntVec2>(rhs.m_value);
      case Property::Type::Direction:    return boost::any_cast<Direction>(lhs.m_value) == boost::any_cast<Direction>(rhs.m_value);
      case Property::Type::Color:        return boost::any_cast<Color>(lhs.m_value) == boost::any_cast<Color>(rhs.m_value);
      case Property::Type::PropertyType: return boost::any_cast<Property::Type>(lhs.m_value) == boost::any_cast<Property::Type>(rhs.m_value);
      case Property::Type::Unknown:      return boost::any_cast<std::string>(lhs.m_value) == boost::any_cast<std::string>(rhs.m_value);
      default:
        throw Property::InvalidPropertyOperationException("operator==", lhs.m_type);
    }
  }

  friend bool operator!=(Property const& lhs, Property const& rhs)
  {
    return !(lhs == rhs);
  }

protected:
  Property(Property::Type type, boost::any value);

private:
  Type m_type;
  boost::any m_value;
};

using PropertyMap = std::unordered_map<std::string, Property>;
using PropertyPair = std::pair<std::string, Property>;