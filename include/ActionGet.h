#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionGet
  :
  public Action
{
public:
  ActionGet(ThingRef subject, ThingRef object);
  ActionGet(ThingRef subject, ThingRef object, unsigned int quantity);
  virtual ~ActionGet();

  virtual std::string get_type() const override
  {
    return "Get";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_take_a_quantity);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
