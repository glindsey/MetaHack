#ifndef COMMON_ENUMS_H_INCLUDED
#define COMMON_ENUMS_H_INCLUDED

/// Enum class for results that SFML events can return.
enum class SFMLEventResult
{
  Pending,      ///< The event has not been handled by any handler yet; only for variable initialization.
  Handled,      ///< The event was handled and should not be passed on
  Acknowledged, ///< The event was handled, but should still be passed on
  Ignored,      ///< The event was ignored, and should be passed on
  Unknown       ///< The event type is unknown; further action is at handler's discretion
};

#endif // COMMON_ENUMS_H_INCLUDED
