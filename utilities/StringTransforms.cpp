#include "utilities/StringTransforms.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace StringTransforms
{
  std::string squishWhitespace(std::string input)
  {
    std::string output;
    std::unique_copy(input.begin(), input.end(),
                     std::back_insert_iterator<std::string>(output),
                     [](char a, char b) { return std::isspace(a) && std::isspace(b); });
    return output;
  }

  bool hasEnding(std::string const& fullString, std::string const& ending)
  {
    if (fullString.length() >= ending.length())
    {
      return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
      return false;
    }
  }

  StringPair splitName(std::string name)
  {
    size_t dotLocation = name.find('.');
    StringPair stringPair;

    if ((dotLocation != std::string::npos) && (dotLocation != name.size() - 1))
    {
      stringPair.first = name.substr(0, dotLocation);
      stringPair.second = name.substr(dotLocation + 1);
    }
    else
    {
      stringPair.first = "";
      stringPair.second = name;
    }

    return stringPair;
  }

} // end namespace
