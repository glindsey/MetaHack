/// File containing common type declarations.

#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <unordered_map>

// Using declarations
// Note: std::unordered_map is faster than std::map, but it obviously means
//       that obtaining a sorted list will be a bit more difficult. However,
//       there won't be many times we NEED a sorted list.
using FlagsMap = std::unordered_map < std::string, bool >;
using ValuesMap = std::unordered_map < std::string, int >;
using StringsMap = std::unordered_map < std::string, std::string >;


#endif // COMMONTYPES_H
