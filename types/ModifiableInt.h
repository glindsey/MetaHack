#pragma once

#include <map>

#include "entity/EntityId.h"
#include "types/common.h"
#include "types/Modifier.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// An integer value that can have modifiers tacked onto it.
/// The current value of the integer is calculated as such:
///     value = ((baseValue * preMultipliers) + adders) * postMultipliers
///
/// Pre- and post-multipliers are floats; adders are integers. The resulting
/// value is rounded back to an integer.
class int
{
public:
  int();
  explicit int(int baseValue);

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

  friend void from_json(json const& j, int& obj);
  friend void to_json(json& j, int const& obj);

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
