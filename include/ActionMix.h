#pragma once

#include "Action.h"
#include "ActionCRTP.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionMix
  :
  public Action, public ActionCRTP<ActionMix>
{
public:
  explicit ActionMix(ThingRef subject);
  virtual ~ActionMix();

  virtual std::string get_type() const override
  {
    return "Mix";
  }

  ACTION_TRAIT(can_be_subject_verb_things);

protected:
  virtual StateResult do_prebegin_work_(AnyMap& params) override;
  virtual StateResult do_begin_work_(AnyMap& params) override;
  virtual StateResult do_finish_work_(AnyMap& params) override;
  virtual StateResult do_abort_work_(AnyMap& params) override;
};
