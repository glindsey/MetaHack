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

/// StringKey is a string used internally as a key, rather than one that is
/// meant to be displayed on-screen.
/// It is a synonym for std::string.
using StringKey = std::string;

/// StringDisplay is a string meant to be displayed on-screen.
/// It is a synonym for std::wstring.
using StringDisplay = std::wstring;

/// Definition of a single character taken from StringDisplay.
/// It is a synonym for wchar_t;
using CharDisplay = wchar_t;

/// Game ticks in the game.
using GameTick = uint32_t;

/// FileName is, well, a filename.
using FileName = std::string;

using BoolMap = std::unordered_map < StringKey, bool >;
using IntMap = std::unordered_map < StringKey, int >;
using DoubleMap = std::unordered_map < StringKey, double >;
using StringMap = std::unordered_map < StringKey, StringDisplay >;
using ColorMap = std::unordered_map < StringKey, sf::Color >;
using Vec2Map = std::unordered_map < StringKey, Vec2i >;
using AnyMap = std::unordered_map < StringKey, boost::any >;
using TypeMap = std::unordered_map < StringKey, std::type_index >;
using CharMap = std::unordered_map < StringKey, char >;

#endif // COMMONTYPES_H
