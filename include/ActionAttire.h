#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionAttire
  :
  public Action, public ActionCRTP<ActionAttire>
{
  ACTION_HDR_BOILERPLATE(ActionAttire)

public:
  ACTION_TRAIT(can_be_subject_verb_object);

  std::string const get_verbed() const override
  {
    return "wore";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
