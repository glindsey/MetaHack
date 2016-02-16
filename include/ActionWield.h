#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionWield
  :
  public Action
{
public:
  explicit ActionWield(ThingRef subject);
  ActionWield(ThingRef subject, ThingRef object);
  virtual ~ActionWield();

  virtual std::string get_type() const override
  {
    return "Wield";
  }

  ACTION_TRAIT(can_be_subject_only);
  ACTION_TRAIT(can_be_subject_verb_thing);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
