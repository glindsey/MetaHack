#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

class ActionDie
  :
  public Action, public ActionCRTP<ActionDie>
{
  ACTION_HDR_BOILERPLATE(ActionDie)
    ACTION_TRAIT(can_be_subject_only)
    ACTION_TRAIT(subject_can_be_in_limbo)

public:
  virtual std::string const get_verbable() const override
  {
    return "mortal";
  }

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
