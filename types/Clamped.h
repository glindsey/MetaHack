#pragma once



#include "stdint.h"

#include "utilities/MathUtils.h"

/// Representation of an 8-bit unsigned value (0-255) that uses clamped
/// arithmetic (e.g. results of operations are clamped to 0 or 255 if
/// they exceed the boundaries).
///
/// Branchfree clamped arithmetic adapted from:
/// https://locklessinc.com/articles/sat_arithmetic/
class Clamped8
{
public:
  Clamped8() : m_value{ 0 } {}
  Clamped8(uint8_t value) : m_value{ value } {}
  Clamped8(uint16_t value) : m_value{ static_cast<uint8_t>(std::min(value, uint16_t(255))) } {}
  Clamped8(uint32_t value) : m_value{ static_cast<uint8_t>(std::min(value, 255U)) } {}
  Clamped8(int16_t value) : m_value{ static_cast<uint8_t>(bounds(int16_t(0), value, int16_t(255))) } {}
  Clamped8(int32_t value) : m_value{ static_cast<uint8_t>(bounds(0, value, 255)) } {}
  Clamped8(float value) : m_value{ static_cast<uint8_t>(bounds(0.0f, value, 255.0f)) } {}
  Clamped8(double value) : m_value{ static_cast<uint8_t>(bounds(0.0, value, 255.0)) } {}

  // Explicit defaults not specifically needed, but here for completeness
  Clamped8(Clamped8 const&) = default;
  Clamped8(Clamped8&&) = default;
  Clamped8& operator=(Clamped8 const&) = default;
  Clamped8& operator=(Clamped8&&) = default;
  virtual ~Clamped8() = default;
  
  friend void to_json(json& j, Clamped8 const& clamped)
  {
    j = clamped.m_value;
  }

  friend void from_json(json const& j, Clamped8& clamped)
  {
    clamped.m_value = j;
  }

  // Conversion operators
  inline operator uint8_t() const
  {
    return m_value;
  }

  // Equality operators
  inline bool operator==(Clamped8 const& other) const
  {
    return (m_value == other.m_value);
  }

  inline bool operator!=(Clamped8 const& other) const
  {
    return !(*this == other);
  }

  inline Clamped8& Clamped8::operator+=(Clamped8 const& rhs)
  {
    uint8_t result = m_value + rhs.m_value;
    result |= -(result < m_value);

    m_value = result;
    return *this;
  }

  inline Clamped8& Clamped8::operator-=(Clamped8 const& rhs)
  {
    uint8_t result = m_value - rhs.m_value;
    result &= -(result <= m_value);

    m_value = result;
    return *this;
  }

  inline Clamped8& Clamped8::operator*=(Clamped8 const& rhs)
  {
    uint16_t result = static_cast<uint16_t>(m_value) * static_cast<uint16_t>(rhs.m_value);

    uint8_t hi = static_cast<uint8_t>(result >> 8);
    uint8_t lo = static_cast<uint8_t>(result);

    m_value = (lo | -!!hi);
    return *this;
  }

  inline Clamped8& Clamped8::operator/=(Clamped8 const& rhs)
  {
    m_value = (m_value / rhs.m_value);
    return *this;
  }

  friend Clamped8 operator+(Clamped8 lhs, Clamped8 const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend Clamped8 operator-(Clamped8 lhs, Clamped8 const& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend Clamped8 operator*(Clamped8 lhs, Clamped8 const& rhs)
  {
    lhs *= rhs;
    return lhs;
  }

  friend Clamped8 operator/(Clamped8 lhs, Clamped8 const& rhs)
  {
    lhs /= rhs;
    return lhs;
  }

protected:

private:
  uint8_t m_value;
};