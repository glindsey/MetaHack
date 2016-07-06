#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingId.h"

#include <string>
#include <vector>

/// Action: Turn to face a particular direction.
class ActionTurn
  :
  public Action, public ActionCRTP<ActionTurn>
{
  ACTION_HDR_BOILERPLATE(ActionTurn)
    ACTION_TRAIT(can_be_subject_verb_direction)

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
