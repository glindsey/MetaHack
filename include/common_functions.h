#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "stdafx.h"

template<typename T>
std::string str(T const& t)
{
  std::stringstream stream;
  stream << t;
  return stream.str();
}

inline bool strip_quotes(std::string& str)
{
  boost::trim(str);
  if (str.front() == '\"' && str.back() == '\"')
  {
    // Strip off the first and last quote characters.
    std::string new_str = str.substr(1, str.length() - 2);
    str = new_str;
    return true;
  }
  return false;
}

inline std::string strip_quotes_copy(std::string str)
{
  boost::trim(str);
  if (str.front() == '\"' && str.back() == '\"')
  {
    // Strip off the first and last quote characters.
    std::string new_str = str.substr(1, str.length() - 2);
    return new_str;
  }
  else
  {
    return str;
  }
}

namespace std
{
  /// Stream operator override for sf::Vector2<T>.
  template <typename T>
  ostream& operator<<(ostream& out, sf::Vector2<T> const& vec)
  {
    out << "(" << vec.x << ", " << vec.y << ")";
    return out;
  }

  /// Stream operator override for sf::Vector3<T>.
  template <typename T>
  ostream& operator<<(ostream& out, sf::Vector3<T> const& vec)
  {
    out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out;
  }

  /// Stream operator override for sf::Color.
  inline ostream& operator<<(ostream& out, sf::Color const& vec)
  {
    out << "(" << vec.r << ", " << vec.g << ", " << vec.b << ", " << vec.a << ")";
    return out;
  }

  /// Stream operator override for sf::IntRect.
  inline ostream& operator<<(ostream& out, sf::IntRect const& rect)
  {
    out << "pos = (" << rect.left << ", " << rect.top <<
      "), size = (" << rect.width << " x " << rect.height << ")";
    return out;
  }
}

#endif // COMMON_FUNCTIONS_H
