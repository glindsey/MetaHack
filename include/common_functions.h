#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "stdafx.h"

template<typename T>
StringDisplay wstr(T const& t)
{
  std::wstringstream wstream;
  wstream << t;
  return StringDisplay(wstream.str());
}

template<typename T>
StringKey str(T const& t)
{
  std::stringstream stream;
  stream << t;
  return StringKey(stream.str());
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

template< typename T >
std::string hexify(T i)
{
  std::stringbuf buf;
  std::ostream os(&buf);

  os << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2)
    << std::hex << i;

  return buf.str().c_str();
}

// convert UTF-8 string to wstring
inline std::wstring utf8_to_wstring(const std::string& str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
inline std::string wstring_to_utf8(const std::wstring& str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.to_bytes(str);
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

  /// Stream operator override for sf::String.
  inline ostream& operator<<(ostream& out, sf::String const& str)
  {
    out << str.toAnsiString();
    return out;
  }
}

#endif // COMMON_FUNCTIONS_H
