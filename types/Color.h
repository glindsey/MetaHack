#pragma once

#include "stdafx.h"

#include "utilities/MathUtils.h"

#include "SFML/Graphics.hpp"

#include "json.hpp"
using json = ::nlohmann::json;

/// Representation of a color in the game, with variable opacity.
class Color final
{
public:
  Color();
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  Color(uint8_t r, uint8_t g, uint8_t b);
  Color(uint32_t value);
  Color(sf::Color color);

  // Explicit defaults not specifically needed, but here for completeness
  Color(Color const&) = default;
  Color(Color&&) = default;
  Color& operator=(Color const&) = default;
  Color& operator=(Color&&) = default;
  virtual ~Color() = default;

  friend void to_json(json& j, Color const& color);
  friend void from_json(json const& j, Color& color);

  // Equality operators
  bool Color::operator==(Color const& other) const;
  bool Color::operator!=(Color const& other) const;

  // Getters  
  uint8_t r() const;
  uint8_t g() const;
  uint8_t b() const;
  uint8_t a() const;

  // Setters
  template<typename T> uint8_t setR(T value) { m_r = static_cast<uint8_t>(bounds(static_cast<T>(0), value, static_cast<T>(255))); return m_r; }
  template<typename T> uint8_t setG(T value) { m_g = static_cast<uint8_t>(bounds(static_cast<T>(0), value, static_cast<T>(255))); return m_g; }
  template<typename T> uint8_t setB(T value) { m_b = static_cast<uint8_t>(bounds(static_cast<T>(0), value, static_cast<T>(255))); return m_b; }
  template<typename T> uint8_t setA(T value) { m_a = static_cast<uint8_t>(bounds(static_cast<T>(0), value, static_cast<T>(255))); return m_a; }

  template<> uint8_t setR(uint8_t value) { m_r = value; return m_r; }
  template<> uint8_t setG(uint8_t value) { m_g = value; return m_g; }
  template<> uint8_t setB(uint8_t value) { m_b = value; return m_b; }
  template<> uint8_t setA(uint8_t value) { m_a = value; return m_a; }

  // Mutator operators
  Color& operator+=(Color const& rhs);
  Color& operator-=(Color const& rhs);
  //Color& operator*=(float const& rhs);
  Color& operator<<=(unsigned int const& rhs);
  Color& operator>>=(unsigned int const& rhs);

  friend Color operator+(Color lhs, Color const& rhs);
  friend Color operator-(Color lhs, Color const& rhs);
  //friend Color operator*(Color lhs, float const& rhs);
  friend Color operator>>(Color lhs, unsigned int const& rhs);
  friend Color operator<<(Color lhs, unsigned int const& rhs);

  // Casts
  operator sf::Color() const;
  operator std::string() const;

  // Other methods/functions
  friend Color average(Color first, Color second);
  friend Color average(Color first, Color second, Color third, Color fourth);

  // Statics used for common colors
  static Color const Transparent;
  static Color const White;
  static Color const Silver;
  static Color const Gray;
  static Color const Black;
  static Color const Red;
  static Color const Maroon;
  static Color const Yellow;
  static Color const Olive;
  static Color const Lime;
  static Color const Green;
  static Color const Aqua;
  static Color const Teal;
  static Color const Blue;
  static Color const Navy;
  static Color const Fuschia;
  static Color const Purple;

private:
  uint8_t m_r;
  uint8_t m_g;
  uint8_t m_b;
  uint8_t m_a;
};

inline std::ostream& operator<<(std::ostream& os, Color const& c)
{
  os << "(" << c.r() << ", " << c.g() << ", " << c.b() << ", " << c.a() << ")";
  return os;
}

// JSON Specializations for the sf::Color type.
//namespace sf
//{
//  void to_json(json& j, Color const& color)
//  {
//    j = {
//      { "type", "color" },
//      { "r", color.r },
//      { "g", color.g },
//      { "b", color.b },
//      { "a", color.a }
//    };
//  }
//
//  void from_json(json const& j, Color& color)
//  {
//    color.r = j["r"];
//    color.g = j["g"];
//    color.b = j["b"];
//    color.a = j["a"];
//  }
//}