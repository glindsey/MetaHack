#pragma once

#include <map>

#include "entity/EntityId.h"
#include "types/common.h"
#include "types/Modifier.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// An boolean value that can have modifiers tacked onto it.
/// The current value of the boolean is determined by the
/// following truth table:
/// baseValue falseOverride trueOverride  trueComesFirst  result
/// ========= ============= ============= =============== ======
///     F          F              F             F           F
///     F          F              F             T           F
///     F          F              T             F           T
///     F          F              T             T           T
///     F          T              F             F           F
///     F          T              F             T           F
///     F          T              T             F           F
///     F          T              T             T           T
///     T          F              F             F           T
///     T          F              F             T           T
///     T          F              T             F           T
///     T          F              T             T           T
///     T          T              F             F           F
///     T          T              F             T           F
///     T          T              T             F           F
///     T          T              T             T           T
///
///
/// Karnaugh Map:
///
///             False  Override
///              0   0   1   1
///             --- --- --- ---
///          0 | 0   1   0   0 | 0
/// Base     0 | 0   1   1   0 | 1  True Comes
/// Value    1 | 1   1   1   0 | 1    First
///          1 | 1   1   0   0 | 0
///             --- --- --- ---
///              0   1   1   0
///              True Override
///
///     value = (baseValue & ~falseOverride) | (trueOverride & (~falseOverride | trueComesFirst))
///
class bool
{
public:
  bool();
  explicit bool(bool baseValue);

  bool value() const;

  bool baseValue() const;
  void setBaseValue(bool baseValue);

  bool setFalseOverrideFrom(EntityId id, Modifier<bool> modifier);
  bool hasFalseOverrideFrom(EntityId id);
  bool clearFalseOverrideFrom(EntityId id);
  void clearAllFalseOverrides();

  bool setTrueOverrideFrom(EntityId id, Modifier<bool> modifier);
  bool hasTrueOverrideFrom(EntityId id);
  bool clearTrueOverrideFrom(EntityId id);
  void clearAllTrueOverrides();

  bool trueComesFirst() const;
  void setTrueComesFirst(bool trueComesFirst);

  void clearAllOverridesFrom(EntityId id);
  void clearAllOverrides();

  friend void from_json(json const& j, bool& obj);
  friend void to_json(json& j, bool const& obj);

protected:

private:
  /// The base value of the object.
  bool m_baseValue;

  /// The set of false overrides.
  std::map<EntityId, Modifier<bool>> m_falseOverrides;

  /// The set of true overrides.
  std::map<EntityId, Modifier<bool>> m_trueOverrides;

  /// IF true, true overrides will supercede false overrides.
  bool m_trueComesFirst = false;
};
