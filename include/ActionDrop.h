#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionDrop
  :
  public Action, public ActionCRTP<ActionDrop>
{
  ACTION_HDR_BOILERPLATE(ActionDrop)

public:
  std::string const get_verbable() const override
  {
    return "droppable";
  }

  std::string const get_verbed() const override
  {
    return "dropped";
  }

  ACTION_TRAIT(can_be_subject_verb_object);
  ACTION_TRAIT(can_take_a_quantity);

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
