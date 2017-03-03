/// File containing common type declarations.

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "stdafx.h"

// Declarations of vector classes
#include "Rect.h"
#include "Vec2.h"
#include "Vec3.h"

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
using Vec2Map = std::unordered_map < std::string, Vec2i >;
using AnyMap = std::unordered_map < std::string, boost::any >;
using TypeMap = std::unordered_map < std::string, std::type_index >;
using CharMap = std::unordered_map < std::string, char >;

#endif // COMMONTYPES_H
