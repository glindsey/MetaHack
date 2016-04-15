#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingId.h"

class ActionFill
  :
  public Action, public ActionCRTP<ActionFill>
{
  ACTION_HDR_BOILERPLATE(ActionFill)
    ACTION_TRAIT(can_be_subject_verb_object_preposition_target)

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
