#include "stdafx.h"

#include "PropertyValue.h"

PropertyValue::PropertyValue(PropertyValue const & other)
{
  m_type = other.m_type;
  m_data = other.m_data;
}

PropertyValue::PropertyValue(double number)
{
  m_type = Type::Number;
  m_data = number;
}

PropertyValue::PropertyValue(float number)
{
  m_type = Type::Number;
  m_data = static_cast<double>(number);
}

PropertyValue::PropertyValue(int number)
{
  m_type = Type::Number;
  m_data = static_cast<double>(number);
}

PropertyValue::PropertyValue(unsigned int number)
{
  m_type = Type::Number;
  m_data = static_cast<double>(number);
}

PropertyValue::PropertyValue(std::string str)
{
  m_type = Type::String;
  m_data = str;
}

PropertyValue::PropertyValue(std::wstring wstr)
{
  m_type = Type::WString;
  m_data = wstr;
}

PropertyValue::PropertyValue(bool boolean)
{
  m_type = Type::Boolean;
  m_data = boolean;
}

PropertyValue::PropertyValue(sf::Color color)
{
  m_type = Type::Color;
  m_data = color;
}

PropertyValue::PropertyValue(sf::Vector2i vector2i)
{
  m_type = Type::Vector2i;
  m_data = vector2i;
}

PropertyValue::Type PropertyValue::type()
{
  return m_type;
}

std::string PropertyValue::type_name()
{
  switch (m_type)
  {
    case Type::Number: return "number";
    case Type::String: return "string";
    case Type::Boolean: return "boolean";
    case Type::Color: return "color";
    case Type::Vector2i: return "vector2i";
    default: return "???";
  }
}

PropertyValue::operator double() const
{
  double dest;
  switch (m_type)
  {
    case Type::Number:
      dest = boost::any_cast<double>(m_data);
      break;
    case Type::String:
    {
      auto src = boost::any_cast<std::string>(m_data);
      dest = boost::lexical_cast<double>(src);
      break;
    }
    case Type::WString:
    {
      auto src = boost::any_cast<std::wstring>(m_data);
      dest = boost::lexical_cast<double>(src);
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator float() const
{
  float dest;
  switch (m_type)
  {
    case Type::Number:
      dest = static_cast<float>(boost::any_cast<double>(m_data));
      break;
    case Type::String:
    {
      auto src = boost::any_cast<std::string>(m_data);
      dest = boost::lexical_cast<float>(src);
      break;
    }
    case Type::WString:
    {
      auto src = boost::any_cast<std::wstring>(m_data);
      dest = boost::lexical_cast<float>(src);
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator int() const
{
  int dest;
  switch (m_type)
  {
    case Type::Number:
      dest = static_cast<int>(boost::any_cast<double>(m_data));
      break;
    case Type::String:
    {
      auto src = boost::any_cast<std::string>(m_data);
      dest = boost::lexical_cast<int>(src);
      break;
    }
    case Type::WString:
    {
      auto src = boost::any_cast<std::wstring>(m_data);
      dest = boost::lexical_cast<int>(src);
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator unsigned int() const
{
  unsigned int dest;
  switch (m_type)
  {
    case Type::Number:
      dest = static_cast<unsigned int>(boost::any_cast<double>(m_data));
      break;
    case Type::String:
    {
      auto src = boost::any_cast<std::string>(m_data);
      dest = boost::lexical_cast<unsigned int>(src);
      break;
    }
    case Type::WString:
    {
      auto src = boost::any_cast<std::wstring>(m_data);
      dest = boost::lexical_cast<unsigned int>(src);
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator std::string() const
{
  std::string dest{};

  switch (m_type)
  {
    case Type::String:
      dest = boost::any_cast<std::string>(m_data);
      break;
    case Type::WString:
    {
      // Convert wstring to UTF-8 string
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      dest = convert.to_bytes(boost::any_cast<std::wstring>(m_data));
      break;
    }
    case Type::Number:
      dest = boost::lexical_cast<std::string>(boost::any_cast<double>(m_data));
      break;
    case Type::Boolean:
      dest = boost::any_cast<bool>(m_data) ? "true" : "false";
      break;
    case Type::Color:
    {
      auto src = boost::any_cast<sf::Color>(m_data);
      dest = "(" +
        std::to_string(src.r) + ", " +
        std::to_string(src.g) + ", " +
        std::to_string(src.b) + ", " +
        std::to_string(src.a) + ")";
      break;
    }
    case Type::Vector2i:
    {
      auto src = boost::any_cast<sf::Vector2i>(m_data);
      std::string dest = "(" +
        std::to_string(src.x) + ", " +
        std::to_string(src.y) + ")";
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator std::wstring() const
{
  std::wstring dest;
  switch (m_type)
  {
    case Type::WString:
      dest = boost::any_cast<std::wstring>(m_data);
      break;
    case Type::String:
    {
      // Convert UTF-8 string to wstring
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      dest = convert.from_bytes(boost::any_cast<std::string>(m_data));
      break;
    }
    case Type::Number:
      dest = boost::lexical_cast<std::wstring>(boost::any_cast<double>(m_data));
      break;
    case Type::Boolean:
      dest = boost::any_cast<bool>(m_data) ? L"true" : L"false";
      break;
    case Type::Color:
    {
      auto src = boost::any_cast<sf::Color>(m_data);
      dest = L"(" +
        std::to_wstring(src.r) + L", " +
        std::to_wstring(src.g) + L", " +
        std::to_wstring(src.b) + L", " +
        std::to_wstring(src.a) + L")";
      break;
    }
    case Type::Vector2i:
    {
      auto src = boost::any_cast<sf::Vector2i>(m_data);
      dest = L"(" +
        std::to_wstring(src.x) + L", " +
        std::to_wstring(src.y) + L")";
      break;
    }
    default:
      throw std::bad_cast();
  }

  return dest;
}

PropertyValue::operator bool() const
{
  /// @todo Parse string, wstring to create a boolean?
  switch (m_type)
  {
    case Type::Boolean:
      return boost::any_cast<bool>(m_data);
    default:
      throw std::bad_cast();
  }
}

PropertyValue::operator sf::Color() const
{
  /// @todo Parse string, wstring to create a color?
  switch (m_type)
  {
    case Type::Color:
      return boost::any_cast<sf::Color>(m_data);
    default:
      throw std::bad_cast();
  }
}

PropertyValue::operator sf::Vector2i() const
{
  /// @todo Parse string, wstring to create a vector?
  switch (m_type)
  {
    case Type::Vector2i:
      return boost::any_cast<sf::Vector2i>(m_data);
    default:
      throw std::bad_cast();
  }
}

