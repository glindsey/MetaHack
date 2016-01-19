#pragma once

#include "Action.h"
#include "ThingRef.h"

#include <string>
#include <vector>

class ActionHurl
  :
  public Action
{
public:
  ActionHurl(ThingRef subject, ThingRef object);
  virtual ~ActionHurl();

  virtual std::string get_type() const override
  {
    return "Hurl";
  }

  ACTION_TRAIT(can_be_subject_verb_thing_preposition_direction);

protected:
  virtual StateResult do_prebegin_work(AnyMap& params) override;
  virtual StateResult do_begin_work(AnyMap& params) override;
  virtual StateResult do_finish_work(AnyMap& params) override;
  virtual StateResult do_abort_work(AnyMap& params) override;
};
