#pragma once

#include <string>
#include <utility>

/// Using declarations
using StringPair = std::pair<std::string, std::string>;

namespace StringTransforms
{
  std::string squishWhitespace(std::string input);

  bool hasEnding(std::string const& fullString, std::string const& ending);

  /// Split a qualified name into a group/subname pair.
  StringPair splitName(std::string name);

} // end namespace
