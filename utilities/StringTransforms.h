#pragma once

#include "stdafx.h"

namespace StringTransforms
{
  std::string squishWhitespace(std::string input);

  bool hasEnding(std::string const& fullString, std::string const& ending);
} // end namespace