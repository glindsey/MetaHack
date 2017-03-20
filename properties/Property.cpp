#include "stdafx.h"

#include "properties/Property.h"

Property::Property() : m_type{ Type::Null } {}
Property::~Property() {}

Property Property::empty(Type type)
{
  switch (type)
  {
    case Type::Boolean:      return Property::from(false);
    case Type::String:       return Property::from(std::string());
    case Type::Integer:      return Property::from(0LL);
    case Type::Number:       return Property::from(0.0);
    case Type::ActionResult: return Property::from(ActionResult::Pending);
    case Type::IntVec2:      return Property::from(IntVec2(0, 0));
    case Type::Direction:    return Property::from(Direction::None);
    case Type::Color:        return Property::from(Color::Transparent);
    case Type::PropertyType: return Property::from(Property::Type::Null);
    default: return Property();
  }
}

Property Property::from(bool value) { return Property(Type::Boolean, value); }
Property Property::from(char value) { return Property(Type::String, std::string(1, value)); }
Property Property::from(std::string value) { return Property(Type::String, value); }
Property Property::from(char const* value) { return Property(Type::String, value ? std::string(value) : std::string()); }
Property Property::from(uint8_t value) { return Property(Type::Integer, static_cast<int64_t>(value)); }
Property Property::from(uint32_t value) { return Property(Type::Integer, static_cast<int64_t>(value)); }
Property Property::from(int32_t value) { return Property(Type::Integer, static_cast<int64_t>(value)); }
Property Property::from(uint64_t value) { return Property(Type::Integer, static_cast<int64_t>(value)); }
Property Property::from(int64_t value) { return Property(Type::Integer, value); }
Property Property::from(EntityId value) { return Property(Type::Integer, static_cast<int64_t>(value)); }
Property Property::from(float value) { return Property(Type::Number, static_cast<double>(value)); }
Property Property::from(double value) { return Property(Type::Number, value); }
Property Property::from(ActionResult value) { return Property(Type::ActionResult, value); }
Property Property::from(UintVec2 value) { return Property(Type::IntVec2, UintVec2(value.x, value.y)); }
Property Property::from(IntVec2 value) { return Property(Type::IntVec2, value); }
Property Property::from(Direction value) { return Property(Type::Direction, value); }
Property Property::from(Color value) { return Property(Type::Color, value); }
Property Property::from(Property::Type value) { return Property(Type::PropertyType, value); }


bool Property::as_bool(bool default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Boolean: return boost::any_cast<bool>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("bool", type());
}

std::string Property::as_string(std::string default_value) const
{
  switch (type())
  {
    case Type::Null:   return default_value;
    case Type::String: return boost::any_cast<std::string>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("std::string", type());
}

uint8_t Property::as_uint8_t(uint8_t default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<uint8_t>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<uint8_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint8_t", type());
}

uint32_t Property::as_uint32_t(uint32_t default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<uint32_t>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<uint32_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint32_t", type());
}

int32_t Property::as_int32_t(int32_t default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<int32_t>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<int32_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("int32_t", type());
}

uint64_t Property::as_uint64_t(uint64_t default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<uint64_t>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<uint64_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint64_t", type());
}

int64_t Property::as_int64_t(int64_t default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return boost::any_cast<int64_t>(m_value);
    case Type::Number:  return static_cast<int64_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("int64_t", type());
}

float Property::as_float(float default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<float>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<float>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("float", type());
}

double Property::as_double(double default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<double>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return boost::any_cast<double>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("double", type());
}

EntityId Property::as_EntityId(EntityId default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::Integer: return static_cast<EntityId>(boost::any_cast<int64_t>(m_value));
    case Type::Number:  return static_cast<EntityId>(static_cast<uint64_t>((boost::any_cast<double>(m_value))));
    default: break;
  }
  throw InvalidPropertyCastException("EntityId", type());
}

ActionResult Property::as_ActionResult(ActionResult default_value) const
{
  switch (type())
  {
    case Type::Null:         return default_value;
    case Type::ActionResult: return boost::any_cast<ActionResult>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("ActionResult", type());
}

UintVec2 Property::as_UintVec2(UintVec2 default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::IntVec2:
    {
      auto value = boost::any_cast<IntVec2>(m_value);
      return UintVec2(value.x, value.y);
    }
    default: break;
  }
  throw InvalidPropertyCastException("UintVec2", type());
}

IntVec2 Property::as_IntVec2(IntVec2 default_value) const
{
  switch (type())
  {
    case Type::Null:    return default_value;
    case Type::IntVec2: return boost::any_cast<IntVec2>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("IntVec2", type());
}

Direction Property::as_Direction(Direction default_value) const
{
  switch (type())
  {
    case Type::Null:      return default_value;
    case Type::Direction: return boost::any_cast<Direction>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("Direction", type());
}

Color Property::as_Color(Color default_value) const
{
  switch (type())
  {
    case Type::Null:  return default_value;
    case Type::Color: return boost::any_cast<Color>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("Color", type());
}

Property::Type Property::as_PropertyType(Property::Type default_value) const
{
  switch (type())
  {
    case Type::Null:         return default_value;
    case Type::PropertyType: return boost::any_cast<Property::Type>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("PropertyType", type());
}

Property::Type Property::type() const
{
  return m_type;
}

Property const& Property::null()
{
  static Property null_property{};
  return null_property;
}

Property& Property::operator++()
{
  switch (type())
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) + 1LL; break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) + 1.0; break;
    default: throw InvalidPropertyOperationException("operator++", type());
  }
  return *this;
}

Property Property::operator++(int)
{
  Property tmp(*this);
  operator++();
  return tmp;
}

Property& Property::operator--()
{
  switch (m_type)
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) - 1LL; break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) - 1.0; break;
    default: throw InvalidPropertyOperationException("operator--", type());
  }
  return *this;
}

Property Property::operator--(int)
{
  Property tmp(*this);
  operator--();
  return tmp;
}

Property& Property::operator+=(Property const& rhs)
{
  if (type() != rhs.type())
  {
    throw MismatchedPropertyTypesException("operator+=", type(), rhs.type());
  }
  switch (type())
  {
    case Type::String:  m_value = boost::any_cast<std::string>(m_value) + boost::any_cast<std::string>(rhs.m_value); break;
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) + boost::any_cast<int64_t>(rhs.m_value); break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) + boost::any_cast<double>(rhs.m_value); break;
    case Type::IntVec2: m_value = boost::any_cast<IntVec2>(m_value) + boost::any_cast<IntVec2>(rhs.m_value); break;
    case Type::Color:   m_value = boost::any_cast<Color>(m_value) + boost::any_cast<Color>(rhs.m_value); break;
    default: throw InvalidPropertyOperationException("operator+=", type());
  }
  return *this;
}

Property& Property::operator-=(Property const& rhs)
{
  if (type() != rhs.type())
  {
    throw MismatchedPropertyTypesException("operator-=", type(), rhs.type());
  }
  switch (type())
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) - boost::any_cast<int64_t>(rhs.m_value); break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) - boost::any_cast<double>(rhs.m_value); break;
    case Type::IntVec2: m_value = boost::any_cast<IntVec2>(m_value) - boost::any_cast<IntVec2>(rhs.m_value); break;
    case Type::Color:   m_value = boost::any_cast<Color>(m_value) - boost::any_cast<Color>(rhs.m_value); break;
    default: throw InvalidPropertyOperationException("operator-=", type());
  }
  return *this;
}

Property& Property::operator*=(Property const& rhs)
{
  if (type() != rhs.type())
  {
    throw MismatchedPropertyTypesException("operator*=", type(), rhs.type());
  }
  switch (type())
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) * boost::any_cast<int64_t>(rhs.m_value); break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) * boost::any_cast<double>(rhs.m_value); break;
    default: throw InvalidPropertyOperationException("operator*=", type());
  }
  return *this;
}

Property& Property::operator/=(Property const& rhs)
{
  if (type() != rhs.type())
  {
    throw MismatchedPropertyTypesException("operator/=", type(), rhs.type());
  }
  switch (type())
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) / boost::any_cast<int64_t>(rhs.m_value); break;
    case Type::Number:  m_value = boost::any_cast<double>(m_value) / boost::any_cast<double>(rhs.m_value); break;
    default: throw InvalidPropertyOperationException("operator/=", type());
  }
  return *this;
}

Property& Property::operator%=(Property const& rhs)
{
  if (type() != rhs.type())
  {
    throw MismatchedPropertyTypesException("operator%=", type(), rhs.type());
  }
  switch (type())
  {
    case Type::Integer: m_value = boost::any_cast<int64_t>(m_value) % boost::any_cast<int64_t>(rhs.m_value); break;
    case Type::Number:  m_value = static_cast<uint32_t>(boost::any_cast<double>(m_value)) % static_cast<uint32_t>(boost::any_cast<double>(rhs.m_value)); break;
    default: throw InvalidPropertyOperationException("operator%=", type());
  }
  return *this;
}

Property::Property(Property::Type type, boost::any value)
  :
  m_type{ type }, m_value{ value }
{}

