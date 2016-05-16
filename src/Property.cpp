#include "Property.h"

Property::Property(double number)
{
  m_type = Type::Number;
  m_data.number = number;
}

Property::Property(float number)
{
  m_type = Type::Number;
  m_data.number = static_cast<double>(number);
}

Property::Property(int number)
{
  m_type = Type::Number;
  m_data.number = static_cast<double>(number);
}

Property::Property(unsigned int number)
{
  m_type = Type::Number;
  m_data.number = static_cast<double>(number);
}

Property::Property(std::string str)
{
  m_type = Type::String;
  m_data.str = str;
}

Property::Property(StringDisplay str)
{
  m_type = Type::String;
  m_data.str = static_cast<std::string>(str);
}

Property::Property(bool boolean)
{
  m_type = Type::Boolean;
  m_data.boolean = boolean;
}

Property::Property(sf::Color color)
{
  m_type = Type::Color;
  m_data.color = color;
}

Property::Property(sf::Vector2i vector2i)
{
  m_type = Type::Vector2i;
  m_data.vector2i = vector2i;
}

Type Property::type()
{
  return m_type;
}

std::string Property::type_name()
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

Property::operator double()
{
  if (m_type == Type::Number)
  {
    return m_data.number;
  }
  else
  {
    throw std::bad_cast();
  }

}

Property::operator float()
{
  if (m_type == Type::Number)
  {
    return static_cast<float>(m_data.number);
  }
  else
  {
    throw std::bad_cast();
  }

}

Property::operator int()
{
  if (m_type == Type::Number)
  {
    return static_cast<int>(m_data.number);
  }
  else
  {
    throw std::bad_cast();
  }

}

Property::operator unsigned int()
{
  if (m_type == Type::Number)
  {
    return static_cast<unsigned int>(m_data.number);
  }
  else
  {
    throw std::bad_cast();
  }

}

Property::operator std::string()
{
  if (m_type == Type::String)
  {
    return m_data.str;
  }
  else
  {
    throw std::bad_cast();
  }
}

Property::operator StringDisplay()
{
  if (m_type == Type::String)
  {
    return static_cast<StringDisplay>(m_data.str);
  }
  else
  {
    throw std::bad_cast();
  }
}

Property::operator bool()
{
  if (m_type == Type::Boolean)
  {
    return m_data.boolean;
  }
  else
  {
    throw std::bad_cast();
  }
}

Property::operator sf::Color()
{
  if (m_type == Type::Color)
  {
    return m_data.color;
  }
  else
  {
    throw std::bad_cast();
  }
}

Property::operator sf::Vector2i()
{
  if (m_type == Type::Vector2i)
  {
    return m_data.vector2i;
  }
  else
  {
    throw std::bad_cast();
  }
}