#pragma once

#include "stdafx.h"

#include "common_types.h"

/// Definition of a PropertyValue within the system.
/// Wrapper around boost::any, basically.
/// A separate "type" enum is used to speed up conversions between types;
/// otherwise we'd have to do string comparisons on typeid() results.
class PropertyValue
{
public:
  enum class Type
  {
    Number,
    String,
    WString,
    Boolean,
    Color,
    Vector2i
  };

  PropertyValue(PropertyValue const& other);
  PropertyValue(double number);
  PropertyValue(float number);
  PropertyValue(int number);
  PropertyValue(unsigned int number);
  PropertyValue(std::string str);
  PropertyValue(std::wstring wstr);
  PropertyValue(bool boolean);
  PropertyValue(sf::Color color);
  PropertyValue(sf::Vector2i vector2i);

  PropertyValue& operator=(PropertyValue other)
  {
    std::swap(*this, other);
    return *this;
  }
  
  Type type();
  std::string type_name();

  operator double() const;
  operator float() const;
  operator int() const;
  operator unsigned int() const;
  operator std::string() const;
  operator std::wstring() const;
  operator bool() const;
  operator sf::Color() const;
  operator sf::Vector2i() const;

  friend bool operator==(PropertyValue const& lhs, PropertyValue const& rhs)
  {
    if (lhs.m_type != rhs.m_type)
    {
      return false;
    }

    switch (lhs.m_type)
    {
      case Type::Number:
      {
        auto left = boost::any_cast<double>(lhs.m_data);
        auto right = boost::any_cast<double>(rhs.m_data);
        return (left == right);
      }
      case Type::Boolean:
      {
        auto left = boost::any_cast<bool>(lhs.m_data);
        auto right = boost::any_cast<bool>(rhs.m_data);
        return (left == right);
      }
      case Type::String:
      {
        auto left = boost::any_cast<std::string>(lhs.m_data);
        auto right = boost::any_cast<std::string>(rhs.m_data);
        return (left == right);
      }
      case Type::WString:
      {
        auto left = boost::any_cast<std::wstring>(lhs.m_data);
        auto right = boost::any_cast<std::wstring>(rhs.m_data);
        return (left == right);
      }
      case Type::Color:
      {
        auto left = boost::any_cast<sf::Color>(lhs.m_data);
        auto right = boost::any_cast<sf::Color>(rhs.m_data);
        return (left == right);
      }
      case Type::Vector2i:
      {
        auto left = boost::any_cast<sf::Vector2i>(lhs.m_data);
        auto right = boost::any_cast<sf::Vector2i>(rhs.m_data);
        return (left == right);
      }
      default:
        return false;
    }
  }

  friend bool operator!=(PropertyValue const& lhs, PropertyValue const& rhs)
  {
    return !(lhs == rhs);
  }

  friend bool operator<(PropertyValue const & lhs, PropertyValue const & rhs)
  {
    if (lhs.m_type != rhs.m_type)
    {
      return false;
    }
    switch (lhs.m_type)
    {
      case Type::Number:
      {
        auto left = boost::any_cast<double>(lhs.m_data);
        auto right = boost::any_cast<double>(rhs.m_data);
        return left < right;
      }
      case Type::Boolean:
      {
        auto left = boost::any_cast<bool>(lhs.m_data);
        auto right = boost::any_cast<bool>(rhs.m_data);
        return (!left && right);
      }
      case Type::String:
      {
        auto left = boost::any_cast<std::string>(lhs.m_data);
        auto right = boost::any_cast<std::string>(rhs.m_data);
        return std::lexicographical_compare<std::string::iterator, std::string::iterator>(left.begin(), left.end(), right.begin(), right.end());
      }
      case Type::WString:
      {
        auto left = boost::any_cast<std::wstring>(lhs.m_data);
        auto right = boost::any_cast<std::wstring>(rhs.m_data);
        return std::lexicographical_compare<std::wstring::iterator, std::wstring::iterator>(left.begin(), left.end(), right.begin(), right.end());
      }
      case Type::Color:
      {
        auto left = boost::any_cast<sf::Color>(lhs.m_data);
        auto right = boost::any_cast<sf::Color>(rhs.m_data);
        uint32_t leftbits =
          static_cast<uint32_t>(left.r) << 24 |
          static_cast<uint32_t>(left.g) << 16 |
          static_cast<uint32_t>(left.b) << 8 |
          static_cast<uint32_t>(left.a);
        uint32_t rightbits =
          static_cast<uint32_t>(right.r) << 24 |
          static_cast<uint32_t>(right.g) << 16 |
          static_cast<uint32_t>(right.b) << 8 |
          static_cast<uint32_t>(right.a);
        return (leftbits < rightbits);
      }
      case Type::Vector2i:
      {
        auto left = boost::any_cast<sf::Vector2i>(lhs.m_data);
        auto right = boost::any_cast<sf::Vector2i>(rhs.m_data);
        return ((left.x * left.x) + (left.y * left.y)) < ((right.x * right.x) + (right.y * right.y));
      }
      default:
        return false;
    }
  }

  friend bool operator>(PropertyValue const& lhs, PropertyValue const& rhs)
  {
    return (rhs < lhs);
  }

  friend bool operator<=(PropertyValue const& lhs, PropertyValue const& rhs)
  {
    return !(lhs > rhs);
  }

  friend bool operator>=(PropertyValue const& lhs, PropertyValue const& rhs)
  {
    return !(lhs < rhs);
  }

  friend std::ostream& operator<<(std::ostream& out, PropertyValue const& prop)
  {
    return out << static_cast<std::string>(prop);
  }

protected:
private:

  /// Type of this PropertyValue.
  Type m_type;

  /// Data for this PropertyValue.
  boost::any m_data;

};