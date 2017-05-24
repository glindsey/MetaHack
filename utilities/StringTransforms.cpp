#include "stdafx.h"

#include "utilities/StringTransforms.h"

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
} // end namespace