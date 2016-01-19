#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionOpen
  :
  public Action
{
public:
  ActionOpen(ThingRef subject);
  ActionOpen(ThingRef subject, ThingRef object);
  virtual ~ActionOpen();

  virtual std::string get_type() const override
  {
    return "Open";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_be_subject_verb_direction);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
