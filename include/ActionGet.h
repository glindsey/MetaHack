#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionGet
  :
  public Action, public ActionCRTP<ActionGet>
{
  ACTION_HDR_BOILERPLATE(ActionGet)
    ACTION_TRAIT(can_be_subject_verb_object)
    ACTION_TRAIT(can_take_a_quantity)

public:
  std::string const get_verbing() const override
  {
    return "getting";
  }

  std::string const get_verbed() const override
  {
    return "got";
  }

  std::string const get_verb_pp() const override
  {
    return "gotten";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
