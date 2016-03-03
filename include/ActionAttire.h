#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

class ActionAttire
  :
  public Action, public ActionCRTP<ActionAttire>
{
  ACTION_HDR_BOILERPLATE(ActionAttire)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(object_must_be_in_inventory)

public:
  std::string const get_verbed() const override
  {
    return "wore";
  }

  std::string const get_verb_pp() const override
  {
    return "worn";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
