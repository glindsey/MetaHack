#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

namespace JSONUtils
{

  /// Add data in second JSON object to first JSON object.
  inline void addTo(json & first, json & second)
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

} // end namespace JSONUtils
