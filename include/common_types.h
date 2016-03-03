/// File containing common type declarations.

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "stdafx.h"

// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.
using BoolMap = std::unordered_map < std::string, bool >;
using IntMap = std::unordered_map < std::string, int >;
using DoubleMap = std::unordered_map < std::string, double >;
using StringMap = std::unordered_map < std::string, std::string >;
using ColorMap = std::unordered_map < std::string, sf::Color >;
using Vec2Map = std::unordered_map < std::string, sf::Vector2i >;
using AnyMap = std::unordered_map < std::string, boost::any >;
using TypeMap = std::unordered_map < std::string, std::type_index >;
using CharMap = std::unordered_map < std::string, char >;

#endif // COMMONTYPES_H
