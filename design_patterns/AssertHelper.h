#pragma once

#include "third_party/easyloggingpp/easylogging++.h"

/// Assertion function.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
/// Not exactly a "design pattern" I know, but goes well here.

#include <iostream>
#include <sstream>

#define STR(x) #x
/// @todo Change this to choose the logging level instead of just enabled/disabled
#define SET_UP_LOGGER(name, enabled)                                      \
{                                                                         \
  el::Loggers::getLogger(name);                                           \
  el::Configurations conf;                                                \
  conf.setToDefault();                                                    \
  conf.set(el::Level::Global,                                             \
           el::ConfigurationType::Enabled, STR(enabled));                 \
  conf.set(el::Level::Global,                                             \
           el::ConfigurationType::Format,                                 \
           "[%logger] %loc %level: %msg");                                \
  el::Loggers::reconfigureLogger(name, conf);                             \
}

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

void _Assert_Print(char const* logger,
                   char const* condition_string,
                   char const* file_name,
                   unsigned int line_number,
                   std::stringstream const& message);

#else
#define Assert(condition, message)
#endif

