#pragma once

#include "third_party/easyloggingpp/easylogging++.h"

#include <iostream>
#include <sstream>

#define STR(x) #x

/// Set up an individual logger.
/// @param name     Name of the logger.
/// @param enabled  True if trace/debug logs should be printed, false if
///                 only standard logs should be printed.
#define SET_UP_LOGGER(name, enabled)                                      \
{                                                                         \
  el::Loggers::getLogger(name);                                           \
  el::Configurations conf;                                                \
  conf.setToDefault();                                                    \
  conf.set(el::Level::Global,                                             \
           el::ConfigurationType::Enabled, "true");                       \
  conf.set(el::Level::Trace,                                              \
           el::ConfigurationType::Enabled, STR(enabled));                 \
  conf.set(el::Level::Debug,                                              \
           el::ConfigurationType::Enabled, STR(enabled));                 \
  conf.set(el::Level::Global,                                             \
           el::ConfigurationType::Format,                                 \
           "[%logger] %loc %level: %msg");                                \
  el::Loggers::reconfigureLogger(name, conf);                             \
}

/// Assert function adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
/// Not exactly a "design pattern" I know, but goes well here.

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

