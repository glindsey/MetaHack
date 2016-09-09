#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingId.h"

class ActionHurl
  :
  public Action, public ActionCRTP<ActionHurl>
{
  ACTION_HDR_BOILERPLATE(ActionHurl)
    ACTION_TRAIT(can_be_subject_verb_object_preposition_direction)

public:
  std::string const get_verbed() const override
  {
    return "threw";
  }

  std::string const get_verb_pp() const override
  {
    return "thrown";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
