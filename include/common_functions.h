#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <boost/algorithm/string.hpp>
#include <string>

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

#endif // COMMON_FUNCTIONS_H
