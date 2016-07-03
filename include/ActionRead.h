#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingId.h"

class ActionRead
  :
  public Action, public ActionCRTP<ActionRead>
{
  ACTION_HDR_BOILERPLATE(ActionRead)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(can_be_subject_verb_direction)

public:
  StringDisplay const get_verbed() const override
  {
    return L"read";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
  virtual void print_message_cant_() const override;
};
