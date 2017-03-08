#pragma once

/// Assertion function.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
/// Not exactly a "design pattern" I know, but goes well here.

#include <iostream>

#ifndef NDEBUG
#define Assert(condition, message) do { std::stringstream os; os << message; _Assert(condition, #condition, __FILE__, __LINE__, os); } while (0)
#else
#define Assert(condition, message)
#endif

namespace
{
  inline void _Assert(bool condition, 
                      char const* condition_string, 
                      char const* file_name, 
                      unsigned int line_number,
                      std::stringstream const& message)
  {
    if (!(condition))                                                            
    {
      std::cerr << "\nAssertion Failed:" << condition_string
        << "\nFile:" << file_name
        << "\nLine:" << line_number
        << "\n" << message.str()
        << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }
}