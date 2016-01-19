#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionFill
  :
  public Action
{
public:
  ActionFill(ThingRef subject, ThingRef object);
  virtual ~ActionFill();

  virtual std::string get_type() const override
  {
    return "Fill";
  }

  ACTION_TRAIT(can_be_subject_verb_thing_preposition_thing);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
