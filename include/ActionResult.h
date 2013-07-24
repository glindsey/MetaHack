#ifndef ACTIONRESULT_H
#define ACTIONRESULT_H

// Enum representing possible results from an action.
enum class ActionResult
{
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
  SuccessSelfReference        = 2
};

#endif // ACTIONRESULT_H
