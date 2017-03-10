#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

class ActionClose
  :
  public Action, public ActionCRTP<ActionClose>
{
  ACTION_HDR_BOILERPLATE(ActionClose)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(can_be_subject_verb_direction)

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};