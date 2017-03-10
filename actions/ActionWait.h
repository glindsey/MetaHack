#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

class ActionWait
  :
  public Action, public ActionCRTP<ActionWait>
{
  ACTION_HDR_BOILERPLATE(ActionWait)
    ACTION_TRAIT(can_be_subject_only)

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};