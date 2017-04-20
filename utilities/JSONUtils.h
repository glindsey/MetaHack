#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace JSONUtils
{

  /// Add data in second JSON object to first JSON object.
  inline void addTo(json& first, json& second)
  {
    json flat_first = first.flatten();

    json flat_second = second.flatten();

    for (auto iter = flat_second.cbegin(); iter != flat_second.cend(); ++iter)
    {
      if (flat_first.count(iter.key()) == 0)
      {
        flat_first[iter.key()] = iter.value();
      }
    }

    first = flat_first.unflatten();
  }

  /// Populate a JSON key if it doesn't already exist.
  /// Returns the contents of the key.
  /// Similar to json::value() except it actually adds the key if it was not
  /// found; value() just returns a default value if it wasn't found, and
  /// does not touch the JSON.
  inline json& addIfMissing(json& j, std::string const& key, json& value)
  {
    if (j.count(key) == 0)
    {
      j[key] = value;
    }

    return j[key];
  }

  /// Set a variable to a value if its key is present in the JSON.
  /// The variable type must have an associated from_json() function or
  /// compilation will fail.
  template <typename T>
  bool setIfPresent(T& variable, json const& j, std::string key)
  {
    if (j.count(key) != 0)
    {
      variable = j[key];
      return true;
    }
    return false;
  }

} // end namespace JSONUtils
