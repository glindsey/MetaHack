#include "stdafx.h"

#include "design_patterns/AssertHelper.h"
#include "types/Color.h"
#include "utilities/MathUtils.h"

// === STATIC INSTANCES =======================================================
Color const Color::Transparent;
Color const Color::White{ 0xFFFFFFFF };
Color const Color::Silver{ 0xC0C0C0FF };
Color const Color::Gray{ 0x808080FF };
Color const Color::Black{ 0x000000FF };
Color const Color::Red{ 0xFF0000FF };
Color const Color::Maroon{ 0x800000FF };
Color const Color::Yellow{ 0xFFFF00FF };
Color const Color::Olive{ 0x808000FF };
Color const Color::Lime{ 0x00FF00FF };
Color const Color::Green{ 0x008000FF };
Color const Color::Aqua{ 0x00FFFFFF };
Color const Color::Teal{ 0x008080FF };
Color const Color::Blue{ 0x0000FFFF };
Color const Color::Navy{ 0x000080FF };
Color const Color::Fuschia{ 0xFF00FFFF };
Color const Color::Purple{ 0x800080FF };

Color::Color()
  : m_r{ 0 }, m_g{ 0 }, m_b{ 0 }, m_a{ 0 }
{}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  : m_r{ r }, m_g{ g }, m_b{ b }, m_a{ a }
{}

Color::Color(uint8_t r, uint8_t g, uint8_t b)
  : Color(r, g, b, 255)
{}

Color::Color(uint32_t value)
  : 
  Color((value & 0xFF000000) >> 24, 
        (value & 0x00FF0000) >> 16, 
        (value & 0x0000FF00) >> 8, 
        (value & 0x000000FF))
{}

Color::Color(sf::Color color)
  : Color(color.r, color.g, color.b, color.a)
{}

void to_json(json& j, Color const& color)
{
  j = json::array(
  { 
    "color", 
    static_cast<uint8_t>(color.m_r), 
    static_cast<uint8_t>(color.m_g), 
    static_cast<uint8_t>(color.m_b), 
    static_cast<uint8_t>(color.m_a)
  });
}

void from_json(json const& j, Color& color)
{
  Assert("Types", 
         j.is_array() && (j[0] == "color") && (j.size() >= 4),
         "Attempted to create a Color out of an invalid JSON object");
  
  color.m_r = j[1];
  color.m_g = j[2];
  color.m_b = j[3];

  if (j.size() < 5)
  {
    color.m_a = 255;
  }
  else
  {
    color.m_a = j[4];
  }
}

bool Color::operator==(Color const& other) const
{
  return 
    (m_r == other.m_r) &&
    (m_g == other.m_g) &&
    (m_b == other.m_b) &&
    (m_a == other.m_a);
}

bool Color::operator!=(Color const& other) const
{
  return !(*this == other);
}

uint8_t Color::r() const { return m_r; }
uint8_t Color::g() const { return m_g; }
uint8_t Color::b() const { return m_b; }
uint8_t Color::a() const { return m_a; }

Color& Color::operator+=(Color const& rhs)
{
  m_r += rhs.m_r;
  m_g += rhs.m_g;
  m_b += rhs.m_b;
  m_a += rhs.m_a;
  return *this;
}

Color& Color::operator-=(Color const& rhs)
{
  m_r -= rhs.m_r;
  m_g -= rhs.m_g;
  m_b -= rhs.m_b;
  m_a -= rhs.m_a;
  return *this;
}

Color::operator sf::Color() const
{
  return sf::Color(m_r, m_g, m_b, m_a);
}

Color::operator std::string() const
{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

Color operator+(Color lhs, Color const& rhs)
{
  lhs += rhs;
  return lhs;
}

Color operator-(Color lhs, Color const& rhs)
{
  lhs -= rhs;
  return lhs;
}

/// Average two colors together.
Color average(Color first, Color second)
{
  Color result;
  result.m_r =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_r) +
     static_cast<uint32_t>(second.m_r)) >> 1);
  result.m_g =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_g) +
     static_cast<uint32_t>(second.m_g)) >> 1);
  result.m_b =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_b) +
     static_cast<uint32_t>(second.m_b)) >> 1);
  result.m_a =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_a) +
     static_cast<uint32_t>(second.m_a)) >> 1);
  return result;
}

/// Average four colors together.
Color average(Color first, Color second, Color third, Color fourth)
{
  Color result;
  result.m_r = 
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_r) +
     static_cast<uint32_t>(second.m_r) +
     static_cast<uint32_t>(third.m_r) +
     static_cast<uint32_t>(fourth.m_r)) >> 2);
  result.m_g =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_g) +
     static_cast<uint32_t>(second.m_g) +
     static_cast<uint32_t>(third.m_g) +
     static_cast<uint32_t>(fourth.m_g)) >> 2);
  result.m_b =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_b) +
     static_cast<uint32_t>(second.m_b) +
     static_cast<uint32_t>(third.m_b) +
     static_cast<uint32_t>(fourth.m_b)) >> 2);
  result.m_a =
    static_cast<uint8_t>(
    (static_cast<uint32_t>(first.m_a) +
     static_cast<uint32_t>(second.m_a) +
     static_cast<uint32_t>(third.m_a) +
     static_cast<uint32_t>(fourth.m_a)) >> 2);
  return result;
}

