#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingId.h"

class ActionShoot
  :
  public Action, public ActionCRTP<ActionShoot>
{
  ACTION_HDR_BOILERPLATE(ActionShoot)
    ACTION_TRAIT(can_be_subject_verb_object_preposition_direction)

public:
  StringDisplay const get_verbed() const override
  {
    return "shot";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
