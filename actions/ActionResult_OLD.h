#pragma once

#include "stdafx.h"

/// Enum representing possible results from an action.
/// Note that a "Failure*" result doesn't necessarily mean a complete failure,
/// just that things did not go as planned. There may still be side effects
/// resulting from the attempt.
/// @note If this class is changed, remember to also change the method that
///       adds it to Lua in LuaObject.h!
enum class ActionResult
{
  FailureActorCantPerform = -21, ///< Used when an actor is intrinsically incapable of performing an Action
  FailureContainerIsEmpty = -20, ///< Used when trying to drink from a container
  FailureContainerCantBeSelf = -19,
  FailureCircularReference = -18, ///< e.g. Putting a container into itself
  FailureItemNotEquippable = -17,
  FailureItemNotEquipped = -16,
  FailureItemWielded = -15,
  FailureItemEquipped = -14,
  FailureNotEnoughHands = -13,
  FailureTooStupid = -12,
  FailureNotLiquidCarrier = -11,
  FailureSelfReference = -10, ///< Tried to perform invalid action on self
  FailureTargetNotAContainer = -9,
  FailureNotInsideContainer = -8,
  FailureContainerOutOfReach = -7,
  FailureEntityOutOfReach = -6,
  FailureAlreadyPresent = -5,
  FailureInventoryCantContain = -4,
  FailureInventoryFull = -3,
  FailureNotPresent = -2,
  Failure = -1,
  Pending = 0,
  Success = 1,
  SuccessDestroyed = 2,
  SuccessSelfReference = 3,
  SuccessInterrupted = 4, ///< The action started, but was interrupted in the middle.
  SuccessSwapHands = 5  ///< For wield, indicates already-wielded
                                   ///< weapon is just changing hands.
};

inline bool was_successful(ActionResult result)
{
  return static_cast<int>(result) > 0;
}

inline std::ostream& operator<<(std::ostream& os, ActionResult result)
{
  switch (result)
  {
    case ActionResult::FailureActorCantPerform: os << "FailureActorCantPerform"; break;
    case ActionResult::FailureContainerIsEmpty: os << "FailureContainerIsEmpty"; break;
    case ActionResult::FailureContainerCantBeSelf: os << "FailureContainerCantBeSelf"; break;
    case ActionResult::FailureCircularReference: os << "FailureCircularReference"; break;
    case ActionResult::FailureItemNotEquippable: os << "FailureItemNotEquippable"; break;
    case ActionResult::FailureItemNotEquipped: os << "FailureItemNotEquipped"; break;
    case ActionResult::FailureItemWielded: os << "FailureItemWielded"; break;
    case ActionResult::FailureItemEquipped: os << "FailureItemEquipped"; break;
    case ActionResult::FailureNotEnoughHands: os << "FailureNotEnoughHands"; break;
    case ActionResult::FailureTooStupid: os << "FailureTooStupid"; break;
    case ActionResult::FailureNotLiquidCarrier: os << "FailureNotLiquidCarrier"; break;
    case ActionResult::FailureSelfReference: os << "FailureSelfReference"; break;
    case ActionResult::FailureTargetNotAContainer: os << "FailureTargetNotAContainer"; break;
    case ActionResult::FailureNotInsideContainer: os << "FailureNotInsideContainer"; break;
    case ActionResult::FailureContainerOutOfReach: os << "FailureContainerOutOfReach"; break;
    case ActionResult::FailureEntityOutOfReach: os << "FailureEntityOutOfReach"; break;
    case ActionResult::FailureAlreadyPresent: os << "FailureAlreadyPresent"; break;
    case ActionResult::FailureInventoryCantContain: os << "FailureInventoryCantContain"; break;
    case ActionResult::FailureInventoryFull: os << "FailureInventoryFull"; break;
    case ActionResult::FailureNotPresent: os << "FailureNotPresent"; break;
    case ActionResult::Failure: os << "Failure"; break;
    case ActionResult::Pending: os << "Pending"; break;
    case ActionResult::Success: os << "Success"; break;
    case ActionResult::SuccessDestroyed: os << "SuccessDestroyed"; break;
    case ActionResult::SuccessSelfReference: os << "SuccessSelfReference"; break;
    case ActionResult::SuccessSwapHands: os << "SuccessSwapHands"; break;
    default: os << "(Unknown ActionResult" << (unsigned int)result << ")"; break;
  }

  return os;
}
