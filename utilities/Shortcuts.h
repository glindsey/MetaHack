/// @file Shortcuts.h
/// @brief Shortcut functions to cut down on typing.
#pragma once

#include <string>

#include "services/Service.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"

// Global helper functions.
inline std::string tr(std::string key)
{
  return S<IStrings>().get(key);
}

inline void putMsg(std::string message)
{
  S<IMessageLog>().add(message);
}