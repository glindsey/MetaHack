#include "stdafx.h"

#include "Property.h"

Property::Property() : m_type{ Type::Null } {}

Property::Property(Type type) : m_type{ type }
{
  switch (m_type)
  {
    case Type::Boolean:      m_value = bool(); break;
    case Type::String:       m_value = std::string(); break;
    case Type::Integer:      m_value = int64_t(); break;
    case Type::Number:       m_value = double(); break;
    case Type::ActionResult: m_value = ActionResult(); break;
    case Type::IntVec2:      m_value = IntVec2(); break;
    case Type::Direction:    m_value = Direction(); break;
    case Type::Color:        m_value = Color(); break;
    default: break;
  }
}

Property::Property(bool value)         : m_type{ Type::Boolean }, m_value{ value } {}
Property::Property(char value)         : m_type{ Type::String }, m_value{ std::string(1, value) } {}
Property::Property(std::string value)  : m_type{ Type::String }, m_value{ value } {}
Property::Property(char const* value)  : m_type{ Type::String }, m_value{ value ? std::string(value) : std::string() } {}
Property::Property(uint8_t value)      : m_type{ Type::Integer }, m_value{ static_cast<int64_t>(value) } {}
Property::Property(uint32_t value)     : m_type{ Type::Integer }, m_value{ static_cast<int64_t>(value) } {}
Property::Property(int32_t value)      : m_type{ Type::Integer }, m_value{ static_cast<int64_t>(value) } {}
Property::Property(uint64_t value)     : m_type{ Type::Integer }, m_value{ static_cast<int64_t>(value) } {}
Property::Property(int64_t value)      : m_type{ Type::Integer }, m_value{ value } {}
Property::Property(EntityId value)     : m_type{ Type::Integer }, m_value{ static_cast<int64_t>(value) } {}
Property::Property(float value)        : m_type{ Type::Number }, m_value{ static_cast<double>(value) } {}
Property::Property(double value)       : m_type{ Type::Number }, m_value{ value } {}
Property::Property(ActionResult value) : m_type{ Type::ActionResult }, m_value{ value } {}
Property::Property(UintVec2 value)     : m_type{ Type::IntVec2 }, m_value{ UintVec2(value.x, value.y) } {}
Property::Property(IntVec2 value)      : m_type{ Type::IntVec2 }, m_value{ value } {}
Property::Property(Direction value)    : m_type{ Type::Direction }, m_value{ value } {}
Property::Property(Color value)        : m_type{ Type::Color }, m_value{ value } {}
Property::~Property() {}

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
    case Type::Number:  return static_cast<EntityId>(boost::any_cast<double>(m_value));
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

