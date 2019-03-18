/// @file Shortcuts.h
/// @brief Shortcut functions to cut down on typing.
#pragma once

#include <string>

#include "config/Strings.h"
#include "game/GameState.h"

// Global helper functions.
inline std::string tr(std::string key)
{
  return Config::strings().get(key);
}

inline void putMsg(std::string message)
{
  GAME.addMessage(message);
}
