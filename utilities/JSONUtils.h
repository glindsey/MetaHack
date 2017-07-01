#pragma once

#include <set>

#include "AssertHelper.h"
#include "utilities/RNGUtils.h"

#include "json.hpp"
using json = ::nlohmann::json;

namespace JSONUtils
{

  /// Copy data in second JSON object into first JSON object.
  /// If any key exists already in the first JSON object, it will be
  /// *overwritten* with the second's value.
  inline void overwriteKeys(json& first, json& second)
  {
    json flat_first = first.flatten();

    json flat_second = second.flatten();

    for (auto iter = flat_second.cbegin(); iter != flat_second.cend(); ++iter)
    {
      flat_first[iter.key()] = iter.value();
    }

    first = flat_first.unflatten();
  }

  /// Add data in second JSON object to first JSON object.
  /// If any key exists already in the first JSON object, it will be skipped.
  inline void addMissingKeys(json& first, json& second)
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

  /// Merge second JSON array into first JSON array.
  /// Duplicate values are discarded.
  /// This algorithm has O(n^2) complexity, so it shouldn't be used on
  /// really big arrays.
  /// @todo Add a hint as to where the second array should be inserted into
  ///       the first. This is important so that templates are traversed in
  ///       the order we want when entity graphics are searched for.
  inline void mergeArrays(json& first, json& second)
  {
    if (!first.is_array() || !second.is_array()) return;

    for (size_t index2 = 0; index2 < second.size(); ++index2)
    {
      bool alreadyExists = false;
      for (size_t index1 = 0; index1 < first.size(); ++index1)
      {
        if (first[index1] == second[index2])
        {
          alreadyExists = true;
          break;
        }
      }
      if (!alreadyExists)
      {
        first.push_back(second[index2]);
      }
    }
  }

  /// Populate a JSON key if it doesn't already exist.
  /// Returns the new contents of the key.
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

  /// Given a source and a target JSON object, and a key, copy the key to the
  /// target object if it exists in the source and is non-null. If the key
  /// exists in the source and *is* null, delete the key in the target object.
  /// If the key does not exist in the source, do nothing.
  /// @warning NOT tested yet.
  inline void update(json const& source, std::string const& key, json& target)
  {
    if (!source.is_object() || !target.is_object()) return;

    if (source.count(key) != 0)
    {
      auto const& value = source.at(key);
      if (!value.is_null())
      {
        target[key] = value;
      }
      else
      {
        target.erase(key);
      }
    }
  }

  /// Perform an action with a key/value pair if the key is present in the JSON.
  inline void doIfPresent(json const& j,
                          std::string key,
                          std::function<void(json const& value)> function)
  {
    if (j.is_object() && j.count(key) != 0)
    {
      json const& value = j[key];
      function(value);
    }
  }

  /// Given a JSON value, get either a value or an array of values into a vector.
  /// If the JSON value is an object, do nothing -- we can't handle that.
  /// If the value is an array, iterate over it and push the values into the vector.
  /// Otherwise, just push the single value into the vector.
  template <typename T>
  std::vector<T> getVector(json const& j)
  {
    std::vector<T> result;
    if (j.is_array())
    {
      for (auto value : j)
      {
        result.push_back(value.get<T>());
      }
    }
    else if (!j.is_object())
    {
      result.push_back(j.get<T>());
    }

    return result;
  }

  /// Given a JSON value, get either a value or an array of values into a set.
  /// If the JSON value is null or an object, do nothing -- we can't handle that.
  /// If the value is an array, iterate over it and push the values into the vector.
  /// Otherwise, just push the single value into the vector.
  template <typename T>
  std::set<T> getSet(json const& j)
  {
    std::set<T> result;
    if (!j.is_null())
    {
      if (j.is_array())
      {
        for (auto value : j)
        {
          result.insert(value.get<T>());
        }
      }
      else if (!j.is_object())
      {
        result.insert(j.get<T>());
      }
    }
    return result;
  }

  /// Do a fancypants read of a value to get an integer.
  /// If the value is a number, just set it directly.
  /// If the value is an array, set the integer to a random number between the two.
  /// This function can be expanded to do other stuff later as well.
  inline int getFancyInteger(json const& j)
  {
    int result = 0;

    if (j.is_array())
    {
      if (j.size() >= 2)
      {
        result = pick_uniform(j[0].get<int>(), j[1].get<int>());
      }
      else
      {
        result = j[0].get<int>();
      }
    }
    else if (j.is_object())
    {
      CLOG(WARNING, "Utilities") << "JSON object was passed in, which is unsupported -- returning 0";      
    }
    else
    {
      result = j.get<int>();
    }

    return result;
  }

} // end namespace JSONUtils
