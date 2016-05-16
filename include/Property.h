#pragma once

#include "stdafx.h"

#include "common_types.h"

/// Definition of a Property within the system.
/// Implemented as a tagged union, because frankly I got sick of trying to
/// deal with boost::variant and whatnot. It isn't the most elegant, but it
/// is nice and legible.
class Property
{
public:
  enum class Type
  {
    Number,
    String,
    Boolean,
    Color,
    Vector2i
  };

  Property(Property const& other);
  Property(double number);
  Property(float number);
  Property(int number);
  Property(unsigned int number);
  Property(std::string str);
  Property(StringDisplay str);
  Property(bool boolean);
  Property(sf::Color color);
  Property(sf::Vector2i vector2i);

  Type type();
  std::string type_name();

  operator double();
  operator float();
  operator int();
  operator unsigned int();
  operator std::string();
  operator StringDisplay();
  operator bool();
  operator sf::Color();
  operator sf::Vector2i();

  friend bool operator==(Property const& lhs, Property const& rhs);

  friend bool operator!=(Property const & lhs, Property const & rhs)
  {
    return !(lhs == rhs);
  }

  friend bool operator<(Property const& lhs, Property const& rhs);

  friend bool operator>(Property const& lhs, Property const& rhs)
  {
    return (rhs < lhs);
  }

protected:
private:

  /// Definition of union that contains the property type.
  union PropertyUnion
  {
    PropertyUnion()
    {
      number = 0;
    }

    ~PropertyUnion()
    {}

    double number;
    std::string str;
    bool boolean;
    sf::Color color;
    sf::Vector2i vector2i;
  };

  /// Type of this Property.
  Type m_type;

  /// Data for this Property.
  PropertyUnion m_data;

};