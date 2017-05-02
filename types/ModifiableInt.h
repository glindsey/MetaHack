#pragma once

#include <map>

#include "entity/EntityId.h"
#include "types/common.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Struct containing values for a modifier.
template <typename ModifierType>
struct Modifier
{
  /// Value of this modifier.
  ModifierType value = 0;

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

/// An integer value that can have modifiers tacked onto it.
/// The current value of the integer is calculated as such:
///     value = ((baseValue * preMultipliers) + adders) * postMultipliers
///
/// Pre- and post-multipliers are floats; adders are integers. The resulting
/// value is rounded back to an integer.
class ModifiableInt
{
public:
  ModifiableInt();
  explicit ModifiableInt(int baseValue);

  int value() const;
  
  int baseValue() const;
  void setBaseValue(int baseValue);

  bool setPreMultiplierFrom(EntityId id, Modifier<float> modifier);
  bool hasPreMultiplierFrom(EntityId id);
  bool clearPreMultiplierFrom(EntityId id);
  void clearAllPreMultipliers();

  bool setAdderFrom(EntityId id, Modifier<int> modifier);
  bool hasAdderFrom(EntityId id);
  bool clearAdderFrom(EntityId id);
  void clearAllAdders();

  bool setPostMultiplierFrom(EntityId id, Modifier<float> modifier);
  bool hasPostMultiplierFrom(EntityId id);
  bool clearPostMultiplierFrom(EntityId id);
  void clearAllPostMultipliers();

  void clearAllModifiersFrom(EntityId id);
  void clearAllModifiers();

  friend void from_json(json const& j, ModifiableInt& obj);
  friend void to_json(json& j, ModifiableInt const& obj);

protected:

private:
  /// The base value of the object.
  int m_baseValue;

  /// The set of pre-multiplier modifiers.
  std::map<EntityId, Modifier<float>> m_preMultipliers;

  /// The set of adder modifiers.
  std::map<EntityId, Modifier<int>> m_adders;

  /// The set of post-multiplier modifiers.
  std::map<EntityId, Modifier<float>> m_postMultipliers;
};