#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionEat
  :
  public Action, public ActionCRTP<ActionEat>
{
  ACTION_HDR_BOILERPLATE(ActionEat)
    ACTION_TRAIT(can_be_subject_verb_object)

public:
  std::string const get_verbed() const override
  {
    return "ate";
  }

  std::string const get_verb_pp() const override
  {
    return "eaten";
  }

  std::string const get_verbable() const override
  {
    return "edible";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
