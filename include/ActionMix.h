#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionMix
  :
  public Action
{
public:
  ActionMix(ThingRef subject, std::vector<ThingRef> objects);
  virtual ~ActionMix();

  virtual std::string get_type() const override
  {
    return "Mix";
  }

  ACTION_TRAIT(can_be_subject_verb_things);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
