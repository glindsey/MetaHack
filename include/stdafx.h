#pragma once

// Standard library includes
#include <algorithm>
#include <cctype>
#include <cinttypes>
#include <climits>
#include <crtdbg.h>
#include <cstdbool>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <queue>
#include <regex>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <vector>

// Boost includes
#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
#include <boost/bimap.hpp>
#include <boost/core/demangle.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/serialization/strong_typedef.hpp>

// Cereal includes
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

// SFML includes
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// Lua includes
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
