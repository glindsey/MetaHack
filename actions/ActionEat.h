#pragma once

#include "stdafx.h"

#include "Action.h"
#include "ActionCRTP.h"
#include "EntityId.h"

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

private:
  /// The status of the be_object_of() call, which needs to be saved
  /// for the "finish" portion of the action.
  ActionResult m_last_eat_result;
};
