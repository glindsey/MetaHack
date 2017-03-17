#include "stdafx.h"

#include "Property.h"

Property::Property() : m_type{ Type::Null } {}

Property::Property(Type type) : m_type{ type }
{
  switch (m_type)
  {
    case Type::Boolean: m_value = bool(); break;
    case Type::String: m_value = std::string(); break;
    case Type::EightBits:
    case Type::Integer:
    case Type::BigInteger:
    case Type::Number: m_value = double(); break;
    case Type::EntityId: m_value = EntityId(); break;
    case Type::ActionResult: m_value = ActionResult(); break;
    case Type::IntegerVec2: m_value = IntegerVec2(); break;
    case Type::Direction: m_value = Direction(); break;
    case Type::Color: m_value = Color(); break;
    default: break;
  }
}

Property::Property(bool value) : m_type{ Type::Boolean }, m_value{ value } {}
Property::Property(char value) : m_type{ Type::String }, m_value{ std::string(1, value) } {}
Property::Property(std::string value) : m_type{ Type::String }, m_value{ value } {}
Property::Property(char const* value) : m_type{ Type::String }, m_value{ std::string(value) } {}
Property::Property(uint8_t value) : m_type{ Type::EightBits }, m_value{ static_cast<double>(value) } {}
Property::Property(uint32_t value) : m_type{ Type::Integer }, m_value{ static_cast<double>(value) } {}
Property::Property(int32_t value) : m_type{ Type::Integer }, m_value{ static_cast<double>(value) } {}
Property::Property(uint64_t value) : m_type{ Type::BigInteger }, m_value{ static_cast<double>(value) } {}
Property::Property(int64_t value) : m_type{ Type::BigInteger }, m_value{ static_cast<double>(value) } {}
Property::Property(float value) : m_type{ Type::Number }, m_value{ static_cast<double>(value) } {}
Property::Property(double value) : m_type{ Type::Number }, m_value{ value } {}
Property::Property(EntityId value) : m_type{ Type::EntityId }, m_value{ value } {}
Property::Property(ActionResult value) : m_type{ Type::ActionResult }, m_value{ value } {}
Property::Property(IntegerVec2 value) : m_type{ Type::IntegerVec2 }, m_value{ value } {}
Property::Property(Direction value) : m_type{ Type::Direction }, m_value{ value } {}
Property::Property(Color value) : m_type{ Type::Color }, m_value{ value } {}
Property::~Property() {}

bool Property::as(bool default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Boolean: return boost::any_cast<bool>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("bool", type());
}

std::string Property::as(std::string default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::String: return boost::any_cast<std::string>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("std::string", type());
}

uint8_t Property::as(uint8_t default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::EightBits: return static_cast<uint8_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint8_t", type());
}

uint32_t Property::as(uint32_t default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Number: return static_cast<uint32_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint32_t", type());
}

int32_t Property::as(int32_t default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Number: return static_cast<int32_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("int32_t", type());
}

uint64_t Property::as(uint64_t default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::BigInteger: return static_cast<uint64_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("uint64_t", type());
}

int64_t Property::as(int64_t default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::BigInteger: return static_cast<int64_t>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("int64_t", type());
}

float Property::as(float default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Number: return static_cast<float>(boost::any_cast<double>(m_value));
    default: break;
  }
  throw InvalidPropertyCastException("float", type());
}

double Property::as(double default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Number: return boost::any_cast<double>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("float", type());
}

EntityId Property::as(EntityId default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::EntityId: return boost::any_cast<EntityId>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("EntityId", type());
}

ActionResult Property::as(ActionResult default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::ActionResult: return boost::any_cast<ActionResult>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("ActionResult", type());
}

IntegerVec2 Property::as(IntegerVec2 default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::IntegerVec2: return boost::any_cast<IntegerVec2>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("IntegerVec2", type());
}

Direction Property::as(Direction default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
    case Type::Direction: return boost::any_cast<Direction>(m_value);
    default: break;
  }
  throw InvalidPropertyCastException("Direction", type());
}

Color Property::as(Color default_value) const
{
  switch (type())
  {
    case Type::Null: return default_value;
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

Property::operator bool() const
{
  return (type() != Type::Null);
}

bool Property::operator!() const
{
  return !(operator bool());
}

Property& Property::operator++()
{
  switch (type())
  {
    case Type::EightBits: m_value = boost::any_cast<uint8_t>(m_value) + static_cast<uint8_t>(1); break;
    case Type::Number: m_value = boost::any_cast<double>(m_value) + 1.0; break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) + 1L; break;
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
    case Type::EightBits: m_value = boost::any_cast<uint8_t>(m_value) - static_cast<uint8_t>(1); break;
    case Type::Number: m_value = boost::any_cast<double>(m_value) - 1.0; break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) - 1L; break;
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
    case Type::String: m_value = boost::any_cast<std::string>(m_value) + boost::any_cast<std::string>(rhs.m_value); break;
    case Type::EightBits: m_value = boost::any_cast<uint8_t>(m_value) + boost::any_cast<uint8_t>(rhs.m_value); break;
    case Type::Number: m_value = boost::any_cast<double>(m_value) + boost::any_cast<double>(rhs.m_value); break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) + boost::any_cast<uint64_t>(rhs.m_value); break;
    case Type::IntegerVec2: m_value = boost::any_cast<IntegerVec2>(m_value) + boost::any_cast<IntegerVec2>(rhs.m_value); break;
    case Type::Color: m_value = boost::any_cast<Color>(m_value) + boost::any_cast<Color>(rhs.m_value); break;
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
    case Type::EightBits: m_value = boost::any_cast<uint8_t>(m_value) - boost::any_cast<uint8_t>(rhs.m_value); break;
    case Type::Number: m_value = boost::any_cast<double>(m_value) - boost::any_cast<double>(rhs.m_value); break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) - boost::any_cast<uint64_t>(rhs.m_value); break;
    case Type::IntegerVec2: m_value = boost::any_cast<IntegerVec2>(m_value) - boost::any_cast<IntegerVec2>(rhs.m_value); break;
    case Type::Color: m_value = boost::any_cast<Color>(m_value) - boost::any_cast<Color>(rhs.m_value); break;
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
    case Type::Number: m_value = boost::any_cast<double>(m_value) * boost::any_cast<double>(rhs.m_value); break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) * boost::any_cast<uint64_t>(rhs.m_value); break;
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
    case Type::Number: m_value = boost::any_cast<double>(m_value) / boost::any_cast<double>(rhs.m_value); break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) / boost::any_cast<uint64_t>(rhs.m_value); break;
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
    case Type::EightBits: m_value = boost::any_cast<uint8_t>(m_value) % boost::any_cast<uint8_t>(rhs.m_value); break;
    case Type::BigInteger: m_value = boost::any_cast<uint64_t>(m_value) % boost::any_cast<uint64_t>(rhs.m_value); break;
    default: throw InvalidPropertyOperationException("operator%=", type());
  }
  return *this;
}

