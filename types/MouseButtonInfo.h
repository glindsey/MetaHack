#pragma once

#include "types/Vec2.h"

struct MouseButtonInfo
{
  /// Whether this button is pressed.
  bool pressed;

  /// Whether a drag is currently in process with this button.
  bool dragging;

  /// Absolute location of the press or release.
  IntVec2 location;

  /// Time elapsed since the last button state change.
  /// @todo Decouple from SFML
  sf::Clock elapsed;
};
