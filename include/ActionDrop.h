#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionDrop
  :
  public Action
{
public:
  ActionDrop(ThingRef subject, ThingRef object);
  ActionDrop(ThingRef subject, ThingRef object, unsigned int quantity);
  virtual ~ActionDrop();

  virtual std::string get_type() const override
  {
    return "Drop";
  }

  ACTION_TRAIT(can_be_subject_verb_thing);
  ACTION_TRAIT(can_take_a_quantity);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
