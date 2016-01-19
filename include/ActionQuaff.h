#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionQuaff
  :
  public Action
{
public:
  ActionQuaff(ThingRef subject);
  ActionQuaff(ThingRef subject, ThingRef object);
  virtual ~ActionQuaff();

  virtual std::string get_type() const override
  {
    return "Quaff";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_be_subject_verb_direction);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
