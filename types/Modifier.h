#pragma once

#include "types/common.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Struct containing values for a modifier.
template <typename ModifierType>
struct Modifier
{
  /// Value of this modifier.
  ModifierType value = ModifierType();

  /// Game time this modifier expires, or 0 if it never expires.
  ElapsedTime expirationTime = 0;

  /// Reason string for this modifier, if any.
  std::string reason = "";

  friend void from_json(json const& j, Modifier& obj)
  {
    obj.value = j["value"];
    obj.expirationTime = j["expiration-time"];
    obj.reason = j["reason"].get<std::string>();
  }

  friend void to_json(json& j, Modifier const& obj)
  {
    j["value"] = obj.value;
    j["expiration-time"] = obj.expirationTime;
    j["reason"] = obj.reason;
  }
};
