/// File containing common type declarations.
#pragma once

#include "stdafx.h"

// Declarations of vector classes
#include "types/Rect.h"
#include "types/Vec2.h"
#include "types/Vec3.h"

// Common enums


// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.

/// Elapsed time in the game, in milliseconds.
using ElapsedTime = std::chrono::duration<uint64_t, std::milli>;

/// FileName is, well, a filename.
using FileName = std::string;

using BoolMap = std::unordered_map < std::string, bool >;
using IntMap = std::unordered_map < std::string, int >;
using DoubleMap = std::unordered_map < std::string, double >;
using StringMap = std::unordered_map < std::string, std::string >;
using ColorMap = std::unordered_map < std::string, sf::Color >;
using Vec2Map = std::unordered_map < std::string, IntVec2 >;
using AnyMap = std::unordered_map < std::string, boost::any >;
using TypeMap = std::unordered_map < std::string, std::type_index >;
using CharMap = std::unordered_map < std::string, char >;