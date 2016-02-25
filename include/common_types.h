/// File containing common type declarations.

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <boost/any.hpp>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.
using BoolMap = std::unordered_map < std::string, bool >;
using IntMap = std::unordered_map < std::string, int >;
using StringMap = std::unordered_map < std::string, std::string >;
using AnyMap = std::unordered_map < std::string, boost::any >;
using TypeMap = std::unordered_map < std::string, std::type_index >;
using CharMap = std::unordered_map < std::string, char >;

#endif // COMMONTYPES_H
