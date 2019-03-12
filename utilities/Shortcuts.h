/// @file Shortcuts.h
/// @brief Shortcut functions to cut down on typing.
#pragma once

#include <string>

#include "config/Strings.h"
#include "services/Service.h"
#include "services/IMessageLog.h"

// Global helper functions.
inline std::string tr(std::string key)
{
  return Config::strings().get(key);
}

inline void putMsg(std::string message)
{
  S<IMessageLog>().add(message);
}
