#ifndef ACTIONRESULT_H
#define ACTIONRESULT_H

#include <iostream>

// Enum representing possible results from an action.
enum class ActionResult
{
  FailureContainerCantBeSelf  = -19,
  FailureCircularReference    = -18, ///< e.g. Putting a container into itself
  FailureItemNotEquippable    = -17,
  FailureItemNotEquipped      = -16,
  FailureItemWielded          = -15,
  FailureItemEquipped         = -14,
  FailureNotEnoughHands       = -13,
  FailureTooStupid            = -12,
  FailureNotLiquidCarrier     = -11,
  FailureSelfReference        = -10, ///< Tried to perform invalid action on self
  FailureTargetNotAContainer  = -9,
  FailureNotInsideContainer   = -8,
  FailureContainerOutOfReach  = -7,
  FailureThingOutOfReach      = -6,
  FailureAlreadyPresent       = -5,
  FailureInventoryCantContain = -4,
  FailureInventoryFull        = -3,
  FailureNotPresent           = -2,
  Failure                     = -1,
  Success                     = 0,
  SuccessDestroyed            = 1,
  SuccessSelfReference        = 2,
  SuccessSwapHands            = 3 ///< For wield, indicates already-wielded
                                  ///< weapon is just changing hands.
};

inline std::ostream& operator<<(std::ostream& os, ActionResult result)
{
	switch (result)
	{
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
	case ActionResult::FailureThingOutOfReach: os << "FailureThingOutOfReach"; break;
	case ActionResult::FailureAlreadyPresent: os << "FailureAlreadyPresent"; break;
	case ActionResult::FailureInventoryCantContain: os << "FailureInventoryCantContain"; break;
	case ActionResult::FailureInventoryFull: os << "FailureInventoryFull"; break;
	case ActionResult::FailureNotPresent: os << "FailureNotPresent"; break;
	case ActionResult::Failure: os << "Failure"; break;
	case ActionResult::Success: os << "Success"; break;
	case ActionResult::SuccessDestroyed: os << "SuccessDestroyed"; break;
	case ActionResult::SuccessSelfReference: os << "SuccessSelfReference"; break;
	case ActionResult::SuccessSwapHands: os << "SuccessSwapHands"; break;
	default: os << "(Unknown ActionResult" << (unsigned int)result << ")"; break;
	}

	return os;
}

#endif // ACTIONRESULT_H
