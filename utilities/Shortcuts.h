/// @file Shortcuts.h
/// @brief Shortcut functions to cut down on typing.
#pragma once

#include <string>

#include "services/Service.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"

// Global helper functions.
inline std::string tr(std::string key)
{
  return Service<IStringDictionary>::get().get(key);
}

inline void putMsg(std::string message)
{
  Service<IMessageLog>::get().add(message);
}