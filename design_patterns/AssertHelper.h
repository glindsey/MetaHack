#pragma once

#include "third_party/easyloggingpp/easylogging++.h"

/// Assertion function.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
/// Not exactly a "design pattern" I know, but goes well here.

#include <iostream>
#include <sstream>

#ifndef NDEBUG
#define Assert(logger, condition, message)                                     \
do                                                                             \
{                                                                              \
  if (!(condition))                                                            \
  {                                                                            \
    std::stringstream os;                                                      \
    os << message;                                                             \
    _Assert_Print(logger, #condition, __FILE__, __LINE__, os);                 \
  }                                                                            \
} while (0)

#else
#define Assert(condition, message)
#endif

namespace
{
  inline void _Assert_Print(char const* logger,
                            char const* condition_string,
                            char const* file_name,
                            unsigned int line_number,
                            std::stringstream const& message)
  {
    CLOG(FATAL, logger) << "\nAssertion Failed:" << condition_string
      << "\nFile:" << file_name
      << "\nLine:" << line_number
      << "\n" << message.str();
  }
}