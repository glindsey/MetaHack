#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionUse
  :
  public Action
{
public:
  ActionUse(ThingRef subject, ThingRef object);
  virtual ~ActionUse();

  virtual std::string get_type() const override
  {
    return "Use";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
