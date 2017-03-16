#include "stdafx.h"

#include "Property.h"

#include "App.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "Vec2.h"


Property::Property()
  : m_type{ Type::Null }
{}

Property::Property(bool value)
  : m_type{ Type::Boolean }, m_value{ value }
{}

Property::Property(std::string value)
  : m_type{ Type::String }, m_value{ value }
{}

Property::Property(int32_t value)
  : m_type{ Type::Integer }, m_value{ value }
{}

Property::Property(double value)
  : m_type{ Type::Float }, m_value{ value }
{}

Property::Property(Vec2i value)
  : m_type{ Type::Vec2Integer }, m_value{ value }
{}

Property::Property(Direction value)
  : m_type{ Type::Direction }, m_value{ value }
{}

Property::Property(Color value)
  : m_type{ Type::Color }, m_value{ value }
{}

Property::~Property()
{}

template <> bool Property::as()
{
  if (type() == Type::Boolean)
  {
    return boost::any_cast<bool>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> std::string Property::as()
{
  if (type() == Type::String)
  {
    return boost::any_cast<std::string>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> int32_t Property::as()
{
  if (type() == Type::Integer)
  {
    return boost::any_cast<int32_t>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> double Property::as()
{
  if (type() == Type::Float)
  {
    return boost::any_cast<double>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> Vec2i Property::as()
{
  if (type() == Type::Vec2Integer)
  {
    return boost::any_cast<Vec2i>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> Direction Property::as()
{
  if (type() == Type::Direction)
  {
    return boost::any_cast<Direction>(m_value);
  }
  throw InvalidPropertyCastException();
}

template <> Color Property::as()
{
  if (type() == Type::Color)
  {
    return boost::any_cast<Color>(m_value);
  }
  throw InvalidPropertyCastException();
}

Property::Type Property::type()
{
  return m_type;
}

/// @todo This is not thread-safe as written.
void Property::initializeIfNeeded()
{
  if (!s_initialized)
  {
    SET_UP_LOGGER("Property", true);
  }
}


