/// File containing common type declarations.

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "stdafx.h"

// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.

/// StringKey is a string used internally as a key, rather than one that is
/// meant to be displayed on-screen.
/// A StringKey is a normal std::string and therefore supports standard ASCII
/// only.
using StringKey = std::string;

/// StringDisplay is a string meant to be displayed on-screen.
using StringDisplay = sf::String;

/// Definition of a single character taken from StringDisplay.
using CharDisplay = sf::Uint32;

/// FileName is, well, a filename.
using FileName = std::string;

using BoolMap = std::unordered_map < StringKey, bool >;
using IntMap = std::unordered_map < StringKey, int >;
using DoubleMap = std::unordered_map < StringKey, double >;
using StringMap = std::unordered_map < StringKey, StringDisplay >;
using ColorMap = std::unordered_map < StringKey, sf::Color >;
using Vec2Map = std::unordered_map < StringKey, sf::Vector2i >;
using AnyMap = std::unordered_map < StringKey, boost::any >;
using TypeMap = std::unordered_map < StringKey, std::type_index >;
using CharMap = std::unordered_map < StringKey, char >;

#endif // COMMONTYPES_H
