/// File containing common type declarations.
#pragma once

#include <string>
#include <unordered_map>

// Declarations of vector classes
#include "types/Rect.h"
#include "types/Vec2.h"
#include "types/Vec3.h"

// Common enums

// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.

/// Bits32 is a 32-bit bitfield.
using Bits32 = uint32_t;

/// Elapsed time in the game, in hundredths of a second.
using ElapsedTicks = uint64_t;

/// FileName is, well, a filename.
using FileName = std::string;

/// Each Map ID is a string.
/// It used to be a 32-bit integer, but I've realized I'll probably want to
/// support branching maps in the future, for which strings will be easier to
/// manage.
using MapID = std::string;

using String = std::string;

using BoolMap = std::unordered_map<std::string, bool>;
using StringMap = std::unordered_map<std::string, std::string>;