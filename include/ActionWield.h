#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

class ActionWield
  :
  public Action, public ActionCRTP<ActionWield>
{
  ACTION_HDR_BOILERPLATE(ActionWield)
    ACTION_TRAIT(can_be_subject_only)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(object_must_be_in_inventory)

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
